#!/usr/bin/env bash

x=35
y=20
c=1
b=10
d=0.01
it=200
f=0.15
samples=200

stdbuf -o0 ./basico -f "$f" -c "$c" -b "$b" -x "$x" -y "$y" -d "$d" -i "$it" --show-avg-fitness-coop --show-avg-fitness-noncoop | ./driveGnuPlotStreams 2 1 "$samples" "$((-4*c))" "$((4*b))" 640x480+0+0 "Fitness promedio de cooperadores" "Fitness promedio de no cooperadores" 0 0
