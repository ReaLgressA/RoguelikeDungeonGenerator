#include "LineRenderer.h"

LineRenderer::LineRenderer(const std::vector<XMFLOAT3>& vertices, const std::vector<Line>& lines) : Renderer() {
    _vertices = vertices;
    _lines = lines;
}

LineRenderer::LineRenderer(const LineRenderer &other) {
    BaseCopyConstructor(other);
    this->_vertices = other._vertices;
    this->_lines = other._lines;
}

LineRenderer::~LineRenderer() {
    Shutdown();
}

bool LineRenderer::InitializeBuffers(ID3D11Device *device) {
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    m_vertexCount = _vertices.size();
    vertices = new VertexType[m_vertexCount];
    if (!vertices) {
        return false;
    }
    m_indexCount = _lines.size() * 2;
    indices = new unsigned long[m_indexCount];
    if (!indices) {
        return false;
    }
    
    const XMFLOAT4 colorWhite = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    for (int i = 0; i < _vertices.size(); ++i) {
        vertices[i].position = _vertices[i];
        vertices[i].color = colorWhite;
    }
    for (int i = 0; i < _lines.size(); ++i) {
        indices[i * 2] = _lines[i].idxA;
        indices[i * 2 + 1] = _lines[i].idxB;
    }

    // Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result)) {
        return false;
    }

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result)) {
        return false;
    }
    delete[] vertices;
    vertices = 0;
    delete[] indices;
    indices = 0;

    return true;
}

//Abstract
void LineRenderer::RenderBuffers(ID3D11DeviceContext *deviceContext) {
    unsigned int stride;
    unsigned int offset;
    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;
    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    // Set the type of primitive that should be rendered from this vertex buffer
    deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    return;
}

void LineRenderer::ShutdownBuffers() {
    if (m_indexBuffer) {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }
    return;
}