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

#define MAXN 512
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

int main(int argc, char **argv) {
    if (argc != 4) { fprintf(stderr, "usage: %s s t graph.txt\n", argv[0]); return 2; }
    int s = atoi(argv[1]), t = atoi(argv[2]);
    if (s < 1 || t < 1 || s > MAXN + 1 || t > MAXN + 1) { fprintf(stderr, "s,t out of range\n"); return 2; }
    FILE *f = fopen(argv[3], "r");
    if (!f || fscanf(f, "%d", &n) != 1 || n < 1 || n > MAXN) { fprintf(stderr, "bad input\n"); return 2; }
    char row[MAXN + 8];
    for (int i = 0; i < n; i++) {
        if (fscanf(f, "%513s", row) != 1 || (int)strlen(row) != n) { fprintf(stderr, "bad row %d\n", i); return 2; }
        for (int j = 0; j < n; j++) {
            if (row[j] != '0' && row[j] != '1') { fprintf(stderr, "bad char in row %d\n", i); return 2; }
            if (row[j] == '1') set(&adj[i], j);
        }
    }
    { char extra[8]; if (fscanf(f, "%7s", extra) == 1) { fprintf(stderr, "trailing data\n"); return 2; } }
    for (int i = 0; i < n; i++) {
        if ((adj[i].w[i >> 6] >> (i & 63)) & 1) { fprintf(stderr, "loop at %d\n", i); return 2; }
        for (int j = 0; j < n; j++) {
            int a = (adj[i].w[j >> 6] >> (j & 63)) & 1, b = (adj[j].w[i >> 6] >> (i & 63)) & 1;
            if (a != b) { fprintf(stderr, "not symmetric at %d,%d\n", i, j); return 2; }
        }
    }
    int bad = 0;
    if (has_clique(s, 0)) {
        printf("FAIL: clique of size %d:", s); for (int i = 0; i < s; i++) printf(" %d", witness[i]); printf("\n"); bad = 1;
    } else printf("no K%d (%lld nodes)\n", s, nodes);
    if (has_clique(t, 1)) {
        printf("FAIL: independent set of size %d:", t); for (int i = 0; i < t; i++) printf(" %d", witness[i]); printf("\n"); bad = 1;
    } else printf("no independent set of size %d (%lld nodes)\n", t, nodes);
    if (!bad) printf("OK: R(%d,%d) > %d\n", s, t, n);
    return bad;
}
