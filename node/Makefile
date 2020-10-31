all: entrypoint.o node.o objstore.o cache.o

entrypoint.o: entrypoint.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c entrypoint.cpp

node.o: node.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c node.cpp

objstore.o: objstore.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c objstore.cpp

cache.o: cache.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c cache.cpp

clean:
	rm -rf *.o
