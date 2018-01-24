#include "Dungeon.h"



Dungeon::Dungeon() { 
    rooms = nullptr;
    passages = nullptr;
}

Dungeon::~Dungeon() {
    if (rooms) {
        
        delete rooms;
        rooms = nullptr;
    }
    if (passages) {
        delete passages;
        passages = nullptr;
    }
}

std::vector<DungeonRoom>* Dungeon::GetRooms() {
    return rooms;
}

void Dungeon::SetRooms(std::vector<DungeonRoom> *rooms) {
    this->rooms = rooms;
}

std::vector<DungeonRoom>* Dungeon::GetPassages() {
    return passages;
}

void Dungeon::SetPassages(std::vector<DungeonRoom>* passages) {
    this->passages = passages;
}
