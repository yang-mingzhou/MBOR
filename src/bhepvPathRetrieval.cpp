#include <cassert>
#include <vector>
#include <list>
#include <array>
#include <iostream>  
#include <chrono> // Include the chrono library
#include <sys/resource.h>
#include "hborWithBhepv.hpp"
using namespace std;


void printQueryTimes(const std::vector<double>& hborQueryTimes, const std::vector<double>& boaQueryTimes) {
    std::cout << "HBOR Query Times (in ms):" << std::endl;
    for (const auto& time : hborQueryTimes) {
        std::cout << time*1000 << " ";
    }
    std::cout << std::endl;

    std::cout << "BOA Query Times (in ms):" << std::endl;
    for (const auto& time : boaQueryTimes) {
        std::cout << time*1000 << " ";
    }
    std::cout << std::endl;
}

void printMemoryUsage()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    std::cout << "Memory usage: " << usage.ru_maxrss << "KB" << std::endl;
}

long getMemoryUsage()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

void processQueries(const std::string& mapName, int nPar) {
    
    int queryCount = -1;
    std::string queryFileName = "../Queries/" + mapName + "-queries";
    std::ifstream queryFile(queryFileName);
    if (!queryFile) {
        std::cerr << "Failed to open query file: " << queryFileName << std::endl;
        return;
    }
    
    
    
    auto startTimeRead = std::chrono::high_resolution_clock::now();
    HBORBasic bhepv = HBORBasic(mapName, nPar);
    long startMemory = getMemoryUsage();
    bhepv.load();
    long endMemory = getMemoryUsage();
    auto endTimeRead = std::chrono::high_resolution_clock::now();
    auto durationRead = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeRead - startTimeRead);
    std::cout<<"Read MEPFV time:" << durationRead.count() << " milliseconds" << std::endl;
    std::cout << "MEPFV memory usage: " << endMemory-startMemory << "KB.\n";
    std::vector<double> hborQueryTimes; // To store the times for hbor method
    std::vector<double> boaQueryTimes; // To store the times for boaPathRetrieval method
    
    std::vector<long> hborMemoryUsages; // To store the memory usages for hbor method
    std::vector<long> boaMemoryUsages; // To store the memory usages for boaPathRetrieval method
    
    std::vector<double> hborRefinedQueryTimes; // To store the times for hborRefined method
    std::vector<long> hborRefinedMemoryUsages; // To store the memory usages for hborRefined method
    double hborRefinedQueryTimeSum = 0.0;

    
    std::string line;
    int queryID = 0;
    
    double hborQueryTimeSum = 0.0, boaQueryTimeSum = 0.0;
    
    
    
    while (std::getline(queryFile, line) && (queryCount <= 0 || queryID < queryCount)) {

        int startNode, endNode;
        std::istringstream iss(line);
        if (!(iss >> startNode >> endNode)) {
            std::cerr << "Invalid query format: " << line << std::endl;
            continue;
        }
        cout<< "sNode: " <<startNode << ", dNode: " << endNode<<endl;
        startMemory = getMemoryUsage();
        // Perform the boaPathRetrieval query and measure the time
        auto startBoa = std::chrono::high_resolution_clock::now();
        int boaNsolutions = bhepv.boa(startNode, endNode); 
        auto endBoa = std::chrono::high_resolution_clock::now();
        endMemory = getMemoryUsage();
        boaMemoryUsages.push_back(endMemory - startMemory);        

        std::chrono::duration<double> boaDuration = endBoa - startBoa;
        double boaQueryTime = boaDuration.count();
        boaQueryTimes.push_back(boaQueryTime);
        
        
        // Perform the hbor query and measure the time
        startMemory = getMemoryUsage();
        auto startHbor = std::chrono::high_resolution_clock::now();
        int hborNsolutions = 0;
        hborNsolutions = bhepv.hbor(startNode, endNode); 
        auto endHbor = std::chrono::high_resolution_clock::now();
        
        endMemory = getMemoryUsage();
        hborMemoryUsages.push_back(endMemory - startMemory);
        
        std::chrono::duration<double> hborDuration = endHbor - startHbor;
        double hborQueryTime = hborDuration.count();
        hborQueryTimes.push_back(hborQueryTime);
        
        // Perform the hborRefined query and measure the time
        startMemory = getMemoryUsage();
        auto startHborRefined = std::chrono::high_resolution_clock::now();
        int hborRefinedNsolutions = bhepv.hborRefined(startNode, endNode); // Assuming hborRefined returns the number of solutions
        auto endHborRefined = std::chrono::high_resolution_clock::now();
        endMemory = getMemoryUsage();
        hborRefinedMemoryUsages.push_back(endMemory - startMemory);

        std::chrono::duration<double> hborRefinedDuration = endHborRefined - startHborRefined;
        double hborRefinedQueryTime = hborRefinedDuration.count();
        hborRefinedQueryTimes.push_back(hborRefinedQueryTime);

        std::cout << "Query (" << startNode << ", " << endNode << ") - MBOR-Basic: " << hborNsolutions << " solutions, BOA: " << boaNsolutions << " solutions." << std::endl;
        std::cout << "Time used - MBOR-Basic: " << hborQueryTime*1000 << " milliseconds, BOA: " << boaQueryTime*1000 << " milliseconds." << std::endl;
        
        std::cout << "Query (" << startNode << ", " << endNode << ") - HBOR-Adv: " << hborRefinedNsolutions << " solutions." << std::endl;
        std::cout << "Time used - HBOR-Adv: " << hborRefinedQueryTime*1000 << " milliseconds." << std::endl;
               
        queryID++;
        
        boaQueryTimeSum += boaQueryTime;
        hborQueryTimeSum += hborQueryTime;
        hborRefinedQueryTimeSum += hborRefinedQueryTime;
    }
    
    bhepv.freeGraphDataVector();
    printQueryTimes(hborQueryTimes, boaQueryTimes);
    // Calculate the average times and print them
    double hborAverageTime = hborQueryTimes.size() > 0 ? hborQueryTimeSum / hborQueryTimes.size() : 0;
    double boaAverageTime = boaQueryTimes.size() > 0 ? boaQueryTimeSum / boaQueryTimes.size() : 0;
    std::cout << "Average MBOR-Basic query time: " << hborAverageTime*1000 << " milliseconds.\n";
    std::cout << "Average BOA query time: " << boaAverageTime*1000 << " milliseconds.\n";
    
    
    // Compute average memory usages
    long hborAverageMemoryUsage = std::accumulate(hborMemoryUsages.begin(), hborMemoryUsages.end(), 0L) / hborMemoryUsages.size();
    long boaAverageMemoryUsage = std::accumulate(boaMemoryUsages.begin(), boaMemoryUsages.end(), 0L) / boaMemoryUsages.size();

    std::cout << "Average MBOR-Basic memory usage: " << hborAverageMemoryUsage << "KB.\n";
    std::cout << "Average BOA memory usage: " << boaAverageMemoryUsage << "KB.\n";
    
    // Add calculations for hborRefined averages
    double hborRefinedAverageTime = hborRefinedQueryTimes.size() > 0 ? hborRefinedQueryTimeSum / hborRefinedQueryTimes.size() : 0;
    long hborRefinedAverageMemoryUsage = std::accumulate(hborRefinedMemoryUsages.begin(), hborRefinedMemoryUsages.end(), 0L) / hborRefinedMemoryUsages.size();

    std::cout << "Average HBOR-Adv query time: " << hborRefinedAverageTime*1000 << " milliseconds.\n";
    std::cout << "Average HBOR-Adv memory usage: " << hborRefinedAverageMemoryUsage << "KB.\n";
    
   
}


int testQueries(const std::string& mapName, int nPar) {
    HBORBasic bhepvR = HBORBasic(mapName, nPar);
    bhepvR.load();
    cout<< bhepvR.hbor(3,5) << endl;
    cout<< "boa: " << bhepvR.boa(3, 5) << endl; 
    cout<< bhepvR.hbor(8,1) << endl;
    cout<< "boa: " << bhepvR.boa(8, 1) << endl; 
    cout<< bhepvR.hbor(1,4) << endl;
    cout<< bhepvR.hbor(4,5) << endl;
    cout<< bhepvR.hbor(3,7) << endl;
    bhepvR.freeGraphDataVector();
    return 0;
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <map_file> <nPartitions>" << std::endl;
        return 1;
    }
    std::string mapName = argv[1];
    int nPartitions = std::stoi(argv[2]);
    processQueries(mapName, nPartitions);
//     testQueries(mapName, nPartitions);
    return 0;
}






