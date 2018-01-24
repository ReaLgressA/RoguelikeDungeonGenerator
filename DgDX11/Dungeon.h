#ifndef _DUNGEON_H_
#define _DUNGEON_H_
#pragma once
#include "DungeonRoom.h"
#include <vector>
#include "engine\LineRenderer.h"
class Dungeon {
public:
    class DungeonGraph {

    public:
        DungeonGraph() { renderer = nullptr; }
        ~DungeonGraph() {
            if (renderer) {
                delete renderer;
                renderer = nullptr;
            }
        }
        void SetGraphData(const std::vector<XMFLOAT3>& vertices, const  std::vector<LineRenderer::LineRenderer::Line>& edges) {
            if (renderer) {
                delete renderer;
                renderer = nullptr;
            }
            renderer = new LineRenderer(vertices, edges);
        }

        const std::vector<XMFLOAT3>& GetVertices() { return gVertices; };
        const std::vector<LineRenderer::LineRenderer::Line>& GetEdges() { return gEdges; };
        LineRenderer* GetRenderer() { return renderer; }

    private:
        std::vector<XMFLOAT3> gVertices;
        std::vector<LineRenderer::LineRenderer::Line> gEdges;
        LineRenderer *renderer;
    };

    Dungeon();
    ~Dungeon();
    std::vector<DungeonRoom>* GetRooms();
    DungeonGraph& GetGraph() { return graph; };
    void SetRooms(std::vector<DungeonRoom> *rooms);
    std::vector<DungeonRoom>* GetPassages();
    void SetPassages(std::vector<DungeonRoom> *passages);

private:
    std::vector<DungeonRoom> *rooms;
    std::vector<DungeonRoom> *passages;
    DungeonGraph graph;
};

#endif