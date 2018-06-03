#!/bin/bash
PREFIX=Coordinator_Dynamic_Speed_
echo $PREFIX

for i in {1..12}
do
    echo "Running configuration $PREFIX$i"
    ../tutorial -r 0 -m -u Cmdenv -c $PREFIX$i -n ..:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../../Downloads/veins-veins-4.6/examples/veins:../../../Downloads/veins-veins-4.6/src/veins --image-path=../../inet/images:../../../Downloads/veins-veins-4.6/images -l ../../inet/src/INET -l ../../../Downloads/veins-veins-4.6/src/veins --cpu-time-limit=300s omnetpp.ini   
done
