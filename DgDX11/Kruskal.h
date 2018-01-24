#ifndef _KRUSKAL_H_
#define _KRUSKAL_H_
#pragma once
#include <vector>
#include <utility>
#include <algorithm>

namespace Kruskal {
    struct Edge {
        int first;
        int second;
        float weight;
        Edge() : first(0), second(0), weight(0.0f) { }
        Edge(int first, int second, float weight) : first(first), second(second), weight(weight) { }
        Edge(const Edge& other) : first(other.first), second(other.second), weight(other.weight) { }
    };

    typedef std::vector< std::vector< std::pair<int, float> > > WeightedGraph;

    class Kruskal {
    public:
        static std::vector<Edge> CalculateMinimumSpanningTree(const WeightedGraph& graph) {
            std::vector<Edge> edges;
            std::vector<Edge> mst; //minimum spanning tree
            std::vector<int> sets(graph.size()); //sets of predecessors
            for (int i = 0; i < graph.size(); ++i) {
                for (auto& edge : graph[i])
                    edges.push_back({ i, edge.first, edge.second });
                sets[i] = i;
            }
            auto comp = [&](Edge left, Edge right) { return left.weight > right.weight; };
            std::sort(edges.begin(), edges.end(), comp);
            Edge minEdge;
            while (!edges.empty()) {
                minEdge = edges.back();
                edges.pop_back();
                int headFirst, headSecond;
                headFirst = getPredecessor(minEdge.first, sets);
                headSecond = getPredecessor(minEdge.second, sets);
                if (headFirst != headSecond) { //if nodes is not in the same set - add them to mst and merge
                    mst.push_back(minEdge);
                    if (headFirst < headSecond) {
                        sets[headSecond] = headFirst;
                    } else {
                        sets[headFirst] = headSecond;
                    }
                }
            }
            return mst;
        }

    private:
        static int getPredecessor(int vIdx, const std::vector<int>& sets) {
            while (sets[vIdx] != vIdx) {
                vIdx = sets[vIdx];
            }
            return vIdx;
        }
    };
}
#endif
