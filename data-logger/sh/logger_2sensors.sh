#!/bin/sh

if [ -d $1 ]; then
	echo "ERROR: Directory $1 already exists."
else
	mkdir $1
	ssm-logger -n LS3D -i 0 -l $1/LS3D.log
	ssm-logger -n Top-URG -i 1 -l $1/Top-URG.log &
fi