#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <map>
#include <iomanip>

using namespace std;

void BronKerboschPivot(set<int> P, set<int> R, set<int> X,
                       const unordered_map<int, set<int>>& graph,
                       vector<set<int>>& maximalCliques,
                       int& largestCliqueSize,
                       map<int, int>& cliqueSizeDistribution) {
    if (P.empty() && X.empty()) {
        maximalCliques.push_back(R);
        largestCliqueSize = max(largestCliqueSize, static_cast<int>(R.size()));
        cliqueSizeDistribution[R.size()]++;
        return;
    }

    int pivot = -1, maxNeighbors = -1;
    set<int> unionPX = P;
    unionPX.insert(X.begin(), X.end());
    for (int u : unionPX) {
        int count = 0;
        for (int neighbor : graph.at(u)) {
            if (P.find(neighbor) != P.end()) count++;
        }
        if (count > maxNeighbors) {
            maxNeighbors = count;
            pivot = u;
        }
    }

    set<int> diffP;
    for (int v : P) {
        if (graph.at(pivot).find(v) == graph.at(pivot).end())
            diffP.insert(v);
    }

    for (int v : diffP) {
        set<int> newP, newR(R), newX;
        newR.insert(v);
        for (int neighbor : graph.at(v)) {
            if (P.find(neighbor) != P.end()) newP.insert(neighbor);
            if (X.find(neighbor) != X.end()) newX.insert(neighbor);
        }

        BronKerboschPivot(newP, newR, newX, graph, maximalCliques, largestCliqueSize, cliqueSizeDistribution);
        P.erase(v);
        X.insert(v);
    }
}

void BronKerboschDegeneracy(const unordered_map<int, set<int>>& graph,
                            vector<set<int>>& maximalCliques,
                            int& largestCliqueSize,
                            map<int, int>& cliqueSizeDistribution) {
    vector<pair<int, int>> vertices;
    for (const auto& [v, neighbors] : graph) {
        vertices.emplace_back(v, neighbors.size());
    }

    sort(vertices.begin(), vertices.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second < b.second;
    });

    for (const auto& [vi, _] : vertices) {
        set<int> P, X;
        for (int neighbor : graph.at(vi)) {
            if (neighbor > vi) P.insert(neighbor);
            else X.insert(neighbor);
        }
        BronKerboschPivot(P, {vi}, X, graph, maximalCliques, largestCliqueSize, cliqueSizeDistribution);
    }
}

unordered_map<int, set<int>> loadGraph(const string& filename) {
    unordered_map<int, set<int>> graph;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        int u, v;
        if (ss >> u >> v) {
            graph[u].insert(v);
            graph[v].insert(u);
        }
    }

    return graph;
}

int main() {
    string filename = "test6.txt";

    unordered_map<int, set<int>> graph = loadGraph(filename);

    vector<set<int>> maximalCliques;
    int largestCliqueSize = 0;
    map<int, int> cliqueSizeDistribution;

    auto startTime = chrono::high_resolution_clock::now();

    BronKerboschDegeneracy(graph, maximalCliques, largestCliqueSize, cliqueSizeDistribution);

    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> executionTime = endTime - startTime;

    cout << "Largest Clique Size: " << largestCliqueSize << endl;
    cout << "Total Number of Maximal Cliques: " << maximalCliques.size() << endl;
    cout << "Execution Time: " << fixed << setprecision(6) << executionTime.count() << " seconds" << endl;

    cout << "Distribution of Maximal Clique Sizes:" << endl;
    for (const auto& [size, count] : cliqueSizeDistribution) {
        cout << "Size " << size << ": " << count << " cliques" << endl;
    }

    return 0;
}