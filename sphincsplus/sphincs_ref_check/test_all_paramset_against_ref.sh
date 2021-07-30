#!/bin/bash

# change working directory to script location
cd "$(dirname "$0")"

set i = 0
for filename in spx_bin/*; do
	echo "Executing parameter set $filename"
	echo "-------"
	./$filename
	if [ $? -ne 0 ]; then 
		echo "FAILED!" 
		exit -1
	 fi
	((i=i+1))
	echo ""
	echo ""
done


echo $i parameter sets have been sucessfuly executed.

if [ $i -ne 24 ]; then
	echo "ERROR! Expecting 24 parameter sets!"
fi
