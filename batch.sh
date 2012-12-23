#!/bin/bash

# for citrus
#for i in {0..62..1}; do
#    ./imagetool -o up -s mid -d dn -f 0 -t 255 -n ../Data/try12/$i
#done

#for i in {0..299..1}; do
#    ./imagetool -o mid -s up -f 270 -t 350 -n ../Data/try11/$i
#done

for i in {0..299..1}; do
    ./imagetool ../Data/try15/$i
done






