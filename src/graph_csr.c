
#include <stdlib.h>
#include "graph.h"
CSRGraph* convert_to_csr(Graph* g) {
    if (!g) return NULL;
    // 1. count edges:
    int n = g->num_vertices;
    int m = 0;
    for (int v = 0; v < n; v++) {
        for (Edge* e = g->vertices[v].head; e != NULL; e = e->next) m++;
    }

    // 2. allocate arrays:
    CSRGraph* csr = (CSRGraph*)malloc(sizeof(CSRGraph));
    if (!csr) return NULL;
    csr->num_vertices = n;
    csr->num_edges = m;

    csr->row_ptr = (int*)malloc((size_t)(n + 1) * sizeof(int));
    csr->col_idx = (int*)malloc((size_t)m * sizeof(int));
    if (!csr->row_ptr || !csr->col_idx) {
        free(csr->row_ptr); free(csr->col_idx); free(csr);
        return NULL;
    }

    // 3. fill row_ptr and 4. fill col_idx:
    int idx = 0;
    csr->row_ptr[0] = 0;
    for (int v = 0; v < n; v++) {
        for (Edge* e = g->vertices[v].head; e != NULL; e = e->next) {
            csr->col_idx[idx++] = e->dst;
        }
        csr->row_ptr[v + 1] = idx;
    }

    return csr;
}
void free_csr(CSRGraph* g) { 
    if (!g) return; free(g->row_ptr); free(g->col_idx); free(g); 
}
