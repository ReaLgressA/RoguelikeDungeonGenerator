#ifndef _DX11GRAPHICS_H_
#define _DX11GRAPHICS_H_
#pragma once
#include <windows.h>
#include <time.h>
#include <random>
#include "DX11D3D.h"

#include "Camera.h"
#include "ColorShader.h"
#include "CellMeshRenderer.h"
#include "Renderer.h"
#include "../DungeonGenerator.h"

//Globals
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 5000.0f;
const float SCREEN_NEAR = 0.1f;

class DX11Graphics {
public:
    DX11Graphics();
    DX11Graphics(const DX11Graphics&);
    ~DX11Graphics();

    bool Initialize(int, int, HWND, Dungeon*);
    void Shutdown();
    bool Frame();
    Camera* GetCamera();
    float GetDeltaTimeSeconds();

private:
    bool Render();

private:
    DX11D3D * m_D3D;
    Camera* m_Camera;
    Dungeon *dungeon;

    ColorShader* m_ColorShader;
    float dTimeSeconds;
};
#endif