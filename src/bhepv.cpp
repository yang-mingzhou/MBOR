#include "bhepv.hpp"

#include <string>
#include <sstream>
#include <omp.h>

#undef min
#undef max


using json = nlohmann::json;
using namespace std;

BHEPV::BHEPV(const std::string& map, int npar) : mapName(map),nPartitions(npar) {
    cout<< "reading" <<endl;
    // read fragment index
    fileFolderName = "../bhepv/"+mapName;
    
}

void BHEPV::addEdge(int start, int end, int cost1, int cost2) {
    adjacencyList[start-1].push_back({end-1, cost1, cost2 });
}



void BHEPV::readOriginGraph(){
    GraphData graphData;
    int num_nodes = 0;
    int num_arcs = 0;
    string filename = "../Maps/"+mapName+"-road-d.txt";
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Read num_nodes and num_arcs
    file >> num_nodes>> num_arcs;
    // Initialize graphData
    bod_initializeGraphData(&graphData, num_nodes, num_arcs);
    
    numVertices = graphData.numOfGnode;
    adjacencyList.resize(numVertices);
    partitions.resize(numVertices);


    // Read data into edgeVectors
    for (int i = 0; i < graphData.numOfArcs; i++) {
        for (int j = 0; j < 4; j++) {
            file >> graphData.edgeVectors[i][j];
        }
        addEdge(graphData.edgeVectors[i][0], graphData.edgeVectors[i][1], graphData.edgeVectors[i][2], graphData.edgeVectors[i][3]);
    }
    file.close();

    // Read data from file and assign it to graphData
//     readOriginGraphDataFromFile(&graphData);
    graphDataVector.push_back(graphData);
}







// Function to read the partition information from a file
void BHEPV::readPartition() {
    string partitionFile = fileFolderName+"/kaffpaIndex.txt";
    std::ifstream inputFile(partitionFile);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open partition file: " + partitionFile);
    }

    partitions.resize(numVertices);

    for (int i = 0; i < numVertices; ++i) {
        int partition;
        inputFile >> partition;
        partitions[i] = partition;
    }

    inputFile.close();
}


void BHEPV::updateBoundaryNodes(){
    // 0 based
    for (int startVertexIndex = 0; startVertexIndex < numVertices; ++startVertexIndex) {
        for (const auto& edge : adjacencyList[startVertexIndex]) {
            int endVertexIndex = edge.end;
            // Check if the endpoints belong to different fragments
            if (partitions[startVertexIndex] != partitions[endVertexIndex]) {
                // Add the endpoints to the boundary nodes set
                boundaryNodes.insert(startVertexIndex);
                boundaryNodes.insert(endVertexIndex);
            }
        }
    }
}


void BHEPV::encodedPathViewToJson(json& j, const std::unordered_map<int, std::unordered_map<int, std::vector<std::vector<int>>>>& data) {
    for (const auto& pair1 : data) {
        for (const auto& pair2 : pair1.second) {
            j[std::to_string(pair1.first)][std::to_string(pair2.first)] = pair2.second;
        }
    }
}



void BHEPV::printFragmentEncodedPathView() {
    for(const auto& outerPair : fragmentEncodedPathView) {
        std::cout << "Outer Key: " << outerPair.first << std::endl;
        
        for(const auto& innerPair : outerPair.second) {
            std::cout << "\tInner Key: " << innerPair.first << std::endl;
            
            int vectorIndex = 0;
            for(const auto& vec : innerPair.second) {
                std::cout << "\t\tVector " << vectorIndex++ << ": ";
                
                for(const auto& value : vec) {
                    std::cout << value << ' ';
                }
                
                std::cout << std::endl;
            }
        }
    }
}



void BHEPV::saveEncodedPathView(){
    cout<< "starting saveEncodedPathView" << endl;
//     printFragmentEncodedPathView();
    string fileNameFragmentEPV = fileFolderName + "/fragmentEPV.json";
    json j_fepv;
    encodedPathViewToJson(j_fepv, fragmentEncodedPathView);
    std::ofstream o_fepv(fileNameFragmentEPV);
    o_fepv << j_fepv << std::endl;
    
    cout<< "EncodedPathView saved" << endl;
    
    string fileNameBoundaryEPV = fileFolderName + "/boundaryEPV.json";
    json j_bepv;
    encodedPathViewToJson(j_bepv, boundaryEncodedPathView);
    std::ofstream o_bepv(fileNameBoundaryEPV);
    o_bepv << j_bepv << std::endl;
    
    cout<< "boundaryEncodedPathView saved" << endl;

}


void BHEPV::loadEncodedPathView() {
    std::string fileNameFragmentEPV = fileFolderName + "/fragmentEPV.json";
    std::ifstream i_fepv(fileNameFragmentEPV);
    json j_fepv;
    i_fepv >> j_fepv;

    // Clear existing data
    fragmentEncodedPathView.clear();

    // Populate boundaryEncodedPathView from json
    for (json::iterator it = j_fepv.begin(); it != j_fepv.end(); ++it) {
        int outerKey = std::stoi(it.key());
        for (json::iterator inner_it = it.value().begin(); inner_it != it.value().end(); ++inner_it) {
            int innerKey = std::stoi(inner_it.key());
            fragmentEncodedPathView[outerKey][innerKey] = inner_it.value().get<std::vector<std::vector<int>>>();
        }
    }
    
    std::string fileNameBoundaryEPV = fileFolderName + "/boundaryEPV.json";
    std::ifstream i_bepv(fileNameBoundaryEPV);
    json j_bepv;
    i_bepv >> j_bepv;

    // Clear existing data
    boundaryEncodedPathView.clear();

    // Populate boundaryEncodedPathView from json
    for (json::iterator it = j_bepv.begin(); it != j_bepv.end(); ++it) {
        int outerKey = std::stoi(it.key());
        for (json::iterator inner_it = it.value().begin(); inner_it != it.value().end(); ++inner_it) {
            int innerKey = std::stoi(inner_it.key());
            boundaryEncodedPathView[outerKey][innerKey] = inner_it.value().get<std::vector<std::vector<int>>>();
        }
    }
    
    
    
}


void BHEPV::saveBoundaryNodePartition(){
    // Create a JSON object
    json root;
    std::string boundaryNodePartitionFilename = fileFolderName + "/boundaryNodePartition.json";
    // Convert the boundary nodes and psartitions to JSON
    for (int node_index: boundaryNodes) {
        int partition = partitions[node_index];

        // Convert the node and partition to strings
        std::stringstream ssNode;
        ssNode << node_index+1;
        std::string strNode = ssNode.str();

        // Add the node and partition to the JSON object
        root[strNode] = partition;
    }

    // Open the file for writing
    std::ofstream bnpfile(boundaryNodePartitionFilename);

    if (bnpfile.is_open()) {
        // Write the JSON object to the file
        bnpfile << root.dump();  // Add pretty-printing with indentation level 4
        bnpfile.close();
        std::cout << "Boundary nodes saved to " << boundaryNodePartitionFilename << std::endl;
    } else {
        std::cerr << "Failed to open file: " << boundaryNodePartitionFilename << std::endl;
    }
}

void BHEPV::generateAndSaveFragments(){
    // Create a mapping of vertex ID to vertex ID in fragment
    std::unordered_map<int, int> vertexIDInFragment;
    
    std::vector<std::unordered_set<int>> fragmentBoundaryNodes(nPartitions);

    // Iterate over the boundary nodes and assign them to their corresponding fragment
    for (int boundaryNodeIndex : boundaryNodes) {
        fragmentBoundaryNodes[partitions[boundaryNodeIndex]].insert(boundaryNodeIndex);
    }
    std::string boundaryFilename = fileFolderName + "/boundaryNodes.txt";
    std::ofstream boundaryFile(boundaryFilename); 
    
    
    // Save each fragment as a separate file
    for (int fragment = 0; fragment < nPartitions; ++fragment) {
        std::string filename = fileFolderName + "/fragments/fragment" + std::to_string(fragment) + ".txt";
        std::ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
        }
        
        
        const auto& fragBoundaryNodeSet = fragmentBoundaryNodes[fragment];
        for (int boundaryNodeIndex : fragBoundaryNodeSet) {
            boundaryFile << boundaryNodeIndex+1 << " ";  // Save the original node ID
        }
        boundaryFile << std::endl;

        

        // Count the number of vertices and edges in the fragment
        int numFragmentVertices = 0;
        int numFragmentEdges = 0;

        for (int vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex) {
            if (partitions[vertexIndex] == fragment) {
                vertexIDInFragment[vertexIndex] = ++numFragmentVertices;           
                for (const auto& edge : adjacencyList[vertexIndex]) {
                    if (partitions[edge.end] == fragment) {
                        ++numFragmentEdges;
                    }
                }
            }
        }

        // Write the number of vertices and edges in the fragment
        outputFile << numFragmentVertices << " " << numFragmentEdges << std::endl;
        
        GraphData graphData;
        bod_initializeGraphData(&graphData, numFragmentVertices, numFragmentEdges);
        int i = 0;
        // Write the edges in the fragment
        for (int vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex) {
            if (partitions[vertexIndex] == fragment) {
                int startVertexIDInFragment = vertexIDInFragment[vertexIndex];

                for (const auto& edge : adjacencyList[vertexIndex]) {
                    int endVertexIndex = edge.end;

                    if (partitions[endVertexIndex] == fragment) {
                        int endVertexIDInFragment = vertexIDInFragment[endVertexIndex];

                        // Write the edge information: start node, end node, cost1, cost2
                        outputFile << startVertexIDInFragment << " " << endVertexIDInFragment << " " << edge.cost1 << " " << edge.cost2 << std::endl;

                        graphData.edgeVectors[i][0] = startVertexIDInFragment;
                        graphData.edgeVectors[i][1] = endVertexIDInFragment;
                        graphData.edgeVectors[i][2] = edge.cost1;
                        graphData.edgeVectors[i][3] = edge.cost2;
                        ++i;
                    }
                    else {
                        std::vector<std::vector<int>> pairWise_solution_list = {{vertexIndex+1, endVertexIndex+1, edge.cost1, edge.cost2}};
                        boundaryGraph.insert(boundaryGraph.end(), pairWise_solution_list.begin(), pairWise_solution_list.end());
                    }
                }
            }
        }

        graphDataVector.push_back(graphData);
        outputFile.close();
    }

    // Save the fragment index file
    std::string indexFilename = fileFolderName + "/fragmentIndex.txt";
    std::ofstream indexFile(indexFilename);
    

    for (int vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex) {
        int fragmentID = partitions[vertexIndex];
        int idInFragment = vertexIDInFragment.at(vertexIndex);
        indexFile << fragmentID << " " << idInFragment << std::endl;
        fragmentIdToOriginId[fragmentID][idInFragment] = vertexIndex+1;
    }

    indexFile.close();
    
}


void BHEPV::loadBoundaryNodes(){
    // read boundary nodes
    string boundaryNodeFileName =  fileFolderName + "/boundaryNodes.txt";
    ifstream inputFile(boundaryNodeFileName);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + boundaryNodeFileName);
    }
    string line;
    while (getline(inputFile, line)) {
        vector<int> row;
        stringstream ss(line);
        int nodeID;
        while (ss >> nodeID) {
                row.push_back(nodeID);
            
        }
        // cout<<"row size"<< row.size() <<endl;
        boundaryNodeSet.push_back(row);
    }
    inputFile.close();
}




void BHEPV::saveBoundaryGraph(){
    std::string filename = fileFolderName + "/boundaryGraph.txt";
    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
    }
    // Write the number of vertices and edges in the fragment
    outputFile << boundaryNodes.size() << " " << boundaryGraph.size() << std::endl;
    
    
    int boundaryID = 0;
    for (int i=0; i <  boundaryGraph.size(); ++i){
        int snode = boundaryGraph[i][0];
        int dnode = boundaryGraph[i][1];
        if (boundaryIdMap.count(snode) <= 0){
            boundaryIdMap[snode]  = ++boundaryID;
        }
        if (boundaryIdMap.count(dnode) <= 0){
            boundaryIdMap[dnode]  = ++boundaryID;
        }          
        
        outputFile << boundaryIdMap[snode] << " " << boundaryIdMap[dnode] << " " << boundaryGraph[i][2] << " " << boundaryGraph[i][3] << std::endl;
        
    }
    
    outputFile.close();
    
    

}

void BHEPV::bodPathRetrievalForFragmentEncodedPathView(int snode, int fragmentId){
    int snodeInFragment = fragmentIndex[snode-1][1];
    
    const GraphData currentGraph = graphDataVector[fragmentId+1];
    const GraphData* graphDataPtr = &currentGraph;
//     cout<< "snode: " << snode << " , snodeInFragment: " << snodeInFragment << "num of nodes: "<< currentGraph.numOfGnode <<endl;
//     bod_printEdgeVectors(graphDataPtr);
    BodSolutions* solutions_array = bod_paretoPathsInFragment(snodeInFragment, graphDataPtr);  
    
    for (unsigned i = 0; i < currentGraph.numOfGnode; ++i) {
        if (i != snodeInFragment-1){
            int originID = fragmentIdToOriginId[fragmentId][i+1];
            std::vector<std::vector<int>> solution_list;
            std::vector<std::vector<int>> pairWise_solution_list;
            SolutionNode* current = solutions_array[i].head;
            while (current != NULL) {
                std::vector<int> solution({current->solution[0], current->solution[1]});
                pairWise_solution_list.push_back({snode, originID, current->solution[0], current->solution[1]}); // revise
                solution_list.push_back(solution);
                SolutionNode* next = current->next;
                free(current);
                current = next;
            }
            //    we want the fragment EPV are in lexocigraphic order of c1, c2
            std::reverse(solution_list.begin(), solution_list.end());
            fragmentEncodedPathView[snode][originID] = solution_list;
            if (isBoundaryNode(originID-1) && isBoundaryNode(snode-1)){
                boundaryGraph.insert(boundaryGraph.end(), pairWise_solution_list.begin(), pairWise_solution_list.end());
            }   
        }
        else {
            SolutionNode* current = solutions_array[i].head;
            while (current != NULL) {
                SolutionNode* next = current->next;
                free(current);
                current = next;
            }
        }
   
    }
    free(solutions_array);
}

void BHEPV::encodeFragmentPathViewAllPairs(){
    // Enumerating through the map
    for (const auto& outerPair : fragmentIdToOriginId) {
        int fragmentId = outerPair.first;
        const std::unordered_map<int, int>& innerMap = outerPair.second;
        std::cout << "Fragment ID: " << fragmentId << std::endl;
        for (const auto& innerPair : innerMap) {
            int locaID = innerPair.first;
            int globalID = innerPair.second;
            bodPathRetrievalForFragmentEncodedPathView(globalID, fragmentId);
        }
    }
}



void BHEPV::encodeFragmentPathView(){
    for (int i; i< boundaryNodeSet.size(); ++i){
        std::cout<< "Encoding fragment: "<< i+1 << "out of " << nPartitions << endl;

        
        int fragmentId = i;
        for (int snode: boundaryNodeSet[i]){
            bodPathRetrievalForFragmentEncodedPathView(snode, fragmentId);
//             cout<< "snode: " << snode << " finish" <<endl;
        }
    }
}

void BHEPV::parseGraphData(MultiGraph& graph) {
    for (const auto& edgeData : boundaryGraph) {
        // Check if the boundary nodes exist in the map.
        auto itFrom = boundaryIdMap.find(edgeData[0]);
        auto itTo = boundaryIdMap.find(edgeData[1]);
        if (itFrom == boundaryIdMap.end() || itTo == boundaryIdMap.end()) {
            std::cout << "Key not found in map!" << std::endl;
            continue;
        }
        // partitions is 0 index based
        bool isLocal = partitions[itFrom->first - 1] == partitions[itTo->first - 1];
        // Add edge to graph.
        graph.addEdge(itFrom->second, itTo->second, isLocal, {edgeData[2], edgeData[3]});
    }
}


void BHEPV::encodeBoundaryPathView() {
    // Initialize the graph.
    MultiGraph multiGraph;

    // Parse and populate the graph data.
    parseGraphData(multiGraph);
    
    std::ofstream outFile(fileFolderName + "/boundaryIdMap.txt");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
    }

    for (const auto& pair : boundaryIdMap) {
        outFile << pair.first << " " << pair.second << "\n";
    }

    outFile.close();
    
    // Get the number of processors available
    int num_procs = omp_get_num_procs();
    int desired_threads;
    desired_threads = std::min(64, num_procs);

    // Set the number of threads to the number of processors
    omp_set_num_threads(desired_threads);
    

    // Display the number of processors and threads
    std::cout << "Number of processors available: " << num_procs << std::endl;
    std::cout << "Number of threads set: " << omp_get_max_threads() << std::endl;
    
    
    int64_t totalGeneratedLabels = 0;
    int64_t totalExpandedNodes = 0;
    
    #pragma omp parallel for // Enable OpenMP parallel for loop
    for (int i = 0; i < boundaryIdMap.size(); ++i) {
        MultiGraphBOD multiGraphBOD(multiGraph, boundaryNodes.size());
        
        auto it = std::next(boundaryIdMap.begin(), i);
        int bnodeInOriginGraph = it->first;
        int bnodeInBoundaryGraph = it->second;
        

        // refined version
        std::vector<std::vector<std::vector<int>>> solutions = multiGraphBOD.one2AllBODRefined(bnodeInBoundaryGraph);
        // baseline
//         std::vector<std::vector<std::vector<int>>> solutions = multiGraphBOD.one2AllBOD(bnodeInBoundaryGraph);
        
        
        // Aggregate statistics from each run
        int64_t localGeneratedLabels = multiGraphBOD.getGeneratedLabels();
        int64_t localExpandedNodes = multiGraphBOD.getExpandedNodesCount();


        #pragma omp critical // Protects writing to shared structure
        for (const auto& pairDest : boundaryIdMap) {
            int destNodeInOriginGraph = pairDest.first;
            
            if (destNodeInOriginGraph == bnodeInOriginGraph) continue;
            
            int destNodeIndexInBoundaryGraph = pairDest.second;
            
            std::vector<std::vector<int>> solution_list = solutions[destNodeIndexInBoundaryGraph];
            
            boundaryEncodedPathView[bnodeInOriginGraph][destNodeInOriginGraph] = solution_list; 
        }
        // Aggregate statistics
        #pragma omp atomic
        totalGeneratedLabels += localGeneratedLabels;
        
        #pragma omp atomic
        totalExpandedNodes += localExpandedNodes;
    }
    
    // Report aggregated statistics
    std::cout << "Boundary Graph Encoding: Total Generated labels: " << totalGeneratedLabels << "\n";
    std::cout << "Boundary Graph Encoding: Total Expanded Nodes: " << totalExpandedNodes << "\n";
}





void BHEPV::loadFragmentIndex(){
    // read fragment index
    string fragmentIndexFilename =  fileFolderName + "/fragmentIndex.txt";
    ifstream fin(fragmentIndexFilename);
    if (!fin.is_open()) {
        throw std::runtime_error("Failed to open file: " + fragmentIndexFilename);
    }
    int fragmentID, nodeID;
    // For each node: [Fragment ID, the corresponding nodeID in the Fragment]
    array<int, 2> nodeIndex;
    // load fin and store data into vector
    while(fin >> fragmentID >> nodeID ){
        nodeIndex[0] = fragmentID;
        nodeIndex[1] = nodeID;
        fragmentIndex.push_back(nodeIndex);
    }
    fin.close();
}





void BHEPV::PrecomputationAndSave(){
    print_memory_usage("Before computation");
    readOriginGraph();
    print_memory_usage("Read origin graph");
    readPartition();
    print_memory_usage("Read partition");
    updateBoundaryNodes();
    print_memory_usage("Update boundary node");
    // generating fragment graphs
    generateAndSaveFragments();
    print_memory_usage("Save fragments");
    loadFragmentIndex();
    loadBoundaryNodes();
    // FOR EACH fragment, bod for all boundaryNode within fragment => output fragment encoded path view 
    encodeFragmentPathView();
//     encodeFragmentPathViewAllPairs();
    print_memory_usage("Encode fragment epv");
    // Generate boundary multigraph based on fragment encoded path view
    saveBoundaryGraph();
    cout<< "Boundary Graph Saved" << endl;
   // bod for all boundaryNodes in boundary multigraph
    // Measure time after previous steps and before encoding boundary path view
   auto encodeBoundaryStart = std::chrono::steady_clock::now();
   
    encodeBoundaryPathView();
    print_memory_usage("Encode Boundary epv");
    // Measure time after encoding boundary path view
    auto end = std::chrono::steady_clock::now();
    
    cout << "Time used for encodeBoundaryPathView(): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - encodeBoundaryStart).count() << " milliseconds" << endl;
    cout<< "Boundary Path View Encoded" << endl;
    saveEncodedPathView();    
    cout<< "BHEPV Saved" << endl;
    saveBoundaryNodePartition();
    
}




void BHEPV::freeGraphDataVector(){
    for (GraphData currentGraph :graphDataVector){
        bod_cleanupGraphData(&currentGraph);
    }
}













































