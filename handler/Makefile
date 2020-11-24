all: handler.o

handler.o: handler.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c handler.cpp

clean:
	rm -rf *.o
