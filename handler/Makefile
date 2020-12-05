all: handler.o outhelper.o inhelper.o

handler.o: handler.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c handler.cpp

outhelper.o: outhelper.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c outhelper.cpp

inhelper.o: inhelper.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c inhelper.cpp

clean:
	rm -rf *.o
