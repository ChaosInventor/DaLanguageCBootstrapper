#!/bin/sh

BUILD_DIR=${BUILD_DIR:-build}
COMPILER=$BUILD_DIR/bin/Interpreters/compiler

#Check if compiler interpreter is made, if not make it, if still not executable
#fail.
if [ ! -x $COMPILER ]; then
    echo 'Compiler interpreter not found, running `make`' >&2
    make $COMPILER
    if [ ! -x $COMPILER ]; then
        echo 'Compiler interpreter is still not executable, error' >&2
        exit -2
    fi
fi

set -e

ASM_DIR=$BUILD_DIR/asm
OBJ_DIR=$BUILD_DIR/obj/asm
BIN_DIR=$BUILD_DIR/bin/asm

mkdir -p $ASM_DIR
mkdir -p $OBJ_DIR
mkdir -p $BIN_DIR

#Run compiler for each position argument
for file; do
    f=${file##*/}
    echo "Creating assembly from file $file" >&2
    $COMPILER $file > $ASM_DIR/$f.asm
    echo "Assembling $ASM_DIR/$f.asm" >&2
    nasm -f elf64 $ASM_DIR/$f.asm -o $OBJ_DIR/$f.o
    echo "Linking $OBJ_DIR/$f.o" >&2
    ld $OBJ_DIR/$f.o -o $BIN_DIR/$f.elf
    echo "Created $BIN_DIR/$f.elf" >&2
done

