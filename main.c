#include "stdio.h"
#include "graph.h"
#include "bellford.h"
#include "router.h"

int main() {

    struct graph *network = init_graph(5);

    printf("Hello world\n");


    // Each node gets an AS number depending on its node_id
    // Nodes have networks attached to them

    set_edge_bidir(network, 0, 1, 1);
    set_edge_bidir(network, 0, 2, 3);
    set_edge_bidir(network, 4, 3, 3);
    set_edge(network, 1, 2, 4);
    set_edge(network, 3, 2, 2);
    set_edge(network, 2, 3, 1);

    struct bellman_results res = bellman_ford(network, 1);


    for (int i = 0; i < res.size; i++)
    {
        printf("Node %i distance %i previous hop %i\n", i, res.distance[i], res.predecessor[i]);
    }

    int as_map[] = {2137, 666, 42, 55, 11};

    struct router * rtr = generate_routing_info(666, network, as_map, "Ciechocinek");

    describe_router(rtr);

    free_router(rtr);

    free(res.distance);
    free(res.predecessor);

    free_graph(network);

    return 0;
}
