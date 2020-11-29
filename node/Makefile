all: node.o pkts.o

node.o: node.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c node.cpp

pkts.o: pkts.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c pkts.cpp

clean:
	rm -rf *.o
