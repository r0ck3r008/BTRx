all: entrypoint.o node.o objstore.o cache.o pktmsg

entrypoint.o: entrypoint.cpp pktmsg
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c entrypoint.cpp

node.o: node.cpp pktmsg
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c node.cpp

objstore.o: objstore.cpp pktmsg
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c objstore.cpp

cache.o: cache.cpp pktmsg
	${COMPILER} -I${COMPILE_PATH} ${COMPILER_FLAGS} -c cache.cpp

pktmsg: pktmsg.proto
	protoc --experimental_allow_proto3_optional --cpp_out=. pktmsg.proto
	mv pktmsg.pb.h ${COMPILE_PATH}/node

clean:
	rm -rf *.o

clean_proto:
	rm -rf pktmsg.pb.cc
	rm -rf ${COMPILE_PATH}/node/pktmsg.pb.h

