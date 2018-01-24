#ifndef _DUNGEONROOM_H_
#define _CAMERA_H_
#pragma once
#include "engine\CellMeshRenderer.h"


class DungeonRoom {
public:
    DungeonRoom(int x, int y, int w, int h, XMFLOAT4 mainColor);
    DungeonRoom(const DungeonRoom &other);
    ~DungeonRoom();
    XMFLOAT3 GetPosition();
    void SetPosition(int offsetX, int offsetY);
    int Width()const;
    int Height()const;
    int Left()const;
    int Top()const;
    int Right()const;
    int Bottom()const;
    bool Overlaps(const DungeonRoom &room, int roomPadding);
    CellMeshRenderer* GetRenderer();
private:
    int offsetX;
    int offsetY;
    int width;
    int height;
    CellMeshRenderer *renderer;
};
#endif
