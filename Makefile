COMPILER := g++
COMPILER_FLAGS := '-g -Wall'
COMPILE_PATH := -I${shell pwd}/include

ALL_OBJS := logger/*.o

all: btrx.bin

btrx.bin: logger_objs
	make clean_objs

logger_objs:
	COMPILER=${COMPILER} COMPILE_PATH=${COMPILE_PATH} \
		   COMPILER_FLAGS=${COMPILER_FLAGS} make -C logger/

clean_objs:
	make -C logger/ clean

clean: clean_objs
	rm -rf bin/*.out
