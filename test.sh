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
    mkdir -p results/sequential/$res;
    for ker in {3..11..2}; do
        echo "Testing sequential solution with resolution $res, kernel size $ker"
        ./bin/sequential.out -k $ker -t 50 -f $data > ./results/sequential/$res/sequential\_$res\_$ker.txt
    done;
fi

# Farm
if [[ $sol == 1 ]]; then
    for ker in {3..11..2}; do
        for nw in {2..32..2}; do
            mkdir -p results/farm/$res/$ker;
            echo "Testing farm solution with resolution $res, kernel size $ker, $nw workers"
            ./bin/farm.out -k $ker -t 50 -f $data -n $nw > ./results/farm/$res/$ker/farm_$nw.txt
        done
    done
fi

# Pipe
if [[ $sol == 2 ]]; then
    mkdir -p results/pipe/$res/$ker;
    for ker in {3..11..2}; do
        for nw in {2..32..2}; do
            echo "Testing pipe solution with kernel size $ker, resolution $res, $nw workers"
            ./bin/pipe.out -k $ker -t 50 -f $data -n $nw > ./results/pipe/$res/$ker/pipe_$nw.txt
        done
    done
fi