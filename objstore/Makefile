all: objstore.o cache.o bfield.o

objstore.o: objstore.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c objstore.cpp

cache.o: cache.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c cache.cpp

bfield.o: bfield.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c bfield.cpp

clean:
	rm -rf *.o
