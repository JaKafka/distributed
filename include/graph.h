/**
 * @file graph.h
 * @author Jakub Kawka, Marcin Kiżewski
 * @brief graph implementation for the Bellman-Ford algorithm
 * @version 0.1
 * @date 2025-05-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "stdlib.h"
#include "string.h"
#include "configchain.h"

#define NO_CONNECTION 9999

/**
 * @brief structure representing a graph
 * 
 * @param nodes number of nodes in the graph
 * @param costs array of costs between nodes
 */
struct graph
{
    int nodes;
    int *costs;
};

/**
 * @brief structure representing an edge in the graph
 * 
 * @param from node from which the edge starts
 * @param to node to which the edge goes
 * @param cost cost of the edge
 */
struct edge
{
    int from;
    int to;
    int cost;
};

/**
 * @brief structure representing the output of the parsing function
 * 
 * @param node_amount number of nodes in the graph
 * @param as_map array mapping AS numbers to node IDs
 * @param names array of names of the nodes
 * @param netgraph pointer to the graph structure
 */
struct parsing_output {
    int node_amount;
    int *as_map;
    char **names;
    struct graph *netgraph;
};

/*

COSTS [r0n0 r0n1 r0n2 ... r0nsize r1n0 ... r1nsize ... rsizen0 ... rsizensize]

*/

// If only there was a language which would let me have an object
// which constructs and destructs and also has some methods
// Hmmmm... it would probably be an upgrade to C, right?

/**
 * @brief function initializing a graph
 * 
 * @param size number of nodes in the graph
 * @return struct graph* pointer to the initialized graph
 */
struct graph *init_graph(int size)
{
    struct graph *newGraph = malloc(sizeof(struct graph));
    newGraph->nodes = size;
    newGraph->costs = calloc(size * size, sizeof(int));

    for (int i = 0; i < size * size; i++)
    {
        newGraph->costs[i] = NO_CONNECTION;
    }

    return newGraph;
}

/**
 * @brief function copying a graph
 * 
 * @param otherGraph pointer to the graph to be copied
 * @return struct graph* pointer to the copied graph
 */
struct graph *copy_graph(struct graph *otherGraph)
{
    struct graph *newGraph = malloc(sizeof(struct graph));
    newGraph->nodes = otherGraph->nodes;
    newGraph->costs = calloc(otherGraph->nodes * otherGraph->nodes, sizeof(int));
    memcpy(newGraph->costs, otherGraph->costs, otherGraph->nodes * otherGraph->nodes * sizeof(int));
    return newGraph;
}

/**
 * @brief function setting a bidirectional edge in the graph
 * 
 * @param G pointer to the graph
 * @param node1 first node of the edge
 * @param node2 second node of the edge
 * @param cost cost of the edge
 */
void set_edge_bidir(struct graph *G, int node1, int node2, int cost)
{
    G->costs[node1 * G->nodes + node2] = cost;
    G->costs[node2 * G->nodes + node1] = cost;
}

/**
 * @brief function setting a node in the graph
 * 
 * @param G pointer to the graph
 * @param node node to be set
 * @param cost cost of the node
 */
void set_node(struct graph *G, int node, int cost)
{
    G->costs[node * G->nodes + node] = cost;
}

/**
 * @brief function getting a node cost
 * 
 * @param G pointer to the graph
 * @param node node to be gotten
 * @return int cost of the node
 */
int get_node(struct graph *G, int node)
{
    return G->costs[node * G->nodes + node];
}

/**
 * @brief function setting an edge cost
 * 
 * @param G pointer to the graph
 * @param node_from first node of the edge
 * @param node_to second node of the edge
 * @param cost cost of the edge
 */
void set_edge(struct graph *G, int node_from, int node_to, int cost)
{
    G->costs[node_from * G->nodes + node_to] = cost;
}

/**
 * @brief function getting an edge cost
 * 
 * @param G pointer to the graph
 * @param node_from first node of the edge
 * @param node_to second node of the edge
 * @return int cost of the edge
 */
int get_edge(struct graph *G, int node_from, int node_to)
{
    return G->costs[node_from * G->nodes + node_to];
}

/**
 * @brief function extracting edges from the graph
 * 
 * @param G pointer to the graph
 * @param amount pointer to the number of edges
 * @return struct edge* pointer to the array of edges
 */
struct edge *extract_edges(struct graph *G, int *amount)
{
    // Count edges first
    int counted_amount = 0;
    for (int i = 0; i < G->nodes; i++)
    {
        for (int j = 0; j < G->nodes; j++)
        {
            if (i == j)
                continue;
            if (get_edge(G, i, j) != NO_CONNECTION)
                counted_amount++;
        }
    }

    struct edge *found = malloc(counted_amount * sizeof(struct edge));

    counted_amount = 0;

    for (int i = 0; i < G->nodes; i++)
    {
        for (int j = 0; j < G->nodes; j++)
        {
            if (i == j)
                continue;
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

/**
 * @brief function printing visual representation of the graph
 * 
 * @param G pointer to the graph
 */
void print_graph(struct graph *G)
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

/**
 * @brief function freeing the graph
 * 
 * @param G pointer to the graph
 */
void free_graph(struct graph *G)
{
    if (G != NULL)
    {
        free(G->costs);
        free(G);
    }
}

/**
 * @brief function getting data from a file
 * 
 * @param filename filenanme to be read
 * @return struct parsing_output* parsed data
 */
struct parsing_output * data_from_file(const char * filename)
{
    struct config_node *cfg = config_from_file(filename);

    describe_config_node(cfg);

    int router_count = 0;
    struct config_node *ptr = cfg;
    while (ptr)
    {
        router_count++;
        ptr = ptr->next;
    }

    int *as_map = malloc(sizeof(int) * router_count);
    struct graph *newgraph = init_graph(router_count);
    char **names = malloc(sizeof(char*) * router_count);

    int index = 0;
    ptr = cfg;
    while (ptr)
    {
        as_map[index] = ptr->as_number;
        names[index] = malloc(strlen(ptr->name) + 1);
        strcpy(names[index], ptr->name);
        ptr = ptr->next;
        index++;
    }

    // for (int i = 0; i < router_count; i++)
    // {
    //     printf("MAP %i <- %i\n", i, as_map[i]);
    // }

    ptr = cfg;
    index = 0;
    while (ptr)
    {
        struct config_peer *peer_ptr = ptr->peer_chain;
        while (peer_ptr)
        {
            int mapped_id = 0;
            while (mapped_id < router_count)
            {
                if (as_map[mapped_id] == peer_ptr->as_number)
                    break;
                mapped_id++;
            }

            printf("Setting %i -> %i with %i\n", index, mapped_id, peer_ptr->distance);
            set_edge(newgraph, index, mapped_id, peer_ptr->distance);
            peer_ptr = peer_ptr->next;
        }
        ptr = ptr->next;
        index++;
    }

    print_graph(newgraph);

    // int as_number, struct graph *src_net, int *as_map, const char *name

    // printf("Found %i routers\n", router_count);

    struct parsing_output *out = malloc(sizeof(struct parsing_output));

    out->node_amount = router_count;
    out->netgraph = newgraph;
    out->as_map = as_map;
    out->names = names;

    free_node_chain(cfg);
    return out;
}

#endif
