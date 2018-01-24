#ifndef _CELLMESHRENDERER_H_
#define _CELLMESHRENDERER_H_
#pragma once
#include "Renderer.h"

class CellMeshRenderer : public Renderer {

public:
    CellMeshRenderer(int width, int height);
    CellMeshRenderer(const CellMeshRenderer& other);
    ~CellMeshRenderer();
    
private:
    bool InitializeBuffers(ID3D11Device *device) override;
    void ShutdownBuffers() override;
    void RenderBuffers(ID3D11DeviceContext *context) override;

private:
    const float CELL_SIZE = 1.0f;
    int width;
    int height;
    XMFLOAT4 colorContour; //default color of mesh vertices on the edge
    XMFLOAT4 colorCells; //default color of mesh vertices inside rectangle
};
#endif

