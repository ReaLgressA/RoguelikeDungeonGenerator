#include "CellMeshRenderer.h"

CellMeshRenderer::CellMeshRenderer(int width, int height) : Renderer() {
    this->width = width;
    this->height = height;
    colorContour = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
    colorCells = XMFLOAT4(0.45f, 0.45f, 0.45f, 1.0f);
}

CellMeshRenderer::CellMeshRenderer(const CellMeshRenderer &other) {
    BaseCopyConstructor(other);
    width = other.width;
    height = other.height;
    colorContour = other.colorContour;
    colorCells = other.colorCells;
}

CellMeshRenderer::~CellMeshRenderer() {
    Shutdown();
}

bool CellMeshRenderer::InitializeBuffers(ID3D11Device *device) {
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    
    // Create the vertex array.
    int y = height;
    int x = width;
    float sz = CELL_SIZE;
    float hsz = sz / 2.0f;
    // Set the number of vertices in the vertex array.
    m_vertexCount = x*y * 4;
    if (m_vertexCount <= 0) {
        return false;
    }
    vertices = new VertexType[m_vertexCount];
    if (!vertices) {
        return false;
    }
    
    // Set the number of indices in the index array.
    m_indexCount = x*y*8;

    // Create the index array.
    indices = new unsigned long[m_indexCount];
    if (!indices) {
        return false;
    }

    for (int i = 0; i < y; ++i) {
        for (int j = 0; j < x; ++j) {
            int idx = i*x*4 + j*4;
            float sx = j * sz;
            float sy = i * sz;
            vertices[idx].position = XMFLOAT3(-hsz + sx, -hsz + sy, 0.0f); // bottom left
            vertices[idx].color = (i == 0 || j == 0) ? colorContour : colorCells;
            vertices[idx + 1].position = XMFLOAT3(-hsz + sx, hsz + sy, 0.0f); // top left
            vertices[idx + 1].color = (i == y - 1 || j == 0) ? colorContour : colorCells;
            vertices[idx + 2].position = XMFLOAT3(hsz + sx, hsz + sy, 0.0f); // top right
            vertices[idx + 2].color = (i == y - 1 || j == x - 1) ? colorContour : colorCells;
            vertices[idx + 3].position = XMFLOAT3(hsz + sx, -hsz + sy, 0.0f); // bottom right
            vertices[idx + 3].color = (i == 0 || j == x - 1) ? colorContour : colorCells;
            int idxI = i*x * 8 + j * 8;
            indices[idxI++] = idx;
            indices[idxI++] = idx + 1;
            indices[idxI++] = idx + 1;
            indices[idxI++] = idx + 2;
            indices[idxI++] = idx + 2;
            indices[idxI++] = idx + 3;
            indices[idxI++] = idx + 3;
            indices[idxI] = idx;
        }
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
        delete[] vertices;
        delete[] indices;
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

    // Create the index buffer.
    
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result)) {
        delete[] vertices;
        delete[] indices;
        return false;
    }
    
    // Release the arrays now that the vertex and index buffers have been created and loaded.
    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void CellMeshRenderer::RenderBuffers(ID3D11DeviceContext *deviceContext) {
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

void CellMeshRenderer::ShutdownBuffers() {
    if (m_indexBuffer) {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }
    return;
}