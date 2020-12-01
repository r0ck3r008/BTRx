COMPILER := g++
COMPILER_FLAGS := -g
LINKER_FLAGS := -pthread
COMPILE_PATH := ${shell pwd}/include

ALL_OBJS := entrypoint.o logger/*.o node/*.o objstore/*.o handler/*.o pkts/*.o nbrmap/*.o

all: btrx.bin

btrx.bin: entrypoint.o logger_objs node_objs objstore_objs handler_objs pkts_objs nbrmap_objs
	${COMPILER} ${LINKER_FLAGS} ${ALL_OBJS} -o peer
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

handler_objs:
	COMPILER=${COMPILER} COMPILE_PATH=${COMPILE_PATH} \
		   COMPILER_FLAGS=${COMPILER_FLAGS} $(MAKE) -C handler/

pkts_objs:
	COMPILER=${COMPILER} COMPILE_PATH=${COMPILE_PATH} \
		   COMPILER_FLAGS=${COMPILER_FLAGS} $(MAKE) -C pkts/

nbrmap_objs:
	COMPILER=${COMPILER} COMPILE_PATH=${COMPILE_PATH} \
		   COMPILER_FLAGS=${COMPILER_FLAGS} $(MAKE) -C nbrmap/

clean_objs:
	$(MAKE) -C logger/ clean
	$(MAKE) -C node/ clean
	$(MAKE) -C objstore/ clean
	$(MAKE) -C handler/ clean
	$(MAKE) -C pkts/ clean
	$(MAKE) -C nbrmap/ clean
	rm -rf entrypoint.o

clean: clean_objs
	rm -rf ./peer
