#!/bin/bash

peersid=($(awk '{print $1}' PeerInfo.cfg))
peers=($(awk '{print $2}' PeerInfo.cfg))

i=1
while [ $i -le 9 ];
do
        ssh -t narora@"${peers["$i"]}" echo "${peersid["$i"]}"
        i=$(( $i + 1 ))
done
