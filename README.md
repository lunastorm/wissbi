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
```bash
wissbi-sub test
```
This will launch a subscriber listening messages from a source named "test", and write the messages to stdout.

Now you can start a new terminal session and execute:
```bash
echo "hello world" | wissbi-pub test
```

You can see the message "hello world" appeared in the subscriber's terminal.

Press Ctrl+c to exit wissbi-sub

See https://github.com/lunastorm/wissbi/wiki/Usage for detailed usage guide.

# Administration
See https://github.com/lunastorm/wissbi/wiki/Administration for detailed administration guide.
