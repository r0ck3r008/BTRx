COMPILER := g++
COMPILER_FLAGS := '-g -Wall'
COMPILE_PATH := ${shell pwd}/include

ALL_OBJS := logger/*.o node/*.

all: btrx.bin

btrx.bin: logger_objs node_objs
	$(MAKE) clean_objs

logger_objs:
	COMPILER=${COMPILER} COMPILE_PATH=${COMPILE_PATH} \
		   COMPILER_FLAGS=${COMPILER_FLAGS} $(MAKE) -C logger/

node_objs:
	COMPILER=${COMPILER} COMPILE_PATH=${COMPILE_PATH} \
		   COMPILER_FLAGS=${COMPILER_FLAGS} $(MAKE) -C node/

clean_objs:
	$(MAKE) -C logger/ clean
	$(MAKE) -C node/ clean

clean: clean_objs
	rm -rf bin/*.out
