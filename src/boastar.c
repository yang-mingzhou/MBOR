/////////////////////////////////////////////////////////////////////
// Carlos Hernandez
// All rights reserved
/////////////////////////////////////////////////////////////////////

#include "heap.h"
#include "node.h"
#include "include.h"
#include "boastar.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

gnode* graph_node;
unsigned num_gnodes;
// unsigned adjacent_table[MAXNODES][MAXNEIGH];
// unsigned pred_adjacent_table[MAXNODES][MAXNEIGH];
unsigned goal, start;
gnode* start_state;
gnode* goal_state;
snode* start_node;

unsigned long long int stat_expansions = 0;
unsigned long long int stat_generated = 0;
unsigned long long int minf_solution = LARGE;
unsigned long long int stat_created = 0;

unsigned solutions[MAX_SOLUTIONS][2];
unsigned nsolutions = 0;
unsigned stat_pruned = 0;


unsigned **adjacent_table;
unsigned **pred_adjacent_table;


void allocateMemoryForTable(unsigned num_gnodes, unsigned num_arcs) {
    double ratio = (double)num_arcs / num_gnodes ;
    unsigned numNeighbors = (unsigned) round(ratio);
    if (numNeighbors<10){
        numNeighbors = 45;
    }
    else {
        numNeighbors = 2000*numNeighbors;
    }

    adjacent_table = malloc(num_gnodes * sizeof(unsigned *));
    pred_adjacent_table = malloc(num_gnodes * sizeof(unsigned *));
    if(adjacent_table == NULL || pred_adjacent_table == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (unsigned i = 0; i < num_gnodes; ++i) {
        adjacent_table[i] = malloc(numNeighbors * sizeof(unsigned));
        pred_adjacent_table[i] = malloc(numNeighbors * sizeof(unsigned));
        if(adjacent_table[i] == NULL || pred_adjacent_table[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
}

void freeMemoryForTable(unsigned numNodes) {
    for (unsigned i = 0; i < numNodes; ++i) {
        free(adjacent_table[i]);
        free(pred_adjacent_table[i]);
    }
    free(adjacent_table);
    free(pred_adjacent_table);
}

void initialize_parameters() {
    start_state = &graph_node[start];
    goal_state = &graph_node[goal];
    stat_percolations = 0;
    for (int i = 0; i < num_gnodes; ++i){
        graph_node[i].gmin = LARGE;
        graph_node[i].heapindex = 0;
    }
    int i, j;
    for (i = 0; i < nsolutions; i++) {
        for (j = 0; j < 2; j++) {
            solutions[i][j]=0;
        }
    }
    minf_solution = LARGE;
}

int backward_dijkstra(int dim) {
    for (int i = 0; i < num_gnodes; ++i)
        graph_node[i].key = LARGE;
    emptyheap_dij();
    goal_state->key = 0;
    goal_state->heapindex = 0;
    insertheap_dij(goal_state);
    
    
    
    while (topheap_dij() != NULL) {
        gnode* n;
        gnode* pred;
        short d;
        n = popheap_dij();
//         printf("expending node: %d\n", n->id);
        if (dim == 1)
            n->h1 = n->key;
        else
            n->h2 = n->key;
        ++stat_expansions;
        
        int cnt;
//         for (cnt=0; cnt<10;cnt++){
//             printf("pred_adjacent_table: %d\n", pred_adjacent_table[n->id][cnt]);
//         }
        
        for (d = 1; d < pred_adjacent_table[n->id][0] * 3; d += 3) {
//             printf("predNode node: %d\n", pred_adjacent_table[n->id][d]);
//             printf("predNode node id: %d\n",graph_node[pred_adjacent_table[n->id][d]].id);
            pred = &graph_node[pred_adjacent_table[n->id][d]];
            int new_weight = n->key + pred_adjacent_table[n->id][d + dim];
            if (pred->key > new_weight) {
                pred->key = new_weight;
//                 printf("trying to insert predNode node whose heapindex is : %d\n", pred->heapindex);
                insertheap_dij(pred);
            }
        }
    }
    return 1;
}

// snode* new_node() {
//     snode* state = (snode*)malloc(sizeof(snode));
//     state->heapindex = 0;
//     return state;
// }

snode* new_node() {
    snode* state = (snode*)malloc(sizeof(snode));
    if(state == NULL) { // check if malloc succeeded
        printf("Error: Memory allocation failed in new_node().\n");
        exit(EXIT_FAILURE);
    }
    state->heapindex = 0;
    return state;
}

void free_node(snode* node) {
    if(node != NULL) { // always good to check if the pointer is not NULL
        free(node);
        node = NULL; // good practice to set freed pointer to NULL
    }
}

int boastar() {
//     printf("Entering boastar\n");
    snode* recycled_nodes[MAX_RECYCLE];
    int next_recycled = 0;
    nsolutions = 0;
    stat_pruned = 0;

    emptyheap();
//     printf("Heap emptied\n");

    start_node = new_node();
    if (!start_node) {
        printf("Failed to allocate start_node\n");
        return 0;
    }
    ++stat_created;
    start_node->state = start;
    start_node->g1 = 0;
    start_node->g2 = 0;
    start_node->key = 0;
    start_node->searchtree = NULL;
//     printf("Start node initialized: state=%d\n", start_node->state);
    
    
    insertheap(start_node);
//     printf("Start node inserted into heap\n");
    stat_expansions = 0;

    while (topheap() != NULL) {
        snode* n = popheap(); //best node in open
//         printf("pop node\n");
        short d;
        if (n->g2 >= graph_node[n->state].gmin || n->g2 + graph_node[n->state].h2 >= minf_solution) {
            stat_pruned++;
//             printf("Node pruned: state=%d\n", n->state);
//             if (next_recycled < MAX_RECYCLE) {
//                 recycled_nodes[next_recycled++] = n;
//             }
            free_node(n);
            continue;
        }
        
        graph_node[n->state].gmin = n->g2;
//         printf("Updated gmin for node %d: %u\n", n->state, graph_node[n->state].gmin);

        if (n->state == goal) {
//             printf("Goal reached: state=%d, g1=%u, g2=%u\n", n->state, n->g1, n->g2);
            //printf("GOAL [%d,%d] nsolutions:%d expanded:%llu generated:%llu heapsize:%d pruned:%d\n", n->g1, n->g2, nsolutions, stat_expansions, stat_generated, sizeheap(), stat_pruned);
            solutions[nsolutions][0] = n->g1;
            solutions[nsolutions][1] = n->g2;
            nsolutions++;
            if (nsolutions > MAX_SOLUTIONS) {
                printf("Maximum number of solutions reached, increase MAX_SOLUTIONS!\n");
                exit(1);
            }
            if (minf_solution > n->g2)
                minf_solution = n->g2;
            free_node(n);
            continue;
        }

        ++stat_expansions;

        for (d = 1; d < adjacent_table[n->state][0] * 3; d += 3) {
//             printf("Expanding node %d\n", n->state);
            snode* succ;
            double newk1, newk2, newkey;
            unsigned nsucc = adjacent_table[n->state][d];
            unsigned cost1 = adjacent_table[n->state][d + 1];
            unsigned cost2 = adjacent_table[n->state][d + 2];

            unsigned newg1 = n->g1 + cost1;
            unsigned newg2 = n->g2 + cost2;
            unsigned h1 = graph_node[nsucc].h1;
            unsigned h2 = graph_node[nsucc].h2;
            

            if (newg2 >= graph_node[nsucc].gmin || newg2 + h2 >= minf_solution)
                continue;

            newk1 = newg1 + h1;
            newk2 = newg2 + h2;

//             if (next_recycled > 0) { //to reuse pruned nodes in memory
//                 succ = recycled_nodes[--next_recycled];
//             }
//             else {
//                 succ = new_node();
//                 ++stat_created;
//             }
            
            succ = new_node();
            ++stat_created;

            succ->state = nsucc;
            stat_generated++;

            newkey = newk1 * (double)BASE + newk2;
            succ->searchtree = n;
            succ->g1 = newg1;
            succ->g2 = newg2;
            succ->key = newkey;
            insertheap(succ);
        }
        if (next_recycled < MAX_RECYCLE) {
            recycled_nodes[next_recycled++] = n;
        }
    }
    // After loop ends
//     printf("Exiting boastar with nsolutions=%d\n", nsolutions);
    int freeCnt;
    for(freeCnt=0;freeCnt < next_recycled;++freeCnt ){
        free_node(recycled_nodes[freeCnt]);
    }

    return nsolutions > 0;
}

/* ------------------------------------------------------------------------------*/
unsigned (*call_boastar())[2] {
    float runtime;
    struct timeval tstart, tend;
    unsigned long long min_cost;
    unsigned long long min_time;

    initialize_parameters();

    gettimeofday(&tstart, NULL);
    //Dijkstra h1
    if (backward_dijkstra(1))
        min_cost = start_state->h1;
    //Dijkstra h2
    if (backward_dijkstra(2))
        min_time = start_state->h2;

    //BOA*
    boastar();

    gettimeofday(&tend, NULL);
    runtime = 1.0 * (tend.tv_sec - tstart.tv_sec) + 1.0 * (tend.tv_usec - tstart.tv_usec) / 1000000.0;
    //		printf("nsolutions:%d Runtime(ms):%f Generated: %llu statexpanded1:%llu\n", nsolutions, time_astar_first1*1000, stat_generated, stat_expansions);
//     printf("%lld;%lld;%d;%f;%llu;%llu;%llu\n",
//         start_state->id + 1,
//         goal_state->id + 1,
//         nsolutions,
//         runtime * 1000,
//         stat_generated,
//         stat_expansions,
//         stat_created);
    return solutions;
}