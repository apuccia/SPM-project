#!/bin/bash

for i in {3..11..2}
do
    echo "Testing sequential solution with kernel size $i, resolution 960x540"
    ./bin/sequential.out -k $i -t 50 -f "./data/SD.mp4"
done

for i in {3..11..2}
do
    echo "Testing sequential solution with kernel size $i, resolution 1280x720"
    ./bin/sequential.out -k $i -t 50 -f "./data/HD.mp4"
done

for i in {3..11..2}
do
    echo "Testing sequential solution with kernel size $i, resolution 1920x1080"
    ./bin/sequential.out -k $i -t 50 -f "./data/FHD.mp4"
done