#ifndef _DX11INPUT_H_
#define _DX11INPUT_H_
#pragma once
class DX11Input {
public:
    DX11Input();
    DX11Input(const DX11Input&);
    ~DX11Input();

    void Initialize();

    void KeyDown(unsigned int);
    void KeyUp(unsigned int);
    bool IsKeyDown(unsigned int);

private:
    bool m_keys[256];
};

#endif