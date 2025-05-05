#include "mpi.h"
#include "stdio.h"
#include "graph.h"
#include "bellford.h"
#include "router.h"
#include "stdlib.h"
#include "string.h"
#include "configchain.h"



/*

node 0 parsuje config
dostaje parsing output i na jego podstawie przesyła dalej

node 0 przesyła liczbę routerów (int)

wszystkie node alokują
tablica int o rozmiarze n_routers (as_map)
tablica int o rozmiarze n_routers (rozmiary string)
tablica char* o rozmiarze n_routers
graf o rozmiarze n_routers

node 0 przesyła as_map
w pętli każdy node przypisuje miejsce na stringi do pętli (strlen + 1)
w pętli przesyłane są nazwy do nodeów

przesyłany jest graf

nodey liczą rzeczy i zapisują do plików

free()




*/

#define MAX_NODES 100


int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    struct parsing_output *G = NULL;
    int as_map[MAX_NODES];
    char names[MAX_NODES][50];
    int costs[MAX_NODES][MAX_NODES];
    struct netgraph netgraph;

    if (rank == 0)
    {
        struct parsing_output *temp = data_from_file();
        G.node_amount = temp->node_amount;
        memcpy(as_map, temp->as_map, G.node_amount * sizeof(int));
        for (int i = 0; i < G.node_amount; i++) {
            strncpy(names[i], temp->names[i], 50);
        }
        memcpy(costs, temp->netgraph->costs, G.node_amount * G.node_amount * sizeof(int));
        netgraph.costs = &costs[0][0];
        netgraph.nodes = temp->netgraph->nodes;
    }

    MPI_Bcast(&G.node_amount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(as_map, MAX_NODES, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(names, MAX_NODES * 50, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(costs, MAX_NODES * MAX_NODES, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&netgraph.nodes, 1, MPI_INT, 0, MPI_COMM_WORLD);

    netgraph.costs = &costs[0][0];

    // Each process computes routing information for its assigned nodes
    for (int i = rank; i < G.node_amount; i += size)
    {
        struct router * rtr = generate_routing_info(as_map[i], &netgraph, as_map, names[i]);
        describe_router(rtr);
        free_router(rtr);
    }

    MPI_Finalize();
    return 0;
}
