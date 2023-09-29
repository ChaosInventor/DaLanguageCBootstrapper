# DaLanguage C bootstrapper

A bootrapper for DaLanguage written in C.

**Note: DaLanguage is currently experimental, there is no documentation. This
project is still highly unstable and untested.**

## Building


In order to build the current test interpreter that can do basic math, simply
run `make` in the project directory

```shell
make -j
```
### Prerequisites

A working C99 compiler with the standard library along with basic unix utilities:

- rm;
- mkdir;
- printf;
- awk;
- sed;
- tr.

And of course make. In case you don't have make, you could run the compiler
yourself with all source files in `src/` and `Interpreters/math.c` and
`include/DaLanguageCBootstrap` in the header search path.

*No external dependencies are required.*

## Running

In order to run the math test interpreter, navigate to `build/bin/Interpreters/`
after having ran `make`. Then run `./math <PROGRAM>`.

**Assuming** the current working directory is the project repo:

```shell
make
cd build/bin/Interpreters/
echo 20 | ./math ../../../Examples/Factorial.dali
```

## Examples

Check the `Examples/` directory.

## Documentation

WIP, TBA.

