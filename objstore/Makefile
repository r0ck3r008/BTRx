all: objstore.o cache.o

objstore.o: objstore.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c objstore.cpp

cache.o: cache.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c cache.cpp

clean:
	rm -rf *.o
