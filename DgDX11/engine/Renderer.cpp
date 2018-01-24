#include "Renderer.h"

Renderer::Renderer() {
    buffersInitialized = false;
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer::BaseCopyConstructor(const Renderer& other) {
    buffersInitialized = false;
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_vertexCount = other.m_vertexCount;
    m_indexCount = other.m_indexCount;
    position = other.position;
    color = other.color;
}

bool Renderer::Initialize(ID3D11Device * device) {
    // Initialize the vertex and index buffer that hold the geometry of an object.
    bool result = InitializeBuffers(device);
    if (!result) {
        buffersInitialized = false;
        return false;
    }
    buffersInitialized = true;
    return true;
}

bool Renderer::IsInitialized() {
    return m_vertexBuffer != nullptr && m_indexBuffer != nullptr && buffersInitialized;
}

void Renderer::Shutdown() {
    ShutdownBuffers();
    return;
}

void Renderer::Render(ID3D11DeviceContext * context) {
    RenderBuffers(context);
    return;
}

int Renderer::GetIndexCount() {
    return m_indexCount;
}

void Renderer::SetPosition(XMFLOAT3 position) {
    this->position = position;
}

XMFLOAT3 Renderer::GetPosition() {
    return position;
}

void Renderer::SetColor(XMFLOAT4 color) {
    this->color = color;
}

XMFLOAT4 Renderer::GetColor() {
    return color;
}

bool Renderer::IsVisible() {
    return isVisible;
}

void Renderer::SetVisible(bool visible) {
    this->isVisible = visible;
}
