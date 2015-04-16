#!/bin/sh

dir=$(dirname $0)
cd "${dir}"

gcc $(find . -name '*.c' | grep -v test) -o filter \
	-W -Wall -Wextra -Werror -Wno-sign-compare -Wno-unused-result \
	-Wno-strict-aliasing \
	-D_POSIX_SOURCE \
	-pthread -g -std=c99 -O2
