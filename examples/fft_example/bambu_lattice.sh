#!/bin/bash
script=$(readlink -e $0)
root_dir=$(dirname $script)
export PATH=../../src:../../../src:/opt/panda/bin:$PATH

BAMBU_OPTION="-v4 -lm -fsingle-precision-constant --evaluation -Os --device-name=LFE335EA8FN484C -ffast-math --libm-std-rounding --experimental-setup=BAMBU"
rm -rf run_dir_lattice
mkdir run_dir_lattice
cd run_dir_lattice
bambu --generate-tb=$root_dir/test_no_main.xml $root_dir/fft_float.c --generate-interface=WB4 --top-fname=FFT $BAMBU_OPTION
return_value=$?
if test $return_value != 0; then
   exit $return_value
fi
cd ..

rm -rf run_dir_lattice_1
mkdir run_dir_lattice_1
cd run_dir_lattice_1
bambu --generate-tb=$root_dir/test.xml $root_dir/fft_float.c -fwhole-program $BAMBU_OPTION
return_value=$?
if test $return_value != 0; then
   exit $return_value
fi
cd ..

