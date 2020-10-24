all: pktmsg

pktmsg: pktmsg.proto
	protoc --experimental_allow_proto3_optional --cpp_out=. pktmsg.proto
	mv pktmsg.pb.h ${COMPILE_PATH}/node

clean:
	rm -rf *.o

clean_proto:
	rm -rf pktmsg.pb.cc
	rm -rf ${COMPILE_PATH}/node/pktmsg.pb.h

