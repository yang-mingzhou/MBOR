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

## File Folders:

1. Folder [Maps](https://github.com/yang-mingzhou/MBOR/tree/master/Maps): Maps data
2. Folder [Queries](https://github.com/yang-mingzhou/MBOR/tree/master/Queries): Queries generated for each map
3. Folder [src](https://github.com/yang-mingzhou/MBOR/tree/master/src): Source code

## Files:
0. [Makefile](https://github.com/yang-mingzhou/MBOR/blob/master/src/Makefile): Makefile for MBOR
3. [bhepv.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/bhepv.cpp): 
4. [bhepvPathRetrieval.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/bhepvPathRetrieval.cpp):  
5. [bhepvPrecomputation.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/bhepvPrecomputation.cpp):  
6. [biobjectiveGraph.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/biobjectiveGraph.cpp):  
7. [biobjectiveGraph.h](https://github.com/yang-mingzhou/MBOR/blob/master/src/biobjectiveGraph.h):  
9. [fragmentPathRetrieval.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/fragmentPathRetrieval.cpp):  
10. [hbor.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/hbor.cpp):  
11. [hbor.h](https://github.com/yang-mingzhou/MBOR/blob/master/src/hbor.h):  
12. [hborWithBhepv.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/hborWithBhepv.cpp):  
13. [hborWithBhepv.hpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/hborWithBhepv.hpp):  
15. [kahip.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/kahip.cpp):  
17. [onlineRouting.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/onlineRouting.cpp):  
18. [pairs.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/pairs.cpp): Generating queries; we use 12 as a random seed in the project.
19. [precomputation.cpp](https://github.com/yang-mingzhou/MBOR/blob/master/src/precomputation.cpp):  


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


