#ifndef _LINERENDERER_H_
#define _LINERENDERER_H_
#pragma once
#include "Renderer.h"
#include <vector>
using namespace DirectX;

class LineRenderer : public Renderer {

public:
    struct Line {
        int idxA;
        int idxB;

        Line() : idxA(0), idxB(0) { }
        Line(const Line &other) : idxA(other.idxA), idxB(other.idxB) { }
        Line(int idxA, int idxB) : idxA(idxA), idxB(idxB) { }
    };

    LineRenderer(const std::vector<XMFLOAT3>& vertices, const std::vector<Line>& lines);
    LineRenderer(const LineRenderer&);
    ~LineRenderer();

private:
    bool InitializeBuffers(ID3D11Device*) override;
    void ShutdownBuffers() override;
    void RenderBuffers(ID3D11DeviceContext*) override;

private:
    std::vector<XMFLOAT3> _vertices;
    std::vector<Line> _lines;
};
#endif

