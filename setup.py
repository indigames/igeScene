from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext as build_ext_orig
import pathlib
import setuptools
import numpy
import sys
import os
import shutil

from os import path
here = path.abspath(path.dirname(__file__))

class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])

class build_ext(build_ext_orig):
    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)
        super().run()

    def build_cmake(self, ext):
        cwd = pathlib.Path().absolute()

        # these dirs will be created in build_py, so if you don't have
        # any python sources to bundle, the dirs will be missing
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        build_temp =  os.path.abspath(os.path.dirname(self.build_temp))

        # example of cmake args
        config = 'Debug' if self.debug else 'Release'
        cmake_args = [
            '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + str(extdir),
            '-DCMAKE_BUILD_TYPE=' + config,
            '-DAPP_STYLE=SHARED',
            '-DPYTHON_VERSION=' + str(sys.version_info[0]) + '.' + str(sys.version_info[1])
        ]

        # example of build args
        build_args = [
            '--config', config
        ]

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        os.chdir(str(build_temp))

        cmake_arch = None
        if sys.platform == 'win32':
            if sys.maxsize > 2 ** 32:
                cmake_arch = 'x64'
            else:
                cmake_arch = 'Win32'
            self.spawn(['cmake', '-A', cmake_arch, str(cwd)] + cmake_args)
        elif sys.platform == "darwin":
            self.spawn(['cmake', '-G', 'Xcode', '-DOSX=1', '-DCMAKE_OSX_ARCHITECTURE=x86_64', str(cwd)] + cmake_args)

        ext_name = str(ext.name).split('.')[-1]
        if not self.dry_run:
            self.spawn(['cmake', '--build', '.', '--target', ext_name] + build_args)
        pyd_path = os.path.join(build_temp, 'bin' if sys.platform == "win32" else 'lib', config, f'{ext_name}.pyd')
        extension_path = os.path.join(cwd, self.get_ext_fullpath(ext.name))
        extension_dir = os.path.dirname(extension_path)
        if not os.path.exists(extension_dir):
            os.makedirs(extension_dir)
        shutil.move(pyd_path, extension_path)

        # Copy additional dlls
        for root, dirs, files in os.walk(os.path.join(build_temp, 'lib', config)):
            for file in files:
                if file.endswith(".dll") or file.endswith(".dylib"):
                    if not os.path.exists(os.path.join(extension_dir, os.path.basename(file))):
                        shutil.copy(os.path.join(root, file), extension_dir)

        # Troubleshooting: if fail on line above then delete all possible
        # temporary CMake files including "CMakeCache.txt" in top level dir.
        os.chdir(str(cwd))


setup(name='igeScene', version='0.2.1',
      description='C++ Scene management extension for 3D and 2D games.',
      author=u'Indigames',
      author_email='dev@indigames.net',
      packages=find_packages(),
      ext_modules=[CMakeExtension('igeScene')],
      cmdclass={
          'build_ext': build_ext,
      },
      long_description=open(path.join(here, 'README.md')).read(),
      long_description_content_type='text/markdown',
      url='https://indigames.net/',
      license='MIT',
      classifiers=[
          'Intended Audience :: Developers',
          'License :: OSI Approved :: MIT License',
          'Programming Language :: Python :: 3',
          #'Operating System :: MacOS :: MacOS X',
          #'Operating System :: POSIX :: Linux',
          'Operating System :: Microsoft :: Windows',
          'Topic :: Games/Entertainment',
      ],
      keywords='3D game Indigames',
      package_data={'igeScene': ['*.dll', '*.dylib']},
      include_package_data=True,
      setup_requires=['wheel']
      )
