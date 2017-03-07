#!encoding=utf8

__author__ = 'mutalisk'

from setuptools import setup
from distutils.extension import Extension

ext_modules = [Extension("pycron", ["pycron/pycron.c", "pycron/cron/cronexpr.c", "pycron/cron/rbtree.c"],
    language="c",
    include_dirs=["pycron/cron", "pywrapper-dependence/include"],
    library_dirs=["pywrapper-dependence/lib64-win"],
    libraries=["python27"]
    )]

setup(name='pycron',
    version='0.1.0',
    description='Python Wrapper of Crontab-like C library',
    author='',
    author_email='',
    url='https://github.com/mutalisk999/pycron',
    platforms='win',
    packages=['pycron'],
    install_requires=["setuptools"],
    zip_safe=False,
    ext_modules = ext_modules,
    )