#!/bin/bash
script=$(readlink -e $0)
root_dir=$(dirname $script)
export PATH=../../src:../../../src:/opt/panda/bin:$PATH

rm -rf constrained_synth_xilinx_v5330_ise
mkdir -p constrained_synth_xilinx_v5330_ise
cd constrained_synth_xilinx_v5330_ise
echo "# ISE synthesis and ICARUS simulation"
bambu -v4 $root_dir/module.c --generate-tb=$root_dir/test.xml --simulator=ICARUS --device-name=xc5vlx330t,-2,ff1738 --evaluation --experimental-setup=BAMBU --generate-interface=WB4 $root_dir/constraints_STD.xml --clock-period=5  --cprf=0.9 --skip-pipe-parameter=1
return_value=$?
if test $return_value != 0; then
   exit $return_value
fi
cd ..
