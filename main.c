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

    int router_count;
    int *as_map = NULL;
    int *names_length = NULL;
    char **names = NULL;

    struct graph *netgraph = NULL;
    

    if (rank == 0)
    {
        struct parsing_output *temp = data_from_file();

        router_count = temp->node_amount;
        as_map = temp->as_map;
        names = temp->names;

        names_length = malloc(sizeof(int) * router_count);

        for (int i = 0; i < router_count; i++)
        {
            names_length[i] = strlen(names[i]);
        }

        netgraph = temp->netgraph;
    }

    // Workery muszą wiedzieć ile jest danych do alokacji
    // addr, count, size, node_id from, comms
    MPI_Bcast(&router_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Tylko zwykłe workery, nie mają gotowych danych
    if (as_map == NULL)
    {
        as_map = malloc(sizeof(int) * router_count);
        names_length = malloc(sizeof(int) * router_count);
        names = malloc(sizeof(char*) * router_count);
    }

    // Prześlij informacje o rozmiarach nazw oraz mapowanie AS
    MPI_Bcast(as_map, router_count, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(names_length, router_count, MPI_INT, 0, MPI_COMM_WORLD);

    // Przygotowanie miejsca w workerach na nazwy
    if (rank != 0)
    {
        for (int i = 0; i < router_count; i++)
        {
            names[i] = malloc(sizeof(char) * names_length[i] + 1);
        }
    }

    // Prześlij nazwy
    for (int i = 0; i < router_count; i++)
    {
        MPI_Bcast(names[i], names_length[i] + 1, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
    
    // Prześlij graf

    if (netgraph == NULL)
    {
        netgraph = init_graph(router_count);
    }

    MPI_Bcast(netgraph->costs, router_count * router_count, MPI_INT, 0, MPI_COMM_WORLD);
    netgraph->nodes = router_count;

    // Each process computes routing information for its assigned nodes
    for (int i = rank; i < G.node_amount; i += size)
    {
        struct router * rtr = generate_routing_info(as_map[i], &netgraph, as_map, names[i]);
        describe_router(rtr);
        free_router(rtr);
    }

    free_graph(netgraph);

    MPI_Finalize();
    return 0;
}
