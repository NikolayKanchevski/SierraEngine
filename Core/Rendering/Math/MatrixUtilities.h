//
// Created by Nikolay Kanchevski on 31.12.22.
//

#pragma once

namespace Sierra::Core::Rendering::MatrixUtilities
{

    inline void ToArray(const Matrix4x4 matrix, float* dstArray)
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

    inline Matrix4x4 FromArray(const float* matrixArray)
    {
        Matrix4x4 matrix;

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

        return matrix;
    }

    inline Matrix4x4 CreateTranslation(const Vector3 position)
    {
        return glm::translate(Matrix4x4(1.0f), position);
    }

    inline Matrix4x4 CreateRotation(const float angle, const Vector3 direction)
    {
        return glm::rotate(Matrix4x4(1.0f), glm::radians(angle), direction);
    }

    inline Matrix4x4 CreateScale(const Vector3 scale)
    {
        return glm::scale(Matrix4x4(1.0f), scale);
    }

    inline Matrix4x4 CreateModelMatrix(const Vector3 position, const Vector3 rotation = {0.0f, 0.0f, 0.0f}, const Vector3 scale = {1.0f, 1.0f, 1.0f})
    {
        Matrix4x4 rotationMatrix = glm::toMat4(Quaternion({ glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z) }));

        return CreateTranslation(position) * rotationMatrix * CreateScale(scale);
    }

    inline bool DecomposeModelMatrix(const Matrix4x4 &matrix, Vector3 &translation, Vector3 &rotation, Vector3 &scale)
    {
        using namespace glm;
        using T = float;

        mat4 LocalMatrix(matrix);

        // Normalize the matrix.
        if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
            return false;

        // First, isolate perspective.  This is the messiest.
        if (
                epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
                epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
                epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
        {
            // Clear the perspective partition
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
            LocalMatrix[3][3] = static_cast<T>(1);
        }

        // Next take care of translation (easy).
        translation = vec3(LocalMatrix[3]);
        LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

        vec3 Row[3];

        // Now get scale and shear.
        for (length_t i = 0; i < 3; ++i)
            for (length_t j = 0; j < 3; ++j)
                Row[i][j] = LocalMatrix[i][j];

        // Compute X scale factor and normalize first row.
        scale.x = length(Row[0]);
        Row[0] = detail::scale(Row[0], static_cast<T>(1));
        scale.y = length(Row[1]);
        Row[1] = detail::scale(Row[1], static_cast<T>(1));
        scale.z = length(Row[2]);
        Row[2] = detail::scale(Row[2], static_cast<T>(1));

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

}
