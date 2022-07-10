#!/bin/bash
source /opt/intel/inteloneapi/setvars.sh > /dev/null 2>&1

#Command Line Arguments
arg="" # set matrix size
src="Grobner/"

echo ====================
echo test
dpcpp ${src}test_device.cpp ${src}test_host.cpp -o ${src}testexe -w -O3
./${src}testexe$arg
