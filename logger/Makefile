all: logger.o logchild.o

logger.o: logger.cpp
	${COMPILER} ${COMPILE_PATH} ${COMPILER_FLAGS} -c logger.cpp

logchild.o: logchild.cpp
	${COMPILER} ${COMPILE_PATH} ${COMPILER_FLAGS} -c logchild.cpp

clean:
	rm -rf *.o
