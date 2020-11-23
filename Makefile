COMPILER := g++
COMPILER_FLAGS := -g
COMPILE_PATH := ${shell pwd}/include

ALL_OBJS := logger/*.o node/*.o objstore/*.o entrypoint.o

all: btrx.bin

btrx.bin: entrypoint.o logger_objs node_objs objstore_objs
	${COMPILER} ${ALL_OBJS} -o peer
	$(MAKE) clean_objs

entrypoint.o: entrypoint.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c entrypoint.cpp

logger_objs:
	COMPILER=${COMPILER} COMPILE_PATH=${COMPILE_PATH} \
		   COMPILER_FLAGS=${COMPILER_FLAGS} $(MAKE) -C logger/

node_objs:
	COMPILER=${COMPILER} COMPILE_PATH=${COMPILE_PATH} \
		   COMPILER_FLAGS=${COMPILER_FLAGS} $(MAKE) -C node/

objstore_objs:
	COMPILER=${COMPILER} COMPILE_PATH=${COMPILE_PATH} \
		   COMPILER_FLAGS=${COMPILER_FLAGS} $(MAKE) -C objstore/

clean_objs:
	$(MAKE) -C logger/ clean
	$(MAKE) -C node/ clean
	$(MAKE) -C objstore/ clean
	rm -rf entrypoint.o

clean: clean_objs
	rm -rf ./peer
