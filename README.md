# A (7,16)-graph on 648 vertices: R(7,16) >= 649

The dynamic survey *Small Ramsey Numbers* (S. P. Radziszowski, Electron. J. Combin. DS1, revision #18, 2026) lists R(7,16) >= 629, from item 2.3.i:

> R(2k-1, l) >= 4R(k, l-1) - 3 for k >= 2, l >= 5
> (Xu, Xie, Exoo, Radziszowski, Electron. J. Combin. 11 (2004) #R35, Theorem 9; two-colour case of Theorem 7)

which corresponds to R(4,15) >= 158 (Table I of the same revision already has R(4,15) >= 159). The google-research repository ([file](https://github.com/google-research/google-research/blob/master/ramsey_number_bounds/improved_bounds/R(4,%2015)%20%3E=%20163.txt)) gives a 162-vertex (4,15)-graph (`ramsey_number_bounds/improved_bounds/R(4, 15) >= 163.txt`, commit 0d94f8c, 2026-10-22). Applying the construction with k = 4, l = 16 to that graph gives a (7,16)-graph on 4 * 162 = 648 vertices, so

**R(7,16) >= 649.**

This depends on the 162-vertex (4,15)-graph posted in the google-research repository, which is not (as far as I know) published in a paper. The input graph was checked independently here (see Verification).

(With R(4,15) >= 159 from Table I of DS1 #18 alone, the same item gives R(7,16) >= 633.)

## Status

- 2026-11-09: reported to the maintainer of the dynamic survey (S. P. Radziszowski). Not yet confirmed or listed.
- 2026-11-10: the survey author acknowledged receipt; inclusion will be decided in a future revision.
- The bound depends on the 162-vertex (4,15)-graph from google-research, checked independently here.

## Quick start

```sh
git clone https://github.com/8rulerstar/ramsey-r7-16 && cd ramsey-r7-16
sh reproduce.sh      # rebuilds the graph from the input and runs every check (about 20 minutes on 8 cores)
```

`reproduce.sh` prints "input matches", "rebuilt graph matches", the automorphism check, and "NONE" for every clique and independent-set search range.

## Files

| file | content |
|---|---|
| `R7_16_648.txt` | the 648-vertex graph: first line n, then n rows of 0/1 (1 = edge). No K7, no independent set of size 16. |
| `R(4, 15) >= 163.txt` | the input graph, unchanged copy from google-research (a 162 x 162 0/1 matrix written as a nested list); Apache License 2.0 |
| `R4_15_162.txt` | modified file: the same input graph converted to the plain format by `convert_gr.py` (only the file format changed) |
| `build_23i.py` | the construction of the theorem (vertices (i, j), i = 0..3 copies, j = vertex of the input graph) |
| `verify.c`, `verify704.c` | plain branch-and-bound checker (greedy colouring bound), no symmetry assumptions |
| `vpar.c` | the same search split by root vertex, used with the automorphisms below |
| `convert_gr.py` | converts the google-research file to the plain format |
| `check_automorphisms.py` | checks that the copy permutations are automorphisms of the 648-vertex graph |
| `reproduce.sh` | rebuilds the graph and runs every check (the independent-set search starts 8 processes and takes about 20 minutes on 8 cores) |
| `SHA256SUMS` | checksums of `R7_16_648.txt`, `R(4, 15) >= 163.txt` and `R4_15_162.txt` |

## Construction (as implemented in `build_23i.py`)

Vertices (i, j), i in {0,1,2,3}, j in V(G). Let G be the (4,15)-graph.
- inside one copy: (i, j1) ~ (i, j2) iff j1 j2 is an edge of G;
- between copies {0,1} and {2,3}: adjacent iff j1 = j2 or j1 j2 is an edge of G;
- between copies 0 and 1, and between 2 and 3: adjacent iff j1 != j2.

## Verification

1. The input graph has no K4 and no independent 15-set (`verify 4 15`).
2. The copy permutations (0 1), (2 3) and (0 2)(1 3) are automorphisms of H (checked on the whole adjacency matrix by `check_automorphisms.py`). They act transitively on the four copies, so every clique or independent set can be mapped to one that meets copy 0 (vertices 0..161).
3. Every clique (resp. independent set) S meeting copy 0 is found exactly once: take v = the smallest vertex of S in copy 0 as the root; then S minus {v} lies in N(v) (resp. in the non-neighbourhood of v) and contains no vertex of {0, ..., v-1} within copy 0. `vpar` searches, for each root v in 0..161, inside N(v) minus {0, ..., v-1} (mode 0, cliques) or inside the non-neighbourhood of v minus {0, ..., v-1} (mode 1, independent sets).
4. Result: no K7 (roots 0..161, 9.7e7 search nodes) and no independent set of size 16 (roots 0..161, 1.87e10 search nodes). Run on an Apple M4 Pro, 8 processes in parallel, about 20 minutes of wall time for the independent-set search (logs in `logs/`).
5. The plain checker without symmetry assumptions (`verify704 7 16`) was not run to completion (it did not finish within about an hour on one core); the exhaustive check above relies on the automorphisms in step 2. The reduction in `vpar` was cross-checked against `verify` on small graphs built by the same construction.
6. Sanity check of the same code path: the construction applied to a 58-vertex (4,8)-graph gives a 232-vertex graph, which `verify` confirms is a (7,9)-graph.

## Contact

Byungkun Jang, independent researcher, bkbkjang@gmail.com

## License

Code in this directory: MIT (see `LICENSE`). The file `R(4, 15) >= 163.txt` is an unchanged copy from google-research and `R4_15_162.txt` is a format conversion of it; both are under the Apache License 2.0 (copy in `LICENSES/Apache-2.0.txt`).
