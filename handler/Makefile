all: handler.o sendloop.o recvloop.o

handler.o: handler.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c handler.cpp

sendloop.o: sendloop.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c sendloop.cpp

recvloop.o: recvloop.cpp
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c recvloop.cpp

clean:
	rm -rf *.o
