#ifndef _CAMERA_H_
#define _CAMERA_H_
#pragma once
#include <directxmath.h>
using namespace DirectX;

class Camera {
public:
    Camera();
    Camera(const Camera&);
    ~Camera();
    void SetPosition(float, float, float);
    void SetPosition(XMFLOAT3);
    void SetRotation(float, float, float);

    XMFLOAT3 GetPosition();
    XMFLOAT3 GetRotation();

    void Render();
    void GetViewMatrix(XMMATRIX&);
private:
    float m_positionX, m_positionY, m_positionZ;
    float m_rotationX, m_rotationY, m_rotationZ;
    XMMATRIX m_viewMatrix;
};
#endif