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



int main()
{

    struct parsing_output * G = NULL;
    G = data_from_file();



    struct router * rtr = generate_routing_info(G->as_map[0], G->netgraph, G->as_map, G->names[0]);

    describe_router(rtr);


    free_graph(G->netgraph);

    for (int i = 0; i < G->node_amount; i++)
    {
        free(G->names[i]);
    }

    free_router(rtr);

    free(G->names);
    free(G->as_map);
    free(G);

    return 0;
}
