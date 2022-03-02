from distutils.core import setup, Extension
import sys

module1 = Extension('refcuckoo',
                    include_dirs=['src'],
                    sources=["src/hashutil.cc", "src/cuckoo_python.cc"],
                    # https://cibuildwheel.readthedocs.io/en/stable/faq/#windows-importerror-dll-load-failed-the-specific-module-could-not-be-found
                    extra_compile_args=['/d2FH4-'] if sys.platform == 'win32' else [])

setup(name='refcuckoo',
      version='1.0',
      description='A cuckoo filter implementation.',
      author='Roger Taylor',
      author_email='roger.taylor.email@gmail.com',
      url='https://github.com/electrumsv/cuckoofilter',
      long_description=open('README.md', 'r').read(),
      long_description_content_type='text/markdown',
      license='Apache License v2.0',
      # This warns about no `__init__.py` file but seems to install workable types.
      packages=['refcuckoo-stubs'],
      package_data={"refcuckoo-stubs": ['__init__.pyi']},
      # The actual package.
      ext_modules=[ module1 ])
