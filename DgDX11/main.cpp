#include "engine/DX11System.h"
#include <memory>
#include "DungeonGenerator.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
    auto system = std::unique_ptr<DX11System>(new DX11System());
    bool result = system->Initialize(
        DungeonGenerator::Generate(250, 70.0f, 3, 16, time(0))
    );
    if (result) {
        system->Run();
    }
    system->Shutdown();
    system.release();
    return 0;
}