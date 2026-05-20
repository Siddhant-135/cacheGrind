#include <stdlib.h>
#include "graph.h"
int bfs_csr(CSRGraph* g, int source, int* dist) { 
    int* queue = malloc((size_t)g->num_vertices * sizeof(*queue));
    if (!queue) return -1; // malloc failed

    int visited_count = 0, curr_queue_size = 0, head = 0, tail = 0;
    for (int i = 0; i < g->num_vertices; i++) { // ENSURE THE INCOMING DIST ARRAY IS INITIALIZED TO -1
        dist[i] = -1;
    }
    // first entry:
    dist[source] = 0;
    queue[tail] = source; // tail points to the last valid entry (to the right). head points to the valid entry to be popped(to the left). curr_queue_size = tail - head + 1
    curr_queue_size++; visited_count++;
    int v = 0, u = 0; // v is current.
    while(curr_queue_size > 0) {
        v = queue[head];
        head++;
        curr_queue_size--;

        for(int i = g->row_ptr[v]; i<g->row_ptr[v+1]; i++){
            u = g->col_idx[i];
            if(dist[u]==-1){ // Corrected: check if u not visited before adding
                dist[u] = dist[v]+1;
                tail++;
                queue[tail]=u;
                visited_count++; curr_queue_size++;
            }
        }
    }
    free(queue);
    return visited_count;
}