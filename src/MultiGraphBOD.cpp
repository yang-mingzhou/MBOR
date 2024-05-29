#include "MultiGraphBOD.hpp"
#include <set>
#include <vector>
#include <limits>
#include <algorithm>
#include <iostream>



std::vector<std::vector<std::vector<int>>>  MultiGraphBOD::one2AllBOD(int source) {
    initializeSearch(source);
    
    // 1 based index
    std::vector<std::vector<std::vector<int>>> allSolutions(numberOfNodes+1);
    
    while (!openList.empty()) {
        
        maxOpenListSize = std::max(maxOpenListSize, openList.size());
        
        Label current = openList.top();
        openList.pop();

        if (checkDominanted(current)) {
            continue;
        }
        
        g2Min[current.node] = current.g[1];
        allSolutions[current.node].push_back(current.g);
        expandedNodesCount++;
        
        
        processSuccessors(current);
    }

//     reportPerformanceMetrics(); // Report performance metrics at the end
    // the path can also be reconstructed based on the labels
    return allSolutions;
}

std::vector<std::vector<std::vector<int>>>  MultiGraphBOD::one2AllBODRefined(int source) {
    initializeSearch(source);
    
    // 1 based index
    std::vector<std::vector<std::vector<int>>> allSolutions(numberOfNodes+1);
    
    int cntLog = 0;
    
    while (!openList.empty()) {
        
        maxOpenListSize = std::max(maxOpenListSize, openList.size());
        
        Label current = openList.top();
        openList.pop();
        


        if (checkDominanted(current)) {
            continue;
        }
        
        
        g2Min[current.node] = current.g[1];
        allSolutions[current.node].push_back(current.g);
        expandedNodesCount++;
        
        
        // refined version
        batchProcessSuccessors(current);
    }
    
//     reportPerformanceMetrics(); // Report performance metrics at the end
    // the path can also be reconstructed based on the labels
    return allSolutions;
}



void MultiGraphBOD::initializeSearch(int source) {
    // Clear the open list at the beginning of each run.
    std::priority_queue<Label, std::vector<Label>, LabelCompare>().swap(openList);
    
    expandedNodesCount = 0;
    maxOpenListSize = 0;
    
    // Initialize with the start node label
    std::vector<int> startCosts(2, 0); // Assuming two objectives, initialize with zero costs
    Label startLabel(generateLabelId(), source, false, startCosts, -1);
    openList.push(startLabel); 
    
    std::fill(g2Min.begin(), g2Min.end(), INT_MAX);
    
    labels.insert({startLabel.id, startLabel});
    
    startTime = std::chrono::steady_clock::now(); // Start timing
    

}

bool MultiGraphBOD::checkDominanted(const Label& current) {
    // Check for dominance  
    return current.g[1] >= g2Min[current.node];
    
}


void MultiGraphBOD::processSuccessors(const Label& current) {
    
    for (const auto& multiEdge : graph.getSuccessors(current.node)) {
        
        for (const auto& edgeCost : multiEdge.costs) {
            std::vector<int> newCosts = current.g;

            for (size_t i = 0; i < edgeCost.size(); ++i) {
                newCosts[i] += edgeCost[i];
            }
            



            int64_t newLabelID = generateLabelId();

            Label newLabel(newLabelID, multiEdge.to, false, newCosts, current.id);

            if (checkDominanted(newLabel)) {
                continue;
            }

            openList.push(newLabel);
            labels.insert({newLabelID, newLabel}); // Store the new labels
        }
    }
    
}

void MultiGraphBOD::batchProcessSuccessors(const Label& current){
    // Process successors...  
    for (const auto& multiEdge : graph.getSuccessors(current.node)) {
        // no continuous local travel
        if (current.isLocal && multiEdge.isLocal){
            continue;
        } 
        

        for (const auto& edgeCost : multiEdge.costs) {
            // The edgecost encoded from fragment path view result is in lexicographical order of (g2, g1).
            std::vector<int> newCosts = current.g;
            for (size_t i = 0; i < edgeCost.size(); ++i) {
                newCosts[i] += edgeCost[i];
            }
            
            int64_t newLabelID = generateLabelId();

            Label newLabel(newLabelID, multiEdge.to, multiEdge.isLocal, newCosts, current.id);
            
            // skip all the other edges after this one since they have a larger g2 > g2_current > g2_min
            if (checkDominanted(newLabel)) {
                break;
            }

            openList.push(newLabel);
            labels.insert({newLabelID, newLabel}); // Store the new labels
        }
    }
}






void MultiGraphBOD::reportPerformanceMetrics() const {
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;
    std::cout << "Elapsed Time of Boundary Graph Encoding: " << elapsedSeconds.count() << "s\n";
    std::cout << "Generated Solutions: " << nextLabelID << "\n";
    std::cout << "Expanded Nodes: " << expandedNodesCount << "\n";
    std::cout << "Max Open List Size: " << maxOpenListSize << "\n";
}





