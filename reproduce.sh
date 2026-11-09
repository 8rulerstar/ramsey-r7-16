#!/bin/sh
# Rebuild and check the 648-vertex (7,16)-graph. Needs a C compiler and python3.
set -e
cd "$(dirname "$0")"
cc -O3 -o verify verify.c
cc -O3 -o verify704 verify704.c
cc -O3 -o vpar vpar.c
python3 convert_gr.py "R(4, 15) >= 163.txt" in.txt
cmp in.txt R4_15_162.txt && echo "input matches R4_15_162.txt"
./verify 4 15 R4_15_162.txt                       # input: no K4, no independent 15-set
python3 build_23i.py R4_15_162.txt out.txt 4 16
cmp out.txt R7_16_648.txt && echo "rebuilt graph matches R7_16_648.txt"
# 648 vertices: the copy permutations (0 1), (2 3), (0 2)(1 3) are automorphisms, so it suffices to
# search cliques / independent sets whose smallest vertex in copy 0 (vertices 0..161) is the root.
python3 check_automorphisms.py R7_16_648.txt
./vpar 7 R7_16_648.txt 0 0 162                    # no K7
for r in "0 10" "10 22" "22 36" "36 52" "52 72" "72 96" "96 126" "126 162"; do
  set -- $r; ./vpar 16 R7_16_648.txt 1 $1 $2 &    # no independent 16-set, split over 8 cores
done; wait
# full plain check (slow, single core): ./verify704 7 16 R7_16_648.txt
