from setuptools import setup, Extension

setup(
    name='pystrategy',
    version='1.0',
    description="The python api for ARTrade's Strategy",
    url='https://github.com/hulingyue/ARTrade',
    license='MIT',
    platforms='Linux',
    packages=['pystrategy'],
    package_data={'pystrategy': ['*.so']},
    ext_modules=[Extension('pystrategy.pystrategy', ['pystrategy/*.so'])]
)
