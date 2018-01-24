#ifndef _COLORSHADER_H_
#define _COLORSHADER_H_
#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace std;
using namespace DirectX;

class ColorShader {
    struct MatrixBufferType {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
        XMFLOAT4 mainColor;
    };

public:
    ColorShader();
    ColorShader(const ColorShader&);
    ~ColorShader();
    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4 mainColor);

private:
    bool InitializeShader(ID3D11Device*, HWND, const wchar_t*, const wchar_t*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4 mainColor);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader * m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11Buffer* m_matrixBuffer;
};
#endif