/**
 * @file bellford.h
 * @author Jakub Kawka, Marcin Kiżewski
 * @brief Bellman-Ford algorithm implementation
 * @version 0.1
 * @date 2025-05-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef BELLFORD_H
#define BELLFORD_H

#include "graph.h"
#include "stdlib.h"
#include "stdio.h"

#define INFINITY 9999
#define NULL_PREDECESSOR -1

/**
 * @brief struct representing the results of the Bellman-Ford algorithm
 * 
 * @param distance array of distances from the source node to each node
 * @param predecessor array of predecessors for each node
 * @param size number of nodes in the graph
 */
struct bellman_results {
    int *distance;
    int *predecessor;
    int size;
};

/**
 * @brief function implementing the Bellman-Ford algorithm
 * 
 * @param G pointer to the graph
 * @param source_id ID of the source node
 * @return struct bellman_results results of the algorithm
 */
struct bellman_results bellman_ford(struct graph *G, int source_id)
{

    // Prepare the structure for the algorithm
    int *distances = (int *)calloc(G->nodes, sizeof(int));
    int *predecessor = (int *)calloc(G->nodes, sizeof(int));

    int node_count = G->nodes;

    struct graph *working_graph = copy_graph(G);

    for (int i = 0; i < node_count; i++)
    {
        distances[i] = INFINITY;
        predecessor[i] = NULL_PREDECESSOR;
    }

    distances[source_id] = 0;

    // Retrieve edge data, this representation is preferred by the algorithm
    int edge_number;
    struct edge *E = extract_edges(working_graph, &edge_number);

    // Relax edges repeatedly

    // for (int i = 0; i < edge_number; i++)
    // {
    //     printf("From %i to %i cost %i\n", E[i].from, E[i].to, E[i].cost);
    // }

    // Relax edges
    for (int i = 1; i < node_count; i++)
    {
        for (int j = 0; j < edge_number; j++)
        {
            /*
                Edge (U,V) with weight W
                U E[j].from
                V E[j].to
                W E[j].cost
            */

            // If distance to [TO] node via edge FROM -> TO is lesser
            // travel to node [TO] via [FROM]
            if (distances[E[j].from] + E[j].cost < distances[E[j].to])
            {
                distances[E[j].to] = distances[E[j].from] + E[j].cost;
                predecessor[E[j].to] = E[j].from;
            }
        }
    }

    // Check for negative-weight cycles

    for (int j = 0; j < edge_number; j++)
    {
        int u = E[j].from;
        int v = E[j].to;
        int w = E[j].cost;

        if (distances[u] + w < distances[v])
        {
            // We don't actually care about the cycle
            // Just that it exists is enough to throw an error

            // predecessor[v] = u;

            // int *visited = (int*) calloc(node_count, sizeof(int));
            // visited[v] = 1;

            // while (visited[u] == 0) {
            //     visited[u] = 1;
            //     u = predecessor[u];
            // }

            // int *cycle = (int*) calloc(node_count, sizeof(int));
            // int cycle_counter = 0;

            // for (int k = 0; k < node_count; k++)
            // {
            //     cycle[k] = NO_CONNECTION;
            // }

            // cycle[0] = u;
            // cycle_counter++;

            // v = predecessor[u];

            // while (v != u) {
            //     cycle[cycle_counter] = v;
            //     v = predecessor[v];
            // }

            printf("Graph contains a negative-weight cycle\n");

            // free(cycle);
            // free(visited);

        }
        
    }

    struct bellman_results returned_data;
    returned_data.distance = distances;
    returned_data.predecessor = predecessor;
    returned_data.size = node_count;

    free_graph(working_graph);
    free(E);

    return returned_data;
}

// Source for the algorithm
// https://en.wikipedia.org/wiki/Bellman%E2%80%93Ford_algorithm

#endif
