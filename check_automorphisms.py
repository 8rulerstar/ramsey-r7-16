"""Check that the copy permutations (0 1), (2 3), (0 2)(1 3) are automorphisms of the 648-vertex graph
(vertex index = copy * 162 + j). Used by the symmetric search in vpar.c."""
import sys
t = open(sys.argv[1] if len(sys.argv) > 1 else "R7_16_648.txt").read().split()[1:]
n, m = len(t), 162
assert n == 4 * m
for p in ([1, 0, 2, 3], [0, 1, 3, 2], [2, 3, 0, 1]):
    f = [p[v // m] * m + v % m for v in range(n)]
    ok = all(t[a][b] == t[f[a]][f[b]] for a in range(n) for b in range(n))
    print("copy permutation", p, "automorphism:", ok)
    assert ok
