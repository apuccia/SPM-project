#!/bin/bash

res=$1

if [[ -n $res ]]; then
    if [[ $res == "SD" ]]; then
        data="./data/SD.mp4"
    elif [[ $res = "HD" ]]; then
        data="./data/HD.mp4"
    elif [[ $res = "FHD" ]]; then
        data="./data/FHD.mp4"
    else
        echo "Not a valid resolution"
        exit
    fi
fi

mkdir -p results;
for ker in {3..11..2}
do
    echo "Testing sequential solution with kernel size $ker, resolution $res"
    ./bin/sequential.out -k $ker -t 50 -f $data > ./results/sequential_$res\_$ker.txt
done