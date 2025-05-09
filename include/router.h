/**
 * @file router.h
 * @author Jakub Kawka, Marcin Kiżewski
 * @brief router representation implementation for the Bellman-Ford algorithm
 * @version 0.1
 * @date 2025-05-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ROUTER_H
#define ROUTER_H

#include "bellford.h"
#include "graph.h"
#include "string.h"
#include "stdlib.h"

/**
 * @brief structure representing a router
 * 
 * @param name name of the router
 * @param as_number AS number of the router
 * @param as_map array mapping node IDs to AS numbers
 * @param next_hop array mapping node IDs to the next hop node IDs
 * @param distance array of distances to each node
 * @param tracked_nodes number of nodes in the network
 */
struct router
{
    char *name;
    int as_number;     // Identifying number of this autonomous system
    int *as_map;       // [NODE ID -> AS NUMBER]
    int *next_hop;     // Next hop [NODE ID -> NODE ID]
    int *distance;     // Distance to NODE ID
    int tracked_nodes; // Amount of nodes in the network
};

/**
 * @brief function generating routing information for a router using the Bellman-Ford algorithm
 * 
 * @param as_number AS number of the router
 * @param src_net pointer to the source network graph
 * @param as_map array mapping node IDs to AS numbers
 * @param name name of the router
 * @return struct router* pointer to the generated router structure
 */
struct router *generate_routing_info(int as_number, struct graph *src_net, int *as_map, const char *name)
{
    struct router *rtr = (struct router *)malloc(sizeof(struct router));

    rtr->as_number = as_number;
    rtr->tracked_nodes = src_net->nodes;

    rtr->name = (char *)malloc(strlen(name) + 1);
    strcpy(rtr->name, name);

    rtr->as_map = (int *)malloc(sizeof(int) * src_net->nodes);
    memcpy(rtr->as_map, as_map, sizeof(int) * src_net->nodes);

    rtr->next_hop = (int *)malloc(sizeof(int) * src_net->nodes);

    int my_node_id = 0;
    for (int i = 0; i < rtr->tracked_nodes; i++)
    {
        rtr->next_hop[i] = NO_CONNECTION;

        if (as_map[i] == as_number)
        {
            my_node_id = i;
            break;
        }
    }

    rtr->next_hop[my_node_id] = my_node_id;

    struct bellman_results res = bellman_ford(src_net, my_node_id);
    int had_to_fix = 0;

    printf("My ID %i\n", my_node_id);


    for (int i = 0; i < rtr->tracked_nodes; i++)
    {
        printf("Initialized hop %i with %i\n", i, i);
        rtr->next_hop[i] = i;
    }

    do
    {
        had_to_fix = 0;
        // Loop over all nodes
        for (int i = 0; i < rtr->tracked_nodes; i++)
        {
            // Don't touch source node
            if (i == my_node_id)
                continue;

            // If next hop is linked directly, ignore
            if (res.predecessor[rtr->next_hop[i]] == my_node_id)
                continue;
            else
            {
                // Else alter next hop to that of its predecessor
                rtr->next_hop[i] = res.predecessor[rtr->next_hop[i]];
                had_to_fix = 1;

                printf("Swapped node %i with %i\n", i, rtr->next_hop[i]);
            }
        }

    } while (had_to_fix);

    rtr->distance = res.distance;
    // free(res.distance); // Do not free, it's reusable
    free(res.predecessor);

    return rtr;
}

/**
 * @brief function to describe/pretty print the routing information of a router
 * @warning This function creates a file with the routing information as well
 * 
 * @param rtr pointer to the router structure to be described
 */
void describe_router(struct router *rtr)
{

    char file[128] = "";

    sprintf(file, "./%s%i.txt", "AS", rtr->as_number);

    FILE *fp;
    fp = fopen(file,"w");

    fprintf(fp, "Autonomous System %i - %s\n", rtr->as_number, rtr->name);

    for (int i = 0; i < rtr->tracked_nodes; i++)
    {
        if (i == rtr->next_hop[i] && rtr->as_map[i] != rtr->as_number)
        {
            fprintf(fp, "UTILIZED PEER %i DIST %i\n", rtr->as_map[i], rtr->distance[i]);
        }
    }

    fprintf(fp, "ROUTING\n");

    for (int i = 0; i < rtr->tracked_nodes; i++)
    {
        if (rtr->as_map[i] == rtr->as_number) continue;
        fprintf(fp, " AS %i VIA %i DIST %i\n", rtr->as_map[i], rtr->as_map[rtr->next_hop[i]], rtr->distance[i]);
    }

    fclose(fp);
}

/**
 * @brief function to free the memory allocated for a router structure
 * 
 * @param rtr pointer to the router structure to be freed
 */
void free_router(struct router *rtr)
{
    free(rtr->name);
    free(rtr->as_map);
    free(rtr->next_hop);
    free(rtr->distance);
    free(rtr);
}

#endif