//
// Created by Nikolay Kanchevski on 10.04.23.
//

#include "MatrixUtilities.h"

namespace Sierra::Core::Rendering
{
    void MatrixUtilities::MatrixToArray(const Matrix4x4 &matrix, float *dstArray)
    {
        dstArray[0] = matrix[0][0];
        dstArray[1] = matrix[0][1];
        dstArray[2] = matrix[0][2];
        dstArray[3] = matrix[0][3];

        dstArray[4] = matrix[1][0];
        dstArray[5] = matrix[1][1];
        dstArray[6] = matrix[1][2];
        dstArray[7] = matrix[1][3];

        dstArray[8] = matrix[2][0];
        dstArray[9] = matrix[2][1];
        dstArray[10] = matrix[2][2];
        dstArray[11] = matrix[2][3];

        dstArray[12] = matrix[3][0];
        dstArray[13] = matrix[3][1];
        dstArray[14] = matrix[3][2];
        dstArray[15] = matrix[3][3];
    }

    void MatrixUtilities::ArrayToMatrix(const float *matrixArray, Matrix4x4 &matrix)
    {
        matrix[0][0] = matrixArray[0];
        matrix[0][1] = matrixArray[1];
        matrix[0][2] = matrixArray[2];
        matrix[0][3] = matrixArray[3];

        matrix[1][0] = matrixArray[4];
        matrix[1][1] = matrixArray[5];
        matrix[1][2] = matrixArray[6];
        matrix[1][3] = matrixArray[7];

        matrix[2][0] = matrixArray[8];
        matrix[2][1] = matrixArray[9];
        matrix[2][2] = matrixArray[10];
        matrix[2][3] = matrixArray[11];

        matrix[3][0] = matrixArray[12];
        matrix[3][1] = matrixArray[13];
        matrix[3][2] = matrixArray[14];
        matrix[3][3] = matrixArray[15];
    }

    Matrix4x4 MatrixUtilities::CreateTranslation(const Vector3 position)
    {
        return glm::translate(Matrix4x4(1.0f), position);
    }

    Matrix4x4 MatrixUtilities::CreateRotation(const float angle, const Vector3 direction)
    {
        return glm::rotate(Matrix4x4(1.0f), glm::radians(angle), direction);
    }

    Matrix4x4 MatrixUtilities::CreateScale(Vector3 scale)
    {
        return glm::scale(Matrix4x4(1.0f), scale);
    }

    Matrix4x4 MatrixUtilities::CreateModelMatrix(const Vector3 position, const Vector3 rotation, const Vector3 scale)
    {
        Matrix4x4 rotationMatrix = glm::toMat4(Quaternion({ glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z) }));
        return CreateTranslation(position) * rotationMatrix * CreateScale(scale);
    }

    bool MatrixUtilities::DecomposeModelMatrix(const Matrix4x4 &matrix, Vector3 &translation, Vector3 &rotation, Vector3 &scale)
    {
        Matrix4x4 localMatrix(matrix);

        // Normalize the matrix.
        if (glm::epsilonEqual(localMatrix[3][3], 0.0f, FLOAT_EPSILON))
            return false;

        // First, isolate perspective. This is the messiest.
        if (
            glm::epsilonNotEqual(localMatrix[0][3], 0.0f, FLOAT_EPSILON) ||
            glm::epsilonNotEqual(localMatrix[1][3], 0.0f, FLOAT_EPSILON) ||
            glm::epsilonNotEqual(localMatrix[2][3], 0.0f, FLOAT_EPSILON))
        {
            // Clear the perspective partition
            localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = 0.0f;
            localMatrix[3][3] = 1.0f;
        }

        // Next take care of translation (easy).
        translation = Vector3 (localMatrix[3]);
        localMatrix[3] = Vector4 (0, 0, 0, localMatrix[3].w);

        Vector3 Row[3];

        // Now get scale and shear.
        for (glm::length_t i = 0; i < 3; ++i)
        {
            for (glm::length_t j = 0; j < 3; ++j)
            {
                Row[i][j] = localMatrix[i][j];
            }
        }

        // Compute X scale factor and normalize first row.
        scale.x = length(Row[0]);
        Row[0] = glm::detail::scale(Row[0], 1.0f);
        scale.y = length(Row[1]);
        Row[1] = glm::detail::scale(Row[1], 1.0f);
        scale.z = length(Row[2]);
        Row[2] = glm::detail::scale(Row[2], 1.0f);

        rotation.y = asin(-Row[0][2]);
        if (cos(rotation.y) != 0) {
            rotation.x = atan2(Row[1][2], Row[2][2]);
            rotation.z = atan2(Row[0][1], Row[0][0]);
        }
        else {
            rotation.x = atan2(-Row[2][0], Row[1][1]);
            rotation.z = 0;
        }

        rotation.x = glm::degrees(rotation.x);
        rotation.y = glm::degrees(rotation.y);
        rotation.z = glm::degrees(rotation.z);

        return true;
    }

    Matrix4x4 MatrixUtilities::CreateViewMatrix(const Vector3 eyePosition, const Vector3 eyeDirection, const Vector3 upDirection)
    {
        return glm::lookAtRH(eyePosition, eyePosition + glm::normalize(eyeDirection), upDirection);
    }

    Matrix4x4 MatrixUtilities::CreateOrthographicProjectionMatrix(const float dimensions, const float near, const float far)
    {
        return glm::orthoRH(-dimensions, dimensions, -dimensions, dimensions, near, far);
    }

    Matrix4x4 MatrixUtilities::CreateProjectionMatrix(const float FOV, const float aspect, const float nearClip, const float farClip)
    {
        Matrix4x4 matrix = glm::perspectiveRH(glm::radians(FOV), aspect, nearClip, farClip);
        matrix[1][1] *= -1; // NOTE: Vulkan-only requirement
        return matrix;
    }
}