# A (7,16)-graph on 648 vertices: R(7,16) >= 649

The dynamic survey *Small Ramsey Numbers* (S. P. Radziszowski, Electron. J. Combin. DS1, revision #18, 2026) lists R(7,16) >= 629, from item 2.3.i:

> R(2k-1, l) >= 4R(k, l-1) - 3 for k >= 2, l >= 5
> (Xu, Xie, Exoo, Radziszowski, Electron. J. Combin. 11 (2004) #R35, Theorem 9; two-colour case of Theorem 7)

with R(4,15) >= 158. The google-research repository gives a 162-vertex (4,15)-graph (`ramsey_number_bounds/improved_bounds/R(4, 15) >= 163.txt`, commit 0d94f8c, 2026-10-22). Applying the construction with k = 4, l = 16 to that graph gives a (7,16)-graph on 4 * 162 = 648 vertices, so

**R(7,16) >= 649.**

(With R(4,15) >= 159 from Table I of DS1 #18 alone, the same item gives R(7,16) >= 633.)

## Files

| file | content |
|---|---|
| `R7_16_648.txt` | the 648-vertex graph: first line n, then n rows of 0/1 (1 = edge). No K7, no independent set of size 16. |
| `R(4, 15) >= 163.txt` | the input graph, unchanged copy from google-research (Apache License 2.0) |
| `R4_15_162.txt` | the same input graph in the plain format (`convert_gr.py`) |
| `build_23i.py` | the construction of the theorem (vertices (i, j), i = 0..3 copies, j = vertex of the input graph) |
| `verify.c`, `verify704.c` | plain branch-and-bound checker (greedy colouring bound), no symmetry assumptions |
| `vpar.c` | the same search split by root vertex, used with the automorphisms below |
| `reproduce.sh` | rebuilds the graph and runs every check |
| `SHA256SUMS` | checksums of the two graph files |

## Construction (as implemented in `build_23i.py`)

Vertices (i, j), i in {0,1,2,3}, j in V(G). Let G be the (4,15)-graph.
- inside one copy: (i, j1) ~ (i, j2) iff j1 j2 is an edge of G;
- between copies {0,1} and {2,3}: adjacent iff j1 = j2 or j1 j2 is an edge of G;
- between copies 0 and 1, and between 2 and 3: adjacent iff j1 != j2.

## Verification

1. The input graph has no K4 and no independent 15-set (`verify 4 15`).
2. The copy permutations (0 1), (2 3) and (0 2)(1 3) are automorphisms of H (checked on the whole adjacency matrix). They act transitively on the four copies, so every clique or independent set can be mapped to one that meets copy 0 (vertices 0..161).
3. `vpar` searches, for each root v in 0..161, cliques (or independent sets) containing v inside N(v) minus {0, ..., v-1}. Over all roots this covers every clique meeting copy 0.
4. Result: no K7 (roots 0..161, 9.7e7 search nodes) and no independent set of size 16 (roots 0..161, 1.87e10 search nodes). Run on an Apple M4 Pro, 8 processes in parallel, about 20 minutes of wall time for the independent-set search (logs in `logs/`).
5. Sanity check of the same code path: the construction applied to a 58-vertex (4,8)-graph gives a 232-vertex graph, which `verify` confirms is a (7,9)-graph.

## Contact

Byungkun Jang, independent researcher, bkbkjang@gmail.com

## License

Code in this directory: MIT. The file `R(4, 15) >= 163.txt` is from google-research (Apache License 2.0).
