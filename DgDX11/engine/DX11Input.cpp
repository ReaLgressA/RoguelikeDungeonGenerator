#include "DX11Input.h"

DX11Input::DX11Input() { }

DX11Input::DX11Input(const DX11Input& other) { }

DX11Input::~DX11Input() { }

void DX11Input::Initialize() {
    for (int i = 0; i < 256; ++i) {
        m_keys[i] = false;
    }
    return;
}

void DX11Input::KeyDown(unsigned int input) {
    // If a key is pressed then save that state in the key array.
    m_keys[input] = true;
    return;
}


void DX11Input::KeyUp(unsigned int input) {
    // If a key is released then clear that state in the key array.
    m_keys[input] = false;
    return;
}

bool DX11Input::IsKeyDown(unsigned int key) {
    // Return what state the key is in (pressed/not pressed).
    return m_keys[key];
}