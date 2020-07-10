import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="hyperbuild",
    version="0.1.12",
    author="Wilson Lin",
    author_email="code@wilsonl.in",
    description="Fast one-pass in-place HTML minifier written in Rust with context-aware whitespace handling",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/wilsonzlin/hyperbuild",
    packages=["hyperbuild"],
    include_package_data=True,
    classifiers=[
        "Programming Language :: Python :: 3 :: Only",
        "Programming Language :: Python :: Implementation :: CPython",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.5',
)
