#!/bin/sh

if [ -d $1 ]; then
	echo "ERROR: Directory $1 already exists."
else
	ssm-logger -n Top-URG -i 0 -l $1/Top-URG.log
fi