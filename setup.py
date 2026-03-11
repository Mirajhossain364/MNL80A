from setuptools import setup, find_packages

setup(
    name="mnl80a",
    version="0.1.0",
    description="Python Wrapper for Jiaxun MNL80A PLC Module AT Commands",
    author="Jiaxun Open Source",
    packages=find_packages(),
    install_requires=[
        "pyserial>=3.5",
    ],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.7",
)
