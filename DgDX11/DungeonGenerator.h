#ifndef _DUNGEONGENERATOR_H_
#define _DUNGEONGENERATOR_H_
#define _USE_MATH_DEFINES

#pragma once
#include "Dungeon.h"
#include <math.h>
#include <random>
#include <thread>
#include <unordered_map>
#include "Delaunay.h"
#include "Kruskal.h"
#include <mutex>
#include <memory>

class DungeonGenerator {
public:
    virtual ~DungeonGenerator() = 0;

    static Dungeon* Generate(int numberOfRooms, float radius, int minRoomSize, int maxRoomSize, unsigned int seed);
    static std::mutex GeneratorMutex;

private:
    static int NextRandomRoomSize();
    static void AsyncronousGeneration(Dungeon *dungeon);
    static std::vector<Kruskal::Edge> CreateRoomGraph(Dungeon *dungeon, const std::vector<int> &mainRoomIndices, std::vector<XMFLOAT3> &vertexPositions);
    static void SeparateRooms(std::vector<DungeonRoom> *rooms, int roomPadding);
    static void CreatePassages(Dungeon *dungeon, std::vector<int> mainRoomIndices, const std::vector<XMFLOAT3>& vertices, const std::vector<Kruskal::Edge>& paths);
    static std::vector<int> SelectMainRooms(std::vector<DungeonRoom> *rooms);
    static void RemoveUnusedRooms(Dungeon *dungeon, const std::vector<int>& mainRoomIndices);
private:
    static std::default_random_engine generator;
    static std::normal_distribution<float> random;
    static std::pair<float, float> GetRandomPointInCircle(float);
    static std::uniform_real_distribution<float> uniform;
    static int minRoomSize;
    static int maxRoomSize;
};
#endif