#include "DungeonRoom.h"



DungeonRoom::DungeonRoom(int offsetX, int offsetY, int width, int height, XMFLOAT4 mainColor) {
    this->offsetX = offsetX;
    this->offsetY = offsetY;
    this->width = width;
    this->height = height;
    this->renderer = new CellMeshRenderer(width, height);
    this->renderer->SetPosition(GetPosition());
    this->renderer->SetColor(mainColor);
}

DungeonRoom::DungeonRoom(const DungeonRoom & other) {
    this->offsetX = other.offsetX;
    this->offsetY = other.offsetY;
    this->width = other.width;
    this->height = other.height;
    this->renderer = new CellMeshRenderer(width, height);
    this->renderer->SetPosition(GetPosition());
    if (other.renderer) {
        this->renderer->SetColor(other.renderer->GetColor());
    }
}

DungeonRoom::~DungeonRoom() {
    if (renderer) {
        delete renderer;
        renderer = nullptr;
    }
}

XMFLOAT3 DungeonRoom::GetPosition() {
    return XMFLOAT3(offsetX, offsetY, 0.0f);
}

void DungeonRoom::SetPosition(int offsetX, int offsetY) {
    this->offsetX = offsetX;
    this->offsetY = offsetY;
    this->renderer->SetPosition(GetPosition());
}

int DungeonRoom::Width() const {
    return width;
}

int DungeonRoom::Height() const {
    return height;
}

int DungeonRoom::Left() const {
    return offsetX;
}

int DungeonRoom::Top() const {
    return offsetY + height;
}

int DungeonRoom::Right() const {
    return offsetX + width;
}

int DungeonRoom::Bottom() const {
    return offsetY;
}

bool DungeonRoom::Overlaps(const DungeonRoom &b, int roomPadding) { 
    return !(b.Left() - roomPadding >= Right() ||
             b.Right() <= Left() - roomPadding ||
             b.Top() <= Bottom() - roomPadding ||
             b.Bottom() - roomPadding >= Top());
}

CellMeshRenderer * DungeonRoom::GetRenderer() {
    return renderer;
}

