/*
      <AdditionalIncludeDirectories>
      C:\Data\Git\electrumsv\contrib\build-windows\amd64\Python-3.10.0\Include
      C:\Data\Git\electrumsv\contrib\build-windows\amd64\Python-3.10.0\PC
    </ClCompile>
    <Link>
      <AdditionalLibraryDirectories>
        C:\Data\Git\electrumsv\contrib\build-windows\amd64\Python-3.10.0\PCbuild\amd64

*/


// #ifdef _MSC_VER
// #pragma warning(disable : 4996)
// #endif

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#ifdef __APPLE__
#include <malloc/malloc.h>
#endif

#include "cuckoofilter.h"

using cuckoofilter::CuckooFilter;
using cuckoofilter::Status;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    CuckooFilter<const void*, 12> *filter;
} CuckooFilterObject;


static void
refcuckoo_dealloc(CuckooFilterObject *self)
{
    delete self->filter;
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
refcuckoo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    CuckooFilterObject *self;
    self = (CuckooFilterObject *) type->tp_alloc(type, 0);
    if (self) {
        self->filter = NULL;
    }
    return (PyObject *) self;
}

static int
refcuckoo_init(CuckooFilterObject *self, PyObject *args, PyObject *kwds)
{
    uint32_t max_key_count;

    if (!PyArg_ParseTuple(args, "k", &max_key_count))
        return -1;

    if (self->filter != NULL) {
        delete self->filter;
        self->filter = NULL;
    }

    self->filter = new CuckooFilter<const void *, 12>(max_key_count);
    return 0;
}

static PyObject *
refcuckoo_get_memory_size(CuckooFilterObject *self, void *closure)
{
    size_t memory_size = self->filter->SizeInBytes();
    return PyLong_FromLong((long)memory_size);
}

static PyObject *
refcuckoo_get_item_count(CuckooFilterObject *self, void *closure)
{
    size_t item_count = self->filter->Size();
    return PyLong_FromLong((long)item_count);
}

static PyObject *
refcuckoo_get_victim(CuckooFilterObject *self, void *closure)
{
    size_t index;
    uint32_t tag;
    uint32_t used;
    self->filter->VictimDetails(&used, &index, &tag);

    if (used == 0)
        Py_RETURN_NONE;

    PyObject *index_object = PyLong_FromUnsignedLong((uint32_t)index);
    if (index_object == NULL) {
        PyErr_SetObject(PyExc_Exception, PyUnicode_FromString("Error allocating 'index' object."));
        return NULL;
    }

    PyObject *tag_object = PyLong_FromUnsignedLong(tag);
    if (tag_object == NULL) {
        Py_DECREF(index_object);
        PyErr_SetObject(PyExc_Exception, PyUnicode_FromString("Error allocating 'tag' object."));
        return NULL;
    }

    return PyTuple_Pack(2, index_object, tag_object);
}

static PyGetSetDef refcuckoo_getsets[] = {
    {
        "item_count",
        (getter) refcuckoo_get_item_count,
        NULL,
        NULL,
        NULL
    },
    {
        "memory_size",
        (getter) refcuckoo_get_memory_size,
        NULL,
        NULL,
        NULL
    },
    {
        "victim",
        (getter) refcuckoo_get_victim,
        NULL,
        NULL,
        NULL
    },
    { NULL } /* Sentinel */
};

static PyObject *
refcuckoo_add(CuckooFilterObject *self, PyObject *args)
{
    void *key;
    Py_ssize_t key_length;

    if (!PyArg_ParseTuple(args, "y#", &key, &key_length))
        return NULL;

    Status status = self->filter->Add(key, key_length);
    return PyLong_FromLong(status);
}

static PyObject *
refcuckoo_contains(CuckooFilterObject *self, PyObject *args)
{
    void *key;
    Py_ssize_t key_length;

    if (!PyArg_ParseTuple(args, "y#", &key, &key_length))
        return NULL;

    Status status = self->filter->Contain(key, key_length);
    return PyLong_FromLong(status);
}

// static PyObject *
// refcuckoo_contains_hash(CuckooFilterObject *self, PyObject *args)
// {
//     uint32_t fingerprint;
//     uint32_t h1;

//     if (!PyArg_ParseTuple(args, "kk", &fingerprint, &h1))
//         return NULL;

//     CUCKOO_FILTER_RETURN result = cuckoo_filter_contains_hash(self->filter, fingerprint, h1);
//     return PyLong_FromLong(result);
// }

// static PyObject *
// refcuckoo_hash(CuckooFilterObject *self, PyObject *args)
// {
//     uint32_t fingerprint;
//     uint32_t h1;
//     uint32_t h2;
//     void *key;
//     Py_ssize_t key_length;

//     if (!PyArg_ParseTuple(args, "y#", &key, &key_length))
//         return NULL;

//     cuckoo_filter_hash(self->filter, key, (uint32_t)key_length, &fingerprint, &h1, &h2);

//     PyObject *fingerprint_object = PyLong_FromUnsignedLong(fingerprint);
//     if (fingerprint_object == NULL) {
//         PyErr_SetObject(PyExc_Exception,
//             PyUnicode_FromString("Error allocating 'fingerprint' object."));
//         return NULL;
//     }

//     PyObject *h1_object = PyLong_FromUnsignedLong(h1);
//     if (h1_object == NULL) {
//         Py_DECREF(fingerprint_object);
//         PyErr_SetObject(PyExc_Exception,
//             PyUnicode_FromString("Error allocating 'h1' object."));
//         return NULL;
//     }

//     PyObject *h2_object = PyLong_FromUnsignedLong(h2);
//     if (h2_object == NULL) {
//         Py_DECREF(fingerprint_object);
//         Py_DECREF(h1_object);
//         PyErr_SetObject(PyExc_Exception,
//             PyUnicode_FromString("Error allocating 'h1' object."));
//         return NULL;
//     }

//     return PyTuple_Pack(3, fingerprint_object, h1_object, h2_object);
// }

static PyObject *
refcuckoo_remove(CuckooFilterObject *self, PyObject *args)
{
    void *key;
    Py_ssize_t key_length;

    if (!PyArg_ParseTuple(args, "y#", &key, &key_length))
        return NULL;

    Status status = self->filter->Delete(key, key_length);
    return PyLong_FromLong(status);
}

static PyMethodDef refcuckoo_methods[] = {
    {
        "add",
        (PyCFunction) refcuckoo_add,
        METH_VARARGS,
        "Add an item to the cuckoo filter."
    },
    {
        "contains",
        (PyCFunction) refcuckoo_contains,
        METH_VARARGS,
        "Check if an item is possibly in the cuckoo filter. This can return false "
            "positives."
    },
    // {
    //     .ml_name  = "contains_hash",
    //     .ml_meth  = (PyCFunction) refcuckoo_contains_hash,
    //     .ml_flags = METH_VARARGS,
    //     .ml_doc   = "Check if an item is possibly in the cuckoo filter. This can return false "
    //         "positives."
    // },
    // {
    //     .ml_name  = "hash",
    //     .ml_meth  = (PyCFunction) refcuckoo_hash,
    //     .ml_flags = METH_VARARGS,
    //     .ml_doc   = "Get the hash values for the given item."
    // },
    {
        "remove",
        (PyCFunction) refcuckoo_remove,
        METH_VARARGS,
        "Remove an item from the cuckoo filter."
    },
    { NULL }  /* Sentinel */
};

static PyTypeObject CuckooFilterType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "refcuckoo.CuckooFilter",               /* tp_name */
    sizeof(CuckooFilterObject),               /* tp_basicsize */
    0,                              /* tp_itemsize */
    (destructor) refcuckoo_dealloc,      /* tp_dealloc */
    0,                              /* tp_vectorcall_offset */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
    0,                              /* tp_as_async */
    0,           /* tp_repr */
    0,                              /* tp_as_number */
    0,                              /* tp_as_sequence */
    0,                              /* tp_as_mapping */
    0,                              /* tp_hash */
    0,                              /* tp_call */
    0,                              /* tp_str */
    0,                              /* tp_getattro */
    0,                              /* tp_setattro */
    0,                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                              /* tp_flags */
    "A cuckoo filter instance",                   /* tp_doc */
    0,                              /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    0,                              /* tp_iter */
    0,                              /* tp_iternext */
    refcuckoo_methods,                              /* tp_methods */
    0,                              /* tp_members */
    refcuckoo_getsets,                              /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    (initproc) refcuckoo_init,                              /* tp_init */
    0,                              /* tp_alloc */
    refcuckoo_new,                      /* tp_new */
};


// static PyObject* create_cuckoo_filter(PyObject* self, PyObject* args)
// {
//     const char* file_name;
//     const unsigned char* hash_data;
//     Py_ssize_t hash_size;
//     unsigned char jpg_quality;

//     if (!PyArg_ParseTuple(args, "sy#b", &file_name, &hash_data, &hash_size, &jpg_quality))
//         return NULL;

//     return PyLong_FromLong(1);
// }

// static PyMethodDef refcuckoo_methods[] = {
//     {"create",  create_cuckoo_filter, METH_VARARGS, "Create a new cuckoo filter"},
//     {NULL, NULL, 0, NULL}        /* Sentinel */
// };

static struct PyModuleDef refcuckoo_module = {
    PyModuleDef_HEAD_INIT,
    "refcuckoo",
    NULL,
    -1,
};

PyMODINIT_FUNC
PyInit_refcuckoo(void)
{
    PyObject *module;
    if (PyType_Ready(&CuckooFilterType) < 0)
        return NULL;

    module = PyModule_Create(&refcuckoo_module);
    if (module == NULL)
        return NULL;

    Py_INCREF(&CuckooFilterType);
    if (PyModule_AddObject(module, "CuckooFilter", (PyObject *) &CuckooFilterType) < 0) {
        Py_DECREF(&CuckooFilterType);
        Py_DECREF(module);
        return NULL;
    }

    return module;
}

