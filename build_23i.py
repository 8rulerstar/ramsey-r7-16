"""XXER 2004 Theorem 9 construction (EJC 11 #R35, Thm 7 colouring, p.10-11, 15):
from a (k,l-1)-graph G on m vertices build H on 4m vertices with no K_{2k-1} and no independent l-set.
Vertices (i,j), i=0..3 copies, j=0..m-1. Colour rules (colour 1 or 2 => edge of H, colour 3 => non-edge):
  inside a copy: G-edge -> 2, else 3
  between {0,1} and {2,3}: j1==j2 -> 1; else G-edge -> 2, non-edge -> 3
  between 0-1 and between 2-3: j1==j2 -> 3; else -> 1
Usage: python build_23i.py G.txt out.txt K L [--complement-in] [--complement-out]
  K, L = parameters of the theorem: (after --complement-in) G must be a (K, L-1)-graph, output is a
  (2K-1, L)-graph (before --complement-out). Theorem 9 requires L >= 5 and K >= 2; both are enforced,
  and G is checked with ./verify before building."""
import os, subprocess, sys
tok = open(sys.argv[1]).read().split(); m = int(tok[0]); G = [[c == '1' for c in r] for r in tok[1:1 + m]]
if "--complement-in" in sys.argv: G = [[(i != j) and not G[i][j] for j in range(m)] for i in range(m)]
K, L = int(sys.argv[3]), int(sys.argv[4])
assert L >= 5 and K >= 2, "XXER Theorem 9 needs l >= 5 and k >= 2"
_tmp = sys.argv[2] + ".G.txt"
open(_tmp, "w").write(f"{m}\n" + "\n".join("".join("1" if x else "0" for x in r) for r in G) + "\n")
_r = subprocess.run([os.path.join(os.path.dirname(os.path.abspath(__file__)), "verify"), str(K), str(L - 1), _tmp], capture_output=True, text=True)
os.unlink(_tmp)
assert _r.returncode == 0, "input G is not a (K, L-1)-graph: " + _r.stdout
N = 4 * m
def edge(a, b):
    i1, j1 = divmod(a, m); i2, j2 = divmod(b, m)
    if a == b: return False
    if i1 == i2: return G[j1][j2]
    same_side = {i1, i2} in ({0, 1}, {2, 3})
    if not same_side:                       # one in {0,1}, other in {2,3}
        return True if j1 == j2 else G[j1][j2]
    return j1 != j2                         # 0-1 or 2-3: equal index -> colour 3, else colour 1
H = [[edge(a, b) for b in range(N)] for a in range(N)]
if "--complement-out" in sys.argv: H = [[(a != b) and not H[a][b] for b in range(N)] for a in range(N)]
assert all(H[a][b] == H[b][a] for a in range(N) for b in range(N))
open(sys.argv[2], "w").write(f"{N}\n" + "\n".join("".join("1" if x else "0" for x in r) for r in H) + "\n")
print(f"built {N}-vertex graph from {m}-vertex G")
