# Towards Pareto-optimality with Multi-level Bi-objective Routing: A Summary of Results #

## [Requirements](https://github.com/yang-mingzhou/biobjectiveRouting):
```
GCC 11
C99
```
## Example Execution: 
```
cd src

## HBOR for the "test" network with 2 partitions
make run_HBOR MAP=test NPARTITIONS=2

## clean object files
make clean

## To compare the time usage for precomputation: modify the line 620-630 in bhepv.cpp.
```

## Files:
0. [Makefile](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/Makefile): Makefile for the both HBOR and HBOR-B
1. [bhepvWorkflow.sh](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/bhepvWorkflow.sh): Workflow script for HBOR
2. [b3hepvWorkflow.sh](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/b3hepvWorkflow.sh): Workflow script for HBOR-B
3. [bhepv.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/bhepv.cpp): 
4. [bhepvPathRetrieval.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/bhepvPathRetrieval.cpp):  
5. [bhepvPrecomputation.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/bhepvPrecomputation.cpp):  
6. [biobjectiveGraph.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/biobjectiveGraph.cpp):  
7. [biobjectiveGraph.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/biobjectiveGraph.h):  
8. [dfs_class_v2.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/dfs_class_v2.h): DFS specifying only source node
9. [fragmentPathRetrieval.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/fragmentPathRetrieval.cpp):  
10. [hbor.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/hbor.cpp):  
11. [hbor.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/hbor.h):  
12. [hborWithBhepv.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/hborWithBhepv.cpp):  
13. [hborWithBhepv.hpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/hborWithBhepv.hpp):  
14. [json.hpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/json.hpp):  
15. [kahip.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/kahip.cpp):  
16. [m.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/m.cpp): main file to run DFS 
17. [onlineRouting.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/onlineRouting.cpp):  
18. [pairs.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/pairs.cpp): Generating queries; we use 12 as a random seed in the project.
19. [precomputation.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/precomputation.cpp):  

### Test cases:

20. [test.cpp](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/test.cpp):  

### Following files are built upon code repository [BOAstar](https://github.com/jorgebaier/BOAstar/)   after fixing some memory leak issues:

21. [boastar.c](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/boastar.c):  
22. [boastar.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/boastar.h):  
23. [bod.c](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/bod.c):  
24. [bod.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/bod.h):  
25. [bodPathRetrieval.c](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/bodPathRetrieval.c):  
26. [bodPathRetrieval.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/bodPathRetrieval.h):  
27. [pathRetrieval.c](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/pathRetrieval.c):  
28. [pathRetrieval.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/pathRetrieval.h):  
29. [main_boa.c](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/main_boa.c):  
30. [main_bod.c](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/main_bod.c):  
31. [include.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/include.h):  
32. [node_namoa.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/node_namoa.h):  
33. [main_namoadr.c](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/main_namoadr.c):  
34. [namoadr.c](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/namoadr.c):  
35. [namoadr.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/namoadr.h):  
36. [node.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/node.h):  
37. [heap.c](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/heap.c):  
38. [heap.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/heap.h):  
39. [graph.c](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/graph.c):  
40. [graph.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/graph.h):  
41. [graph_data.h](https://github.com/yang-mingzhou/biobjectiveRouting/blob/main/src/graph_data.h):  


## File Folders:

1. Folder [Maps](https://github.com/yang-mingzhou/biobjectiveRouting/tree/main/Maps)
2. Folder [Queries](https://github.com/yang-mingzhou/biobjectiveRouting/tree/main/Queries)
3. Folder [src](https://github.com/yang-mingzhou/biobjectiveRouting/tree/main/src)

## Acknowledgements

This repository builds upon the following papers and code repositories:

### Paper
[Simple and efficient bi-objective search algo-
rithms via fast dominance checks](https://www.sciencedirect.com/science/article/pii/S0004370222001473)  
Carlos Hernández, William Yeoh, Jorge A Baier, Han Zhang, Luis Suazo, Sven
Koenig, and Oren Salzman. 2023. Simple and efficient bi-objective search algo-
rithms via fast dominance checks. Artificial Intelligence 314 (2023), 103807

[Think Locally, Act Globally: Highly
Balanced Graph Partitioning](https://link.springer.com/chapter/10.1007/978-3-642-38527-8_16)  
Peter Sanders and Christian Schulz. 2013. Think Locally, Act Globally: Highly
Balanced Graph Partitioning. In Experimental Algorithms, 12th International
Symposium, SEA 2013, Rome, Italy, June 5-7, 2013. Proceedings, Vol. 7933. Springer,
164–175.

### Code Repository
Repository Name: [BOAstar](https://github.com/jorgebaier/BOAstar/)  
Author(s)/Owner: jorgebaier 
Description: A C implementation of Bi-Objective A* (BOA*), New Approach to Multi-Objective A* with dimensionality reduction (NAMOA*dr), and Bi-Objective Dijkstra (BOD). 

Repository Name: [KaHIP](https://github.com/KaHIP/KaHIP)  
Author(s)/Owner: Karlsruhe High Quality Graph Partitioning 
Description: The graph partitioning framework KaHIP -- Karlsruhe High Quality Partitioning.

## Troubleshooting Log

#### Entry 1
- **Issue Description**: Application throws a 'Segmentation Fault' upon execution.
- **Steps to Reproduce**:
  1. Execute the application with command `./bhepvPathRetrieval.o test 3`.
  2. Observe the segmentation fault.
- **Tried Solutions**: Checked for memory leaks and stack size; Check ulimit settins of the system using 'ulimit -a'.
- **Solution Description**: Increased the stack size on the Linux server by executing `ulimit -s unlimited`. This resolved the stack overflow caused by deep recursion within the application.

