//
// Created by Nikolay Kanchevski on 31.12.22.
//

#pragma once

namespace Sierra::Core::Rendering::MatrixUtilities
{

    void MatrixToArray(const Matrix4x4 &matrix, float* dstArray);
    void ArrayToMatrix(const float* matrixArray, Matrix4x4 &matrix);

    Matrix4x4 CreateTranslation(Vector3 position);
    Matrix4x4 CreateRotation(float angle, Vector3 direction);
    Matrix4x4 CreateScale(Vector3 scale);

    Matrix4x4 CreateModelMatrix(Vector3 position, Vector3 rotation = {0.0f, 0.0f, 0.0f}, Vector3 scale = { 1.0f, 1.0f, 1.0f });
    bool DecomposeModelMatrix(const Matrix4x4 &matrix, Vector3 &translation, Vector3 &rotation, Vector3 &scale);

    Matrix4x4 CreateViewMatrix(Vector3 eyePosition, Vector3 eyeDirection, Vector3 upDirection = { 0.0f, 1.0f, 0.0f });
    Matrix4x4 CreateProjectionMatrix(float FOV, float aspect, float nearClip, float farClip);
    Matrix4x4 CreateOrthographicProjectionMatrix(float dimensions, float near, float far);
}
