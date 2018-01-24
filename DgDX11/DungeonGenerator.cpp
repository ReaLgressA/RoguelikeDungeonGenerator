#include "DungeonGenerator.h"

std::default_random_engine DungeonGenerator::generator = std::default_random_engine(0);
std::normal_distribution<float> DungeonGenerator::random = std::normal_distribution<float>(0, 1);
std::uniform_real_distribution<float> DungeonGenerator::uniform = std::uniform_real_distribution<float>(0, 1);
int DungeonGenerator::minRoomSize = 0;
int DungeonGenerator::maxRoomSize = 0;
std::mutex DungeonGenerator::GeneratorMutex;

Dungeon* DungeonGenerator::Generate(int numberOfRooms, float radius, int minRoomSize, int maxRoomSize, unsigned int seed) {

    DungeonGenerator::minRoomSize = minRoomSize;
    DungeonGenerator::maxRoomSize = maxRoomSize;
    DungeonGenerator::random = std::normal_distribution<float>((minRoomSize + (maxRoomSize - minRoomSize) / 2.0f), ((maxRoomSize - minRoomSize) / 6.0f));;
    generator = std::default_random_engine(seed);
    Dungeon *dungeon = new Dungeon();
    auto rooms = new std::vector<DungeonRoom>();
    //Generate rooms inside a circle of given radius
    for (int i = 0; i < numberOfRooms; ++i) {
        auto pos = GetRandomPointInCircle(radius);
        rooms->push_back(DungeonRoom(pos.first, pos.second, NextRandomRoomSize(), NextRandomRoomSize(), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));
    }       
    dungeon->SetRooms(rooms);
    std::thread generatorThread(&DungeonGenerator::AsyncronousGeneration, dungeon);
    generatorThread.detach();
    return dungeon;
}

int DungeonGenerator::NextRandomRoomSize() {
    int size = std::round(random(generator));
    return size < minRoomSize ? minRoomSize : (size > maxRoomSize ? maxRoomSize : size);
}

void DungeonGenerator::AsyncronousGeneration(Dungeon * dungeon) {
    SeparateRooms(dungeon->GetRooms(), 3);
    std::vector<int> mainRoomIndices = SelectMainRooms(dungeon->GetRooms());
    std::vector<XMFLOAT3> vertexPositions;
    auto msp = CreateRoomGraph(dungeon, mainRoomIndices, vertexPositions);
    CreatePassages(dungeon, mainRoomIndices, vertexPositions, msp);
    RemoveUnusedRooms(dungeon, mainRoomIndices);
    dungeon->GetGraph().GetRenderer()->SetVisible(false);
}

std::vector<Kruskal::Edge> DungeonGenerator::CreateRoomGraph(Dungeon *dungeon, const std::vector<int> &mainRoomIndices, std::vector<XMFLOAT3> &vertexPositions) {
    //triangulate middle points of main rooms to get a graph of main rooms
    Delaunay::Delaunay<int> delaunay;
    std::vector< Delaunay::Vector2<int> > vertices;
    auto rooms = dungeon->GetRooms();
    for (int i : mainRoomIndices) {
        int x = (rooms->at(i).Right() + rooms->at(i).Left()) / 2;
        int y = (rooms->at(i).Top() + rooms->at(i).Bottom()) / 2;
        vertices.push_back(Delaunay::Vector2<int>(x, y));
    }
    auto tris = delaunay.Triangulate(vertices);
    
    std::unordered_map< Delaunay::Vector2<int>, int, Delaunay::Vector2Hasher<int> > vertexMap;
    for (auto v : delaunay.GetVertices()) {
        vertexMap.insert(std::make_pair(v, vertexPositions.size()));
        vertexPositions.push_back(XMFLOAT3(v.x, v.y, 1.0f));
    }

    
    std::vector<LineRenderer::LineRenderer::Line> edges;
    for (auto e : delaunay.GetEdges()) {
        edges.push_back(LineRenderer::Line(vertexMap[e.p1], vertexMap[e.p2]));
    }

    //DungeonGenerator::GeneratorMutex.lock();
    //dungeon->GetGraph().SetGraphData(vertexPositions, edges);
    //DungeonGenerator::GeneratorMutex.unlock();

    //Transform graph to minimal spanning tree
    Kruskal::WeightedGraph graph;
    for (int i = 0; i < vertices.size(); ++i) {
        graph.push_back(std::vector< std::pair<int, float> >());
    }
    for (auto edge : edges) {
        int sqrDistance = vertices[edge.idxA].MagnitudeSqr(vertices[edge.idxB]);
        graph[edge.idxA].push_back(std::make_pair(edge.idxB, sqrDistance));
    }
    std::vector<Kruskal::Edge> msp = Kruskal::Kruskal::CalculateMinimumSpanningTree(graph);

    
    std::vector<LineRenderer::Line> kruskalEdges;
    for (auto e : msp) {
        kruskalEdges.push_back(LineRenderer::Line(e.first, e.second));
    }

    //DungeonGenerator::GeneratorMutex.lock();
    dungeon->GetGraph().SetGraphData(vertexPositions, kruskalEdges);
    //DungeonGenerator::GeneratorMutex.unlock();
    
    return msp;
}

void DungeonGenerator::SeparateRooms(std::vector<DungeonRoom> *rooms, int roomPadding) {
    DungeonRoom *roomA, *roomB;
    int dx, dxa, dxb, dy, dya, dyb; //delta values of the overlap
    
    bool isTouching; // flag to keep track of touching or overlaping rooms
    do {
        isTouching = false;
        for (int i = 0; i < rooms->size(); ++i) {
            roomA = &rooms->at(i);
            for (int j = i + 1; j < rooms->size(); ++j) {
                roomB = &rooms->at(j);
                if (roomA->Overlaps(*roomB, roomPadding)) {
                    isTouching = true;
                    dx = min(roomA->Right(), roomB->Right()) - max(roomA->Left(), roomB->Left()) + roomPadding;
                    dy = min(roomA->Top(), roomB->Top()) - max(roomA->Bottom(), roomB->Bottom()) + roomPadding;
                    if (std::abs(dx) < std::abs(dy)) {
                        dy = 0;
                    } else {
                        dx = 0;
                    }
                    dxa = -dx / 2;
                    dxb = dx + dxa;
                    dya = -dy / 2;
                    dyb = dy + dya;
                    // shift both rooms
                    roomA->SetPosition(roomA->Left() + dxa, roomA->Bottom() + dya);
                    roomB->SetPosition(roomB->Left() + dxb, roomB->Bottom() + dyb);
                }
            }
        }
    } while (isTouching);
}

void DungeonGenerator::CreatePassages(Dungeon *dungeon, std::vector<int> mainRoomIndices, const std::vector<XMFLOAT3>& vertices, const std::vector<Kruskal::Edge>& paths) {
    std::vector<DungeonRoom> *passages = new std::vector<DungeonRoom>();
    const XMFLOAT4 COLOR_PASSAGE = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    const int PASSAGE_WIDTH = 3;

    std::unordered_map<int, int> vertexToRoom;//map each vertex of graph to corresponded main room
    auto rooms = dungeon->GetRooms();
    for (int i = 0; i < vertices.size(); ++i) {
        int x = std::round(vertices[i].x);
        int y = std::round(vertices[i].y);
        for (int idx : mainRoomIndices) {
            if (rooms->at(idx).Left() <= x && rooms->at(idx).Right() >= x && 
                rooms->at(idx).Top() >= y && rooms->at(idx).Bottom() <= y) {
                vertexToRoom.insert(std::make_pair(i, idx));
            }
        }
    }

    for (int i = 0; i < paths.size(); ++i) {
        XMFLOAT3 a = vertices[paths[i].first];
        XMFLOAT3 b = vertices[paths[i].second];
        //Get the references to evade creation of copies of rooms. DO NOT MODIFY
        DungeonRoom &roomA = rooms->at(vertexToRoom[paths[i].first]);
        DungeonRoom &roomB = rooms->at(vertexToRoom[paths[i].second]);
        int midX = std::round((a.x + b.x) / 2.0f);
        int midY = std::round((a.y + b.y) / 2.0f);
        
        if (midX >= roomA.Left() && midX <= roomA.Right() && midX >= roomB.Left() && midX <= roomB.Right()) {
            //Vertical passage
            int x = midX - PASSAGE_WIDTH / 2;
            int y = roomA.Top() <= midY ? (roomA.Top() - PASSAGE_WIDTH / 2) : (roomB.Top() - PASSAGE_WIDTH / 2);
            int w = PASSAGE_WIDTH;
            int h = roomA.Top() <= midY ? (roomB.Bottom() - roomA.Top() + PASSAGE_WIDTH) : (roomA.Bottom() - roomB.Top() + PASSAGE_WIDTH);
            DungeonRoom passage = DungeonRoom(x, y, w, h, COLOR_PASSAGE);
            passages->push_back(passage);
        } else if (midY >= roomA.Bottom() && midY <= roomA.Top() && midY >= roomB.Bottom() && midY <= roomB.Top()) {
            //Horizontal passage
            int x = roomA.Right() <= midX ? (roomA.Right() - PASSAGE_WIDTH / 2) : (roomB.Right() - PASSAGE_WIDTH / 2);
            int y = midY - PASSAGE_WIDTH / 2;
            int w = roomA.Right() <= midX ? (roomB.Left() - roomA.Right() + PASSAGE_WIDTH) : (roomA.Left() - roomB.Right() + PASSAGE_WIDTH);
            int h = PASSAGE_WIDTH;
            DungeonRoom passage = DungeonRoom(x, y, w, h, COLOR_PASSAGE);
            passages->push_back(passage);
        } else {
            int x, y, w, h;
            //Diagonal - create two passages
            bool isRoomALeft = roomA.Right() <= roomB.Left();
            x = (isRoomALeft ? roomA.Right() : roomB.Right()) - PASSAGE_WIDTH / 2;
            y = (isRoomALeft ? (a.y) : (b.y)) - PASSAGE_WIDTH / 2;
            w = isRoomALeft ? (b.x - roomA.Right() + PASSAGE_WIDTH) : (a.x - roomB.Right() + PASSAGE_WIDTH);
            h = PASSAGE_WIDTH;
            passages->push_back(DungeonRoom(x, y, w, h, COLOR_PASSAGE));
            bool isRoomABottom = roomA.Top() <= roomB.Bottom();
            w = PASSAGE_WIDTH;
            h = isRoomABottom ? (roomB.Bottom() - a.y + PASSAGE_WIDTH) : (roomA.Bottom() - b.y + PASSAGE_WIDTH);;
            x = (isRoomALeft ? b.x : a.x) - PASSAGE_WIDTH / 2;
            y = (isRoomALeft ? (a.y) : (b.y)) - PASSAGE_WIDTH / 2;
            if ((isRoomALeft && !isRoomABottom) || (!isRoomALeft && isRoomABottom)) {
                y -= h;
            }
            passages->push_back(DungeonRoom(x, y, w, h, COLOR_PASSAGE));
        }

        DungeonGenerator::GeneratorMutex.lock();
        dungeon->SetPassages(passages);
        DungeonGenerator::GeneratorMutex.unlock();
    }    
}

std::vector<int> DungeonGenerator::SelectMainRooms(std::vector<DungeonRoom>* rooms) {
    std::vector<int> mainRoomIndices;
    int averageArea = pow((maxRoomSize + minRoomSize) / 2 + 1, 2);
    DungeonRoom *room;
    for (int i = 0; i < rooms->size(); ++i) {
        room = &rooms->at(i);
        if (room->Width() * room->Height() >= averageArea) {
            room->GetRenderer()->SetColor(XMFLOAT4(0.25f, 0.4f, 0.9f, 1.0f));
            mainRoomIndices.push_back(i);
        }
    }
    return mainRoomIndices;
}

void DungeonGenerator::RemoveUnusedRooms(Dungeon * dungeon, const std::vector<int>& mainRoomIndices) {
    auto rooms = dungeon->GetRooms();
    auto passages = dungeon->GetPassages();
    std::unique_ptr<bool> isMainRoom(new bool[rooms->size()]{ 0 });
    for (int idx : mainRoomIndices) {
        isMainRoom.get()[idx] = true;
    }
    for (int i = 0; i < rooms->size(); ++i) {
        if (isMainRoom.get()[i]) {
            continue; //main rooms should always be in the final dungeon
        }
        //Check is this room crossed by any passage. If don't - make it invisible
        bool hasPassageCollision = false;
        for (int j = 0; j < passages->size(); ++j) {
            if (rooms->at(i).Overlaps(passages->at(j), 1)) {
                hasPassageCollision = true;
            }
        }
        if (!hasPassageCollision) {
            rooms->at(i).GetRenderer()->SetVisible(false);
        }
    }
}

std::pair<float, float> DungeonGenerator::GetRandomPointInCircle(float radius) {
    //https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
    float t = 2 * M_PI * uniform(generator);
    float u = uniform(generator) + uniform(generator);
    float r = u > 1 ? (2 - u) : u;
    return std::make_pair(roundf(radius * r * cos(t)), roundf(radius * r * sin(t)));
}
