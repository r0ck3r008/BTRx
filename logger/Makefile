all: logger.o logchild.o logmsg

logger.o: logger.cpp logmsg
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c logger.cpp

logchild.o: logchild.cpp logmsg
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c logchild.cpp

logmsg: logmsg.proto
	protoc --cpp_out=. logmsg.proto
	mv logmsg.pb.h ${COMPILE_PATH}/logger

clean:
	rm -rf *.o
	rm -rf logmsg.pb.cc
	rm -rf ${COMPILE_PATH}/logger/logmsg.pb.h
