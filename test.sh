#!/bin/bash

res=$1
sol=$2

if [[ -n $res ]]; then
    if [[ $res == "SD" ]]; then
        data="./data/SD.mp4"
    elif [[ $res = "HD" ]]; then
        data="./data/HD.mp4"
    elif [[ $res = "FHD" ]]; then
        data="./data/FHD.mp4"
    else
        echo "Not a valid resolution"
        echo "Pass as first argument a resolution between \"SD\" (960x540), \"HD\" (1280x720) or \"FHD\" (1920x1080)"
        exit
    fi
else
    echo "Pass as first argument a resolution between \"SD\" (960x540), \"HD\" (1280x720) or \"FHD\" (1920x1080)"
    exit
fi

if [[ $sol < 0 || $sol > 2 ]]; then
    echo "Pass as second argument a solution between \"0\" (sequential), \"1\" (farm) or \"2\" (pipe)"
    exit
fi

# Sequential
if [[ $sol == 0 ]]; then
    mkdir -p results/sequential;
    echo "Testing sequential solution with resolution $res, kernel size 11"
    ./bin/sequential.out -k 11 -t 50 -f $data > ./results/sequential/sequential_$res.txt
fi

# Farm
if [[ $sol == 1 ]]; then
    mkdir -p results/farm;
    ./bin/farm.out -k 11 -t 50 -f $data -n 1 > ./results/farm/farm_$res.txt
    for (( nw = 2; nw <= 64; nw = nw * 2 )); do
        echo "Testing farm solution with resolution $res, kernel size 11, $nw workers"
        ./bin/farm.out -k 11 -t 50 -f $data -n $nw >> ./results/farm/farm_$res.txt
    done
fi

# Pipe
if [[ $sol == 2 ]]; then
    mkdir -p results/pipe;
    ./bin/pipe.out -k 11 -t 50 -f $data -n 1 > ./results/pipe/$res/pipe.txt
    for (( nw = 2; nw <= 64; nw = nw * 2 )); do
        echo "Testing pipe solution with resolution $res, kernel size 11, $nw workers"
        ./bin/pipe.out -k 11 -t 50 -f $data -n $nw >> ./results/pipe/pipe_$res.txt
    done
fi