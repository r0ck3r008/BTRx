<h1 align="center"> Project Report </h1> <br>

## Table of Contents

- [Table of Contents](#table-of-contents)
- [Build Process](#build-process)
- [What is working ?](#what-is-working-)
- [entrypoint.cpp](#entrypointcpp)
- [handler.cpp](#handlercpp)
- [pks.cpp](#pkscpp)
- [node.cpp](#nodecpp)
- [objstore.cpp](#objstorecpp)
  - [bfield.cpp](#bfieldcpp)
  - [cache.cpp](#cachecpp)
- [nbrmap.cpp](#nbrmapcpp)
- [Logger.cpp](#loggercpp)

## Build Process

- unzip the compressed file using `unzip folder.zip`
- `make` to compile project
- `./peer 1001` to run the compiled binary with Process ID 1001.
- `make clean` to clean the compiled binaries

## What is working ?

- Able to read Common.cfg and PeerInfo.cfg
- Able to create logger file per node
- Able to create LRU Cache that holds the file chunks
- Able to create peer in a mesh pattern
- Able to exchange handshake message
- Able to exchange bitfield message
- Able to exchange interested/not interested packets
- Each node has preview of bitfield of all the nodes it is peers
- In synchronizaiton with all the peers its connected to so that no piece is request twice to save bandwidth
- It is able to synchronize all the request for pieces in between peers.
- Graceful exit is possible
  
## entrypoint.cpp

`entrypoint.cpp` is responsible for parsing two major files i.e. *Common.cfg* and *PeerInfo.cfg*. It has two methods `read_conf_file` and `read_peer_info` file responsible for paring Common.cfg and PeerInfor.cfg file respectively. Once both the files is parsed, an Instance of Node object (which has file) is created responsible for accepting connection from other nodes.

## handler.cpp

Handler is response for all the message it transfers between two nodes. It contains a switch case and based the type of message/packet, it forwards it to the peer.

## pks.cpp

Contains json definition for all the types of message.

## node.cpp

It is responsible for sockets for incoming and outgoing messages. For efficiency, we are serializing messages in BSON format and then transmiting them over the socket. Every socket connection between client and server is handled by a thread. Every node object has its own Objstore and NBRMap (Neighbor Map).

## objstore.cpp

Objstore consists of two module: Bitfield and Cache. Objstore collectively reprsents the file that is being transmitted.

### bfield.cpp

Every file that is to be transmitted is divided into chunks. These chunks are mapped on to a Bitfield that denotes if the current node has that file chunk or not. These bitfield are in little endian order. If the file chunk is absent, then the current node would send interested message to its neighbor map that has the file chunk. This is done by comparing the bitfield of both peers and computed a difference of. These bitfield differenc denotes which chunks are absent and that needs to be requested from it's peer.

### cache.cpp

Cache is LRU implementation that keeps information of the file that is being transmitted. Once the program exits, the file present in the cache is written out in main memory. We have in memory cache rather than file system for storing these files once they are transmitted.

## nbrmap.cpp

nbrmap holds the list of neighbor a node is connected to. Apart from node reference, it also holds various other properties such as if the node is choked or unchoked via a shared atomic variable.

## Logger.cpp

A Logger utility is added to program that prints the log message. `msg_write` method is overloaded depending upon the message type that needs to be printed.