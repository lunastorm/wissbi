# Tested Build Environment
- Ubuntu 12.04 with gcc 4.6.3 and cmake 2.8.7

# Build Steps
## Binaries
```bash
make
```
The output binaries will be put under tmp/build

## Testing
```bash
make test
```
The JUnit testing result compatible xml files will be put under output/test

## Debian Package
```bash
make deb
```
The debian package will be put under output/artifacts

## Installation
- You can run dpkg -i [debian package name] to install it to system or
- simply run make install


# Usage

# Administration
