#include <iostream>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>

using namespace std;
using namespace chrono;

void readGraph(const string& filename, vector<unordered_set<int>>& graph, int& numVertices, vector<int>& vertexDegrees) {
    ifstream file(filename);
    string line;
    set<int> allNodes;

    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        int u, v;
        stringstream ss(line);
        if (!(ss >> u >> v)) continue;

        allNodes.insert(u);
        allNodes.insert(v);

        int maxNode = max(u, v);
        if ((int)graph.size() <= maxNode) {
            graph.resize(maxNode + 1);
        }

        graph[u].insert(v);
        graph[v].insert(u);
    }

    numVertices = graph.size();
    vertexDegrees.resize(numVertices, 0);
    for (int i = 0; i < numVertices; ++i) {
        vertexDegrees[i] = graph[i].size();
    }
}

inline bool isFullyConnected(const set<int>& C, const vector<unordered_set<int>>& graph, int v) {
    for (const int& u : C) {
        if (graph[v].find(u) == graph[v].end()) return false;
    }
    return true;
}

void update(int i, set<int>& C, const vector<unordered_set<int>>& graph,
            vector<int>& S, vector<int>& T, set<set<int>>& maximalCliques,
            const vector<int>& sortedVertices, int n) {
    if (i == n) {
        if (C.size() < 2) return;

        bool isClique = true;
        for (auto u = C.begin(); u != C.end(); ++u) {
            for (auto v = next(u); v != C.end(); ++v) {
                if (graph[*u].find(*v) == graph[*v].end()) {
                    isClique = false;
                    break;
                }
            }
            if (!isClique) break;
        }

        if (!isClique) return;

        for (int v = 0; v < n; ++v) {
            if (C.find(v) == C.end() && isFullyConnected(C, graph, v)) {
                return;
            }
        }

        maximalCliques.insert(C);
    } else {
        int v = sortedVertices[i];
        bool prune = false;
        for (const int& u : C) {
            if (graph[u].find(v) == graph[u].end()) {
                prune = true;
                break;
            }
        }

        if (!prune) {
            C.insert(v);
            update(i + 1, C, graph, S, T, maximalCliques, sortedVertices, n);
            C.erase(v);
        }

        update(i + 1, C, graph, S, T, maximalCliques, sortedVertices, n);
    }
}

void findCliques(const vector<unordered_set<int>>& graph, int numVertices, const vector<int>& vertexDegrees) {
    set<int> C;
    vector<int> S(numVertices, 0), T(numVertices, 0);
    set<set<int>> maximalCliques;

    vector<int> sortedVertices(numVertices);
    iota(sortedVertices.begin(), sortedVertices.end(), 0);
    sort(sortedVertices.begin(), sortedVertices.end(),
         [&vertexDegrees](int a, int b) { return vertexDegrees[a] > vertexDegrees[b]; });

    auto start = high_resolution_clock::now();
    update(0, C, graph, S, T, maximalCliques, sortedVertices, numVertices);
    auto end = high_resolution_clock::now();
    duration<double> duration = end - start;

    cout << scientific << setprecision(6);
    cout << "Execution Time: " << duration.count() << " seconds\n";

    int largestCliqueSize = 0;
    for (const auto& clique : maximalCliques) {
        largestCliqueSize = max(largestCliqueSize, (int)clique.size());
    }

    cout << "Largest Clique Size: " << largestCliqueSize << "\n";
    cout << "Total Number of Maximal Cliques: " << maximalCliques.size() << "\n";

    map<int, int> sizeDistribution;
    for (const auto& clique : maximalCliques) {
        if (clique.size() >= 2) {
            sizeDistribution[clique.size()]++;
        }
    }

    cout << "Distribution of Maximal Clique Sizes:\n";
    for (const auto& [size, count] : sizeDistribution) {
        cout << "Size " << size << ": " << count << " cliques\n";
    }
}

int main() {
    vector<unordered_set<int>> graph;
    int numVertices;
    vector<int> vertexDegrees;

    string filename = "test6.txt";
    readGraph(filename, graph, numVertices, vertexDegrees);
    findCliques(graph, numVertices, vertexDegrees);

    return 0;
}