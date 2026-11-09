// Independent Ramsey certificate checker.
// Usage: ./verify s t graph.txt
//   graph.txt: first line n, then n lines of '0'/'1' (adjacency matrix).
// Exit 0 and prints "OK" iff the graph has no clique of size s and
// no independent set of size t (so R(s,t) > n).
// No symmetry assumptions: plain branch and bound with greedy colouring bound.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAXN 704
#define W (MAXN / 64)
typedef struct { uint64_t w[W]; } bs;

static int n;
static bs adj[MAXN];

static inline void set(bs *b, int i) { b->w[i >> 6] |= 1ULL << (i & 63); }
static inline int popc(const bs *b) { int c = 0; for (int i = 0; i < W; i++) c += __builtin_popcountll(b->w[i]); return c; }
static inline int empty(const bs *b) { for (int i = 0; i < W; i++) if (b->w[i]) return 0; return 1; }
static inline int first(const bs *b) { for (int i = 0; i < W; i++) if (b->w[i]) return i * 64 + __builtin_ctzll(b->w[i]); return -1; }
static inline void clr(bs *b, int i) { b->w[i >> 6] &= ~(1ULL << (i & 63)); }
static inline bs and_(const bs *a, const bs *b) { bs r; for (int i = 0; i < W; i++) r.w[i] = a->w[i] & b->w[i]; return r; }
static inline bs andnot(const bs *a, const bs *b) { bs r; for (int i = 0; i < W; i++) r.w[i] = a->w[i] & ~b->w[i]; return r; }

static bs g[MAXN];          // graph currently searched
static int target;
static long long nodes;
static int witness[MAXN + 1], wlen;

// greedy colouring upper bound on clique size in P
static int colour_bound(bs P) {
    int colours = 0;
    while (!empty(&P)) {
        colours++;
        bs Q = P;
        while (!empty(&Q)) {
            int v = first(&Q);
            clr(&Q, v); clr(&P, v);
            Q = andnot(&Q, &g[v]);
        }
    }
    return colours;
}

static int expand(int depth, bs P) {
    nodes++;
    if (depth >= target) return 1;
    if (depth + popc(&P) < target) return 0;
    if (depth + colour_bound(P) < target) return 0;
    while (!empty(&P)) {
        if (depth + popc(&P) < target) return 0;
        int v = first(&P);
        clr(&P, v);
        witness[depth] = v;
        if (expand(depth + 1, and_(&P, &g[v]))) return 1;
    }
    return 0;
}

static int has_clique(int k, int complement) {
    for (int i = 0; i < n; i++) {
        memset(&g[i], 0, sizeof(bs));
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            int e = (adj[i].w[j >> 6] >> (j & 63)) & 1;
            if (complement ? !e : e) set(&g[i], j);
        }
    }
    target = k; nodes = 0;
    bs P; memset(&P, 0, sizeof P);
    for (int i = 0; i < n; i++) set(&P, i);
    return expand(0, P);
}

/* vpar: parallel/symmetric part of verify. Usage: ./vpar k file mode lo hi
   mode 0: clique K_k among '1' edges, mode 1: independent k-set. Only cliques whose smallest vertex in [0,COPY)
   lies in [lo,hi) are searched (roots v, candidates = N(v) minus {0..v-1}). Combined over lo..hi = 0..COPY this is
   complete for graphs whose automorphisms move every vertex into [0,COPY) (checked separately). */
int main(int argc, char **argv) {
    if (argc != 6) { fprintf(stderr, "usage: vpar k file mode lo hi\n"); return 2; }
    int k = atoi(argv[1]), mode = atoi(argv[3]), lo = atoi(argv[4]), hi = atoi(argv[5]);
    FILE *f = fopen(argv[2], "r");
    if (!f || fscanf(f, "%d", &n) != 1 || n < 1 || n > MAXN) { fprintf(stderr, "bad input\n"); return 2; }
    char row[MAXN + 8];
    for (int i = 0; i < n; i++) {
        if (fscanf(f, "%705s", row) != 1 || (int)strlen(row) != n) { fprintf(stderr, "bad row %d\n", i); return 2; }
        for (int j = 0; j < n; j++) if (row[j] == '1') set(&adj[i], j);
    }
    for (int i = 0; i < n; i++) {
        memset(&g[i], 0, sizeof(bs));
        for (int j = 0; j < n; j++) { if (i == j) continue; int e = (adj[i].w[j >> 6] >> (j & 63)) & 1; if (mode ? !e : e) set(&g[i], j); }
    }
    target = k; nodes = 0;
    for (int v = lo; v < hi; v++) {
        bs Q = g[v]; for (int u = 0; u < v; u++) clr(&Q, u);
        witness[0] = v;
        if (expand(1, Q)) { printf("FOUND %s of size %d:", mode ? "independent set" : "clique", k); for (int i = 0; i < k; i++) printf(" %d", witness[i]); printf("\n"); return 1; }
    }
    printf("NONE mode=%d k=%d roots %d..%d (%lld nodes)\n", mode, k, lo, hi - 1, nodes);
    return 0;
}
