#!/bin/sh

make -B icpc pgo=1
make run option=0
make -B icpc pgouse=1
