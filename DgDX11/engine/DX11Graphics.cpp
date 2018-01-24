#include "DX11Graphics.h"

DX11Graphics::DX11Graphics() {
    m_D3D = nullptr;
    m_Camera = nullptr;
    m_ColorShader = nullptr;
    dungeon = nullptr;
}

DX11Graphics::DX11Graphics(const DX11Graphics &) { }

DX11Graphics::~DX11Graphics() { }

bool DX11Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd, Dungeon *dungeon) {
    bool result;

    this->dungeon = dungeon;

    // Create the Direct3D object.
    m_D3D = new DX11D3D();
    if (!m_D3D) {
        return false;
    }

    // Initialize the Direct3D object.
    result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    // Create the camera object.
    m_Camera = new Camera();
    if (!m_Camera) {
        return false;
    }
    
    m_Camera->SetPosition(25.0f, 50.0f, -350.0f);
    
    auto rooms = dungeon->GetRooms();
    for (int i = 0; i < rooms->size(); ++i) {
        result = rooms->at(i).GetRenderer()->Initialize(m_D3D->GetDevice()); 
        if (!result) {
            return false;
        }
    }
    
    m_ColorShader = new ColorShader;
    if (!m_ColorShader) {
        return false;
    }
    result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
        return false;
    }
    return true;
}

void DX11Graphics::Shutdown() {
    if (m_ColorShader) {
        m_ColorShader->Shutdown();
        delete m_ColorShader;
        m_ColorShader = nullptr;
    }
    if (dungeon) {
        delete dungeon;
        dungeon = nullptr;
    }
    if (m_Camera) {
        delete m_Camera;
        m_Camera = nullptr;
    }
    if (m_D3D) {
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = nullptr;
    }
    return;
}

bool DX11Graphics::Frame() {
    bool result;
    DungeonGenerator::GeneratorMutex.lock();
    LineRenderer* graphRenderer = dungeon->GetGraph().GetRenderer();
    if (graphRenderer && !graphRenderer->IsInitialized()) {
        graphRenderer->Initialize(m_D3D->GetDevice());
        graphRenderer->SetColor(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
        graphRenderer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
    }
    DungeonGenerator::GeneratorMutex.unlock();

    // Render the graphics scene.
    clock_t tClock = clock();
    
    result = Render();
    if (!result) {
        return false;
    }
    
    dTimeSeconds = float(clock() - tClock) / CLOCKS_PER_SEC;
    
    return true;
}

Camera * DX11Graphics::GetCamera() {
    return m_Camera;
}

float DX11Graphics::GetDeltaTimeSeconds() {
    return dTimeSeconds;
}

bool DX11Graphics::Render() {

    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    // Clear the buffers to begin the scene.
    m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the world, view, and projection matrices from the camera and d3d objects.
    
    m_Camera->GetViewMatrix(viewMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);
    m_D3D->GetWorldMatrix(worldMatrix);

    auto rooms = dungeon->GetRooms();
    for (int i = 0; i < rooms->size(); ++i) {
        auto renderer = rooms->at(i).GetRenderer();
        if (!renderer->IsVisible()) {
            continue;
        }
        m_D3D->GetWorldMatrix(worldMatrix);
        worldMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&(renderer->GetPosition())));
        renderer->Render(m_D3D->GetDeviceContext());
        result = m_ColorShader->Render(m_D3D->GetDeviceContext(), renderer->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, renderer->GetColor());
        if (!result) {
            m_D3D->EndScene();
            return false;
        }
    }

    DungeonGenerator::GeneratorMutex.lock();
    LineRenderer* graphRenderer = dungeon->GetGraph().GetRenderer();
    if (graphRenderer && graphRenderer->IsInitialized() && graphRenderer->IsVisible()) {
        DungeonGenerator::GeneratorMutex.unlock();
        m_D3D->GetWorldMatrix(worldMatrix);
        worldMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&(graphRenderer->GetPosition())));
        graphRenderer->Render(m_D3D->GetDeviceContext());
        result = m_ColorShader->Render(m_D3D->GetDeviceContext(), graphRenderer->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, graphRenderer->GetColor());
        if (!result) {
            m_D3D->EndScene();
            return false;
        }
    } else {
        DungeonGenerator::GeneratorMutex.unlock();
    }

    DungeonGenerator::GeneratorMutex.lock();
    auto passages = dungeon->GetPassages();
    if (passages) {
        DungeonGenerator::GeneratorMutex.unlock();
        for (int i = 0; i < passages->size(); ++i) {
            DungeonGenerator::GeneratorMutex.lock();
            auto renderer = passages->at(i).GetRenderer();
            if (renderer == nullptr) {
                continue;
            }
            if (!renderer->IsInitialized()) {
                renderer->Initialize(m_D3D->GetDevice());
                DungeonGenerator::GeneratorMutex.unlock();
                continue;
            }
            DungeonGenerator::GeneratorMutex.unlock();
            m_D3D->GetWorldMatrix(worldMatrix);
            worldMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&(renderer->GetPosition())));
            renderer->Render(m_D3D->GetDeviceContext());
            result = m_ColorShader->Render(m_D3D->GetDeviceContext(), renderer->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, renderer->GetColor());
            if (!result) {
                m_D3D->EndScene();
                return false;
            }
        }
    } else {
        DungeonGenerator::GeneratorMutex.unlock();
    }

    // Present the rendered scene to the screen.
    m_D3D->EndScene();
    return true;
}