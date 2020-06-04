from setuptools import setup
from codecs import open
import os

here = os.path.abspath(os.path.dirname(__file__))

with open(os.path.join(here, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()


setup(
	name = 'RTRTC',
	version = '0.1.6',
	description = 'Toy ray-tracer based on ThrustRTC on CURandRTC',
	long_description=long_description,
	long_description_content_type='text/markdown',  
	url='https://github.com/fynv/RTRTC',
	license='Anti 996',
	author='Fei Yang',
	author_email='hyangfeih@gmail.com',
	keywords='GPU CUDA RayTracing',
	packages=['RTRTC'],
	package_data = { 'RTRTC': ['*.dll', '*.so']},
	install_requires = ['CURandRTC', 'ThrustRTC', 'numpy', 'pillow']
)

