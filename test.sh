#!/bin/bash

res=$1

if [[ -n $res ]]; then
    echo $res
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

for i in {3..11..2}
do
    echo "Testing sequential solution with kernel size $i, resolution $res"
    ./bin/sequential.out -k $i -t 50 -s -f $data
done