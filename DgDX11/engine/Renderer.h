#ifndef _RENDERER_H_
#define _RENDERER_H_
#pragma once
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class Renderer {
public:
    Renderer();
    bool Initialize(ID3D11Device *device);
    bool IsInitialized();
    void Shutdown();
    void Render(ID3D11DeviceContext *context);
    int GetIndexCount();
    void SetPosition(XMFLOAT3);
    XMFLOAT3 GetPosition();
    void SetColor(XMFLOAT4 color);
    XMFLOAT4 GetColor();
    bool IsVisible();
    void SetVisible(bool visible);

protected:
    struct VertexType {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };
    void BaseCopyConstructor(const Renderer&);
    virtual bool InitializeBuffers(ID3D11Device *device) = 0;
    virtual void ShutdownBuffers() = 0;
    virtual void RenderBuffers(ID3D11DeviceContext *context) = 0;

protected:
    ID3D11Buffer *m_vertexBuffer;
    ID3D11Buffer *m_indexBuffer;
    int m_vertexCount;
    int m_indexCount;
    XMFLOAT3 position;
    XMFLOAT4 color; // main color used for shader
    bool buffersInitialized;
    bool isVisible;
};
#endif
