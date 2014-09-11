#!/bin/sh

g++ -g -c lib/RobotLib.c -o lib/RobotLib.o

for src in `find src/*.c`
do
	echo "Building $src"
	g++ -g -c $src -o $src.o
	g++ -g $src lib/RobotLib.o -o bin/`basename $src .c`
done

