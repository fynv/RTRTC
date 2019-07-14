#!/usr/bin/python3

from setuptools import setup, Extension
from codecs import open
import os
import platform

here = os.path.abspath(os.path.dirname(__file__))

with open(os.path.join(here, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

extra_compile_args=[]
extra_link_args = []
define_macros = []
name_thrustrtc = ''
name_curandrtc = ''

if os.name == 'nt':
	define_macros += [
		('RT_RTC_DLL_EXPORT', None), 
		('CURAND_RTC_DLL_IMPORT', None), 
		('THRUST_RTC_DLL_IMPORT', None),
		('_CRT_SECURE_NO_DEPRECATE', None),
		('_SCL_SECURE_NO_DEPRECATE', None),
		('_CRT_SECURE_NO_WARNINGS', None)
	]
	name_thrustrtc = 'PyThrustRTC.cp37-win_amd64'
	name_curandrtc = 'PyCURandRTC.cp37-win_amd64'
else:
	extra_compile_args = ['-std=c++11']
	extra_link_args = ['-Wl,-rpath=$ORIGIN']
	name_thrustrtc = ':PyThrustRTC.cpython-37m-x86_64-linux-gnu.so'
	name_curandrtc = ':PyCURandRTC.cpython-37m-x86_64-linux-gnu.so'
	

sources = [ 
'PyRTRTC.cpp',
'../internal/cuda_wrapper.cpp',
'../internal/initialize.cpp',
'../DVImage.cpp',
'../DVSphere.cpp',
'../RayTracer.cpp',
]


module_RTRTC = Extension(
	'PyRTRTC',
	sources = sources,
	include_dirs = ['.', '..', '../internal', '../install/include'],
	library_dirs = ['.'],
	libraries = [name_thrustrtc, name_curandrtc],
	define_macros = define_macros,
	extra_compile_args = extra_compile_args,
	extra_link_args = extra_link_args
	)

setup(
	name = 'RTRTC',
	version = '0.0.1',
	description = 'Toy ray-tracer based on ThrustRTC on CURandRTC',
	long_description=long_description,
	long_description_content_type='text/markdown',  
	url='https://github.com/fynv/RTRTC',
	license='Anti 996',
	author='Fei Yang',
	author_email='hyangfeih@gmail.com',
	keywords='GPU CUDA RayTracing',
	py_modules=['RTRTC'],
	ext_modules=[module_RTRTC],
	install_requires = ['CURandRTC', 'ThrustRTC', 'numpy', 'pillow']
)

