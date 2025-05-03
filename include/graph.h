#ifndef GRAPH_H
#define GRAPH_H

#include "stdlib.h"
#include "string.h"

#define NO_CONNECTION 9999

struct graph {
    int nodes;
    int *costs;
};

struct edge {
    int from;
    int to;
    int cost;
};

/*

COSTS [r0n0 r0n1 r0n2 ... r0nsize r1n0 ... r1nsize ... rsizen0 ... rsizensize]

*/

// If only there was a language which would let me have an object
// which constructs and destructs and also has some methods
// Hmmmm... it would probably be an upgrade to C right?

struct graph *init_graph(int size) {
    struct graph *newGraph = malloc(sizeof(struct graph));
    newGraph->nodes = size;
    newGraph->costs = calloc(size * size, sizeof(int));

    for (int i = 0; i < size * size; i++)
    {
        newGraph->costs[i] = NO_CONNECTION;
    }

    return newGraph;
}

struct graph *copy_graph(struct graph *otherGraph) {
    struct graph *newGraph = malloc(sizeof(struct graph));
    newGraph->nodes = otherGraph->nodes;
    newGraph->costs = calloc(otherGraph->nodes * otherGraph->nodes, sizeof(int));
    memcpy(newGraph->costs, otherGraph->costs, otherGraph->nodes * otherGraph->nodes * sizeof(int));
    return newGraph;
}

void set_edge_bidir(struct graph* G, int node1, int node2, int cost)
{
    G->costs[node1 * G->nodes + node2] = cost;
    G->costs[node2 * G->nodes + node1] = cost;
}

void set_node(struct graph* G, int node, int cost)
{
    G->costs[node * G->nodes + node] = cost;
}

int get_node(struct graph* G, int node)
{
    return G->costs[node * G->nodes + node];
}

void set_edge(struct graph* G, int node_from, int node_to, int cost)
{
    G->costs[node_from * G->nodes + node_to] = cost;
}

int get_edge(struct graph* G, int node_from, int node_to)
{
    return G->costs[node_from * G->nodes + node_to];
}

struct edge* extract_edges(struct graph* G, int * amount) {
    // Count edges first
    int counted_amount = 0;
    for (int i = 0; i < G->nodes; i++)
    {
        for (int j = 0; j < G->nodes; j++)
        {
            if (i == j) continue;
            if (get_edge(G, i, j) != NO_CONNECTION) counted_amount++;
        }
    }

    struct edge * found = malloc(counted_amount * sizeof(struct edge));

    counted_amount = 0;

    for (int i = 0; i < G->nodes; i++)
    {
        for (int j = 0; j < G->nodes; j++)
        {
            if (i == j) continue;
            int value = get_edge(G, i, j);
            if (value != NO_CONNECTION)
            {
                found[counted_amount].from = i;
                found[counted_amount].to = j;
                found[counted_amount].cost = value;
                counted_amount++;
            }
        }
    }

    *amount = counted_amount;
    return found;
}

void print_graph(struct graph* G)
{
    int graph_size = G->nodes;

    for (int i = 0; i < (graph_size * graph_size); i++)
    {
        printf("%i\t", G->costs[i]);
        if (i % graph_size == graph_size - 1)
        {
            printf("\n");
        }
    }
}

void free_graph(struct graph * G)
{
    free(G->costs);
    free(G);
}

#endif
