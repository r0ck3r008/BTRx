#!/bin/bash

lines=$(wc -l PeerInfo.cfg | awk '{print $1}')
peersid=($(awk '{print $1}' PeerInfo.cfg))
peers=($(awk '{print $2}' PeerInfo.cfg))

i=0
while [ $i -lt "$lines" ];
do
        tmux new-session -d -s "${peersid["$i"]}" ssh -t narora@"${peers["$i"]}" ./peer "${peersid["$i"]}"
        i=$(( $i + 1 ))
done
