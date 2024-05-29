#ifndef MULTIGRAPHBOD_HPP
#define MULTIGRAPHBOD_HPP

#undef min
#undef max

#include <vector>
#include <queue>
#include <set>
#include <chrono>
#include <sstream>
#include <functional> // For std::function
#include <climits>
#include <unordered_map>
#include <cstdint> 




struct Label {
    int64_t id;
    int node;
    bool isLocal; // Inherits the local attribute from the edge used for extension 
    std::vector<int> g; // Actual costs up to this node
    int parentID;

    // Default constructor
    Label() : id(-1), node(-1), parentID(-1) {}

    Label(int64_t id, int node, bool isLocal, const std::vector<int>& g,  int parentID)
        :id(id), node(node), isLocal(isLocal), g(g), parentID(parentID) {}

  
    // Equality check (or similar logic based on your requirements)
    bool isEqual(const Label& other) const {
        return node == other.node && g == other.g; // Simple example: checks node and costs equality
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Label { ID: " << id << ", Node: " << node << ", g: [";
        for (size_t i = 0; i < g.size(); ++i) {
            ss << g[i] << (i < g.size() - 1 ? ", " : "");
        }
        ss << "], ParentID: " << parentID << " }";
        return ss.str();
    }
    
};


struct LabelCompare {
    bool operator()(const Label& lhs, const Label& rhs) const {
        // Implement your comparison logic here
        if (lhs.g[0] == rhs.g[0]) return lhs.g[1] > rhs.g[1];
        return lhs.g[0] > rhs.g[0];
    }
};



class MultiGraph {
public:
    struct MultiEdge {
        int to; // Destination node ID
        bool isLocal; // Indicates if the edge connects nodes within the same partition
        std::vector<std::vector<int>> costs; // Vector of costs for each parallel edge

        MultiEdge(int destination, bool isLocal) : to(destination), isLocal(isLocal) {}

        void addEdgeCost(const std::vector<int>& cost) {
            costs.push_back(cost);
        }
    };

private:
    std::unordered_map<int, std::vector<MultiEdge>> adjacencyList;

public:
    void addEdge(int from, int to, bool isLocal, const std::vector<int>& cost) {
        // If there is no MultiEdge to 'to' yet, create one
        if (!adjacencyList[from].empty() && adjacencyList[from].back().to == to) {
            adjacencyList[from].back().addEdgeCost(cost);
        } else {
            MultiEdge newMultiEdge(to, isLocal);
            newMultiEdge.addEdgeCost(cost);
            adjacencyList[from].push_back(newMultiEdge);
        }
    }

    const std::vector<MultiEdge>& getSuccessors(int node) const {
        static const std::vector<MultiEdge> empty;
        auto it = adjacencyList.find(node);
        return it != adjacencyList.end() ? it->second : empty;
    }

};





class MultiGraphBOD {
public:
    MultiGraphBOD(const MultiGraph& graph, unsigned numberOfNodes)
        : graph(graph), numberOfNodes(numberOfNodes), g2Min(numberOfNodes+1, INT_MAX) {} // 1 based index
    
    std::vector<std::vector<std::vector<int>>>  one2AllBOD(int source);
    std::vector<std::vector<std::vector<int>>>  one2AllBODRefined(int source);
    void initializeSearch(int source);
    void processSuccessors(const Label& current);
    void batchProcessSuccessors(const Label& current);
    bool checkDominanted(const Label& current);
    void reportPerformanceMetrics() const;
    int getExpandedNodesCount() const {
        return expandedNodesCount;
    }
    int64_t getGeneratedLabels() const {
        return nextLabelID;
    }
    

private:
    MultiGraph graph;
    std::unordered_map<int64_t, Label> labels; // Maps unique label IDs to Label objects
    
    std::priority_queue<Label, std::vector<Label>, LabelCompare> openList;
    std::vector<int> g2Min;
    unsigned numberOfNodes;
    
    int64_t nextLabelID = 0; // Tracks the next unique ID to be assigned to a new label

    int64_t generateLabelId() {
        return nextLabelID++; // Simply increment to generate a unique ID
    }

    int expandedNodesCount = 0;
    size_t maxOpenListSize = 0;
    std::chrono::steady_clock::time_point startTime;
};


#endif // ROUTINGALGORITHM_HPP
