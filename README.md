# jgade-code-googletest-version

This repository uses [jgade-code](https://pragprog.com/titles/jgade/source_code).

Thanks for James Grenning who is author of `Test Driven Development for Embedded C`

## Platforms

This sample has been used on only Cygwin.

### Requirements

* Cygwin v2.4.1 or newer, and below packages
    * gcc-g++ v5.3.0-3 or newer
    * make v4.1-1 or newer
    * perl 5.22.1-1 or newer
    * ccache 3.1.9-2 or newer (optional)
    * clang 3.7.1 or newer (optional)

## Usage

### Test

Below is the flow of testing LedDriver on cygwin shell.

```bash
cd <root directory of this repository>/test/LedDriverTest
make
```

And you can ccache for reduce the time of build.

```bash
make UT_USE_CCACHE=y
```

### Coverage

Below is the flow of getting the coverage of LedDriver on Cygwin shell.

```bash
cd <root directory of this repository>/test/LedDriver
make clean
make coverage
# Open ./coverage-report/index.html
```

### Static Analysis

Below is the flow of getting the Static Analysis of LedDriver on cygwin shell.

```bash
cd <root directory of this repository>/test/LedDriverTest
make clean
scan-build make UT_ENABLE_DEBUG=y
# Open the static analysis report with browser you use .
# You can get path of the static analysis report from scan-build message.
```
