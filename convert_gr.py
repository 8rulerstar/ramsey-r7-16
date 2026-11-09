"""Convert the google-research matrix file ('[[0 1 ...]]') to the plain format used here (n line + 0/1 rows)."""
import re, sys
d = [c for c in re.findall(r"[01]", open(sys.argv[1]).read())]
n = int(round(len(d) ** 0.5)); assert n * n == len(d)
open(sys.argv[2], "w").write(f"{n}\n" + "\n".join("".join(d[i*n:(i+1)*n]) for i in range(n)) + "\n")
