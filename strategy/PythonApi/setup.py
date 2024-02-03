from setuptools import setup

setup(
    name='pystrategy',
    version="1.0.1",
    description="The python api for ARTrade's Strategy",
    url='https://github.com/hulingyue/ARTrade',
    license='MIT',
    platforms='Linux',
    packages=['pystrategy'],
    package_data={'pystrategy': ['pystrategy.cpython-310-x86_64-linux-gnu.so']},
    install_requires=[
        'numpy',
    ],
)
