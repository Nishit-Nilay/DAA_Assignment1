#include <iostream>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <iterator>
#include <algorithm>

using namespace std;
using namespace chrono;

inline void fastIO() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
}

void readGraph(const string& filename, vector<unordered_set<int>>& graph, int& numVertices) {
    ifstream file(filename);
    string line;
    unordered_set<int> allNodes;

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        int u, v;
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

    numVertices = allNodes.size();
}

void expand(const vector<unordered_set<int>>& graph, unordered_set<int>& Q, unordered_set<int>& CAND, unordered_set<int>& SUBG,
            int& totalCliques, set<set<int>>& uniqueCliques) {
    if (CAND.empty()) {
        set<int> clique(Q.begin(), Q.end());
        uniqueCliques.insert(clique);
        totalCliques++;
    } else {
        int pivot = *max_element(CAND.begin(), CAND.end(), [&graph](int a, int b) {
            return graph[a].size() < graph[b].size();
        });

        unordered_set<int> candidatesCopy = CAND;
        for (auto it = candidatesCopy.begin(); it != candidatesCopy.end();) {
            int v = *it;

            if (graph[pivot].find(v) != graph[pivot].end()) {
                it = candidatesCopy.erase(it);
                continue;
            }

            Q.insert(v);

            unordered_set<int> newCandidates;
            for (int u : CAND) {
                if (graph[v].find(u) != graph[v].end()) {
                    newCandidates.insert(u);
                }
            }

            expand(graph, Q, newCandidates, SUBG, totalCliques, uniqueCliques);

            Q.erase(v);
            it = candidatesCopy.erase(it);
        }
    }
}

void cliques(const vector<unordered_set<int>>& graph, int numVertices) {
    unordered_set<int> Q;
    unordered_set<int> SUBG;
    unordered_set<int> CAND;

    for (int i = 0; i < (int)graph.size(); ++i) {
        if (!graph[i].empty()) {
            SUBG.insert(i);
            CAND.insert(i);
        }
    }

    int totalCliques = 0;
    set<set<int>> uniqueCliques;

    auto start = high_resolution_clock::now();
    expand(graph, Q, CAND, SUBG, totalCliques, uniqueCliques);
    auto end = high_resolution_clock::now();
    duration<double> duration = end - start;

    cout << "Execution Time: " << duration.count() << " seconds" << endl;

    int largestCliqueSize = 0;
    for (const auto& clique : uniqueCliques) {
        largestCliqueSize = max(largestCliqueSize, (int)clique.size());
    }

    cout << "Largest Clique Size: " << largestCliqueSize << endl;
    cout << "Total Number of Maximal Cliques: " << uniqueCliques.size() << endl;

    map<int, int> sizeDistribution;
    for (const auto& clique : uniqueCliques) {
        sizeDistribution[clique.size()]++;
    }

    cout << "Distribution of Maximal Clique Sizes:" << endl;
    for (const auto& [size, count] : sizeDistribution) {
        cout << "Size " << size << ": " << count << " cliques" << endl;
    }
}

int main() {
    fastIO();
    vector<unordered_set<int>> graph;
    int numVertices;
    string filename = "test6.txt";

    readGraph(filename, graph, numVertices);
    cliques(graph, numVertices);

    return 0;
}