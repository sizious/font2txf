#!/bin/sh

./ttf2txf -o sans10.txf -s 10 -w 256 -h 256 -f cel-subset.txt arial.ttf
./ttf2txf -o sans12.txf -s 12 -w 256 -h 256 -f cel-subset.txt arial.ttf
./ttf2txf -o sans14.txf -s 14 -w 512 -h 256 -f cel-subset.txt arial.ttf
./ttf2txf -o sans16.txf -s 16 -w 512 -h 256 -f cel-subset.txt arial.ttf
./ttf2txf -o sans20.txf -s 20 -w 512 -h 256 -f cel-subset.txt arial.ttf
./ttf2txf -o sans24.txf -s 24 -w 512 -h 512 -f cel-subset.txt arial.ttf
./ttf2txf -o sans30.txf -s 30 -w 1024 -h 512 -f cel-subset.txt arial.ttf

./ttf2txf -o sansbold10.txf -s 10 -w 256 -h 256 -f cel-subset.txt arialbd.ttf
./ttf2txf -o sansbold12.txf -s 12 -w 256 -h 256 -f cel-subset.txt arialbd.ttf
./ttf2txf -o sansbold14.txf -s 14 -w 512 -h 256 -f cel-subset.txt arialbd.ttf
./ttf2txf -o sansbold16.txf -s 16 -w 512 -h 256 -f cel-subset.txt arialbd.ttf
./ttf2txf -o sansbold20.txf -s 20 -w 512 -h 512 -f cel-subset.txt arialbd.ttf
./ttf2txf -o sansbold24.txf -s 24 -w 512 -h 512 -f cel-subset.txt arialbd.ttf
# ./ttf2txf -o sansbold30.txf -s 30 -w 1024 -h 1024 -f cel-subset.txt arialbf.ttf
