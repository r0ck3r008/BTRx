all: node.o

node.o: node.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c node.cpp

clean:
	rm -rf *.o
