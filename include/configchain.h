/**
 * @file configchain.h
 * @author Jakub Kawka, Marcin Kiżewski
 * @brief routing configuration chain implementation
 * @version 0.1
 * @date 2025-05-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef CONFIG_CHAIN
#define CONFIG_CHAIN

#include "stdlib.h"
#include "stdio.h"

/**
 * @brief structure representing a peer in the routing configuration
 * 
 * @param next pointer to the next peer in the chain
 * @param as_number AS number of the peer
 * @param distance distance to the peer
 */
struct config_peer {
    struct config_peer *next;
    int as_number;
    int distance;
};

/**
 * @brief structure representing a node in the routing configuration
 * 
 * @param next pointer to the next node in the chain
 * @param name name of the node
 * @param as_number AS number of the node
 * @param peer_chain pointer to the chain of peers for this node
 */
struct config_node {
    struct config_node *next;
    char *name;
    int as_number;
    struct config_peer *peer_chain;
};

/**
 * @brief function to create a new config node
 * 
 * @param name name of the node
 * @param as_number AS number of the node
 * @return struct config_node* pointer to the new config node
 */
struct config_node * new_config_node(char * name, int as_number)
{
    struct config_node * new_node = (struct config_node*)malloc(sizeof(struct config_node));
    new_node->name = name;
    new_node->as_number = as_number;
    new_node->peer_chain = NULL;
    new_node->next = NULL;

    return new_node;
}

/**
 * @brief function to add a peer to a config node
 * 
 * @param node pointer to the config node
 * @param as_number AS number of the peer
 * @param distance distance to the peer
 */
void add_peer_to_node(struct config_node * node, int as_number, int distance)
{
    struct config_peer * ptr = node->peer_chain;

    struct config_peer * new_peer = (struct config_peer *)malloc(sizeof(struct config_peer));

    new_peer->next = NULL;
    new_peer->as_number = as_number;
    new_peer->distance = distance;

    if (ptr == NULL)
    {
        node->peer_chain = new_peer;
    } else {
        new_peer->next = node->peer_chain;
        node->peer_chain = new_peer;
    }

    return;
}

/**
 * @brief function to free the memory allocated for a peer chain
 * 
 * @param ptr pointer to the config peer
 */
void free_peer_chain (struct config_peer * ptr)
{
    if (ptr->next != NULL)
    {
        free_peer_chain(ptr->next);
        
    }

    free(ptr);
}

/**
 * @brief function to free the memory allocated for a config node chain
 * 
 * @param ptr pointer to the config node
 */
void free_node_chain (struct config_node * ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    free_node_chain(ptr->next);
    
    if (ptr->peer_chain != NULL)
    {
        free_peer_chain(ptr->peer_chain);
    }
    
    free(ptr->name);
    free(ptr);
}

/**
 * @brief function to describe/pretty print the peer chain
 * 
 * @param ptr pointer to the config peer
 */
void describe_peer_node(struct config_peer * ptr) {
    if (ptr->next)
    {
        describe_peer_node(ptr->next);
    }

    printf("PEER %i %i\n", ptr->as_number, ptr->distance);
}

/**
 * @brief function to describe/pretty print the config node chain
 * 
 * @param ptr pointer to the config node
 */
void describe_config_node(struct config_node * ptr) {

    if (ptr == NULL) return;

    if (ptr->next)
    {
        describe_config_node(ptr->next);
    }

    printf("Node %i %s\n", ptr->as_number, ptr->name);

    if (ptr->peer_chain)
    {
        describe_peer_node(ptr->peer_chain);
    }
}

/**
 * @brief function to read the configuration from a file
 * 
 * @param filename name of the file to be read
 * @return struct config_node* pointer to the config node chain
 */
struct config_node * config_from_file(const char * filename)
{
    // Prepare the data and open the file
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    int read;
    

    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    struct config_node *config_chain = NULL;

    // Walk over all lines in the file
    while ((read = getline(&line, &len, fp)) != -1)
    {
        // Verify type of line
        if (read == 1)
        {
            // Just formatting
        }
        else
        {
            // Do not register the newline
            line[read - 1] = 0;

            char *token = strtok(line, " ");
            while (token != NULL)
            {

                if (strcmp(token, "ROUTER") == 0)
                {
                    

                    token = strtok(NULL, " ");
                    char * new_router_name = (char*)malloc(strlen(token) + 1);
                    strcpy(new_router_name, token);

                    token = strtok(NULL, " ");
                    int new_router_as_number = atoi(token);

                    struct config_node *new_router = new_config_node(new_router_name, new_router_as_number);

                    if (config_chain == NULL) {
                        config_chain = new_router;
                    } else
                    {
                        new_router->next = config_chain;
                        config_chain = new_router;
                    }

                } else if (strcmp(token, "PEER") == 0)
                {
                    token = strtok(NULL, " ");
                    int peer_as_number = atoi(token);

                    token = strtok(NULL, " ");
                    int peer_distance = atoi(token);

                    add_peer_to_node(config_chain, peer_as_number, peer_distance);
                }

                token = strtok(NULL, " ");
            }
            
        }
    }

    // Free last unused string and the file handle
    free(line);
    fclose(fp);

    return config_chain;
}

#endif