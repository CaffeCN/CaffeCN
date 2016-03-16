#!/bin/bash
# This script is used to extract loss or accuracy values from the outputs of Caffe.
# Usage:
# caffe_output_filter.sh [train|test] [loss|top1|top5] [file]
# -----------
show_usage () {
echo "
********************************************
This script is used to extract loss or accuracy values from the outputs of Caffe.

Usage: $0 <option(s)> SOURCE
caffe_output_filter.sh [<train|test>] [<loss|top1|top5>] [file].
Options:
	train|test: specify the stage of CNN.
	loss|top1|top5: extract loss or accuracy.
	file: read from a file or the standard input.
Examples: 
	./caffe_output_filter.sh train top1 caffe_output.txt
	cat caffe_output.txt | ./caffe_output_filter.sh test top5
	build/tools/caffe train --solver solver.prototxt |& ./caffe_output_filter.sh test top5 
********************************************"
}

if [ "$1" = "train" ];
then
	phase=Train
elif [ "$1" = "test" ]
then
	phase=Test
else
	show_usage
	exit 1
fi

if [ "$2" = "loss" ];
then
	obj=0
elif [ "$2" = "top1" ]
then
	obj=1
elif [ "$2" = "top5" ]
then
	obj=2
else
	show_usage
	exit 1
fi

# if [ -z $3 ];
# then
# 	show_usage
# 	exit 1
# elif [ -f $3 ];
# then
# 	log=$3
# else
# 	show_usage
# 	exit 1
# fi

while read line
do
	grep --line-buffered -o "${phase} net output #${obj}.*" | cut -d " " -f 7
done < "${3:-/dev/stdin}"
