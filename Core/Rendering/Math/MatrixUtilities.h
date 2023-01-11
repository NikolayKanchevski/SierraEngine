//
// Created by Nikolay Kanchevski on 31.12.22.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Sierra::Core::Rendering::Matrix
{

    inline void ToArray(const glm::mat4x4 matrix, float* dstArray)
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

    inline glm::mat4x4 FromArray(const float* matrixArray)
    {
        glm::mat4x4 matrix;

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

    inline glm::mat4x4 CreateTranslation(const glm::vec3 position)
    {
        return glm::translate(glm::mat4x4(1.0f), position);
    }

    inline glm::mat4x4 CreateRotation(const float angle, const glm::vec3 direction)
    {
        return glm::rotate(glm::mat4x4(1.0f), glm::radians(angle), direction);
    }

    inline glm::mat4x4 CreateScale(const glm::vec3 scale)
    {
        return glm::scale(glm::mat4x4(1.0f), scale);
    }

    inline glm::mat4x4 CreateModel(const glm::vec3 position, const glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, const glm::vec3 scale = { 1.0f, 1.0f, 1.0f })
    {
        glm::mat4x4 rotationMatrix = glm::toMat4(glm::quat({ glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z) }));

        return CreateTranslation(position) * rotationMatrix * CreateScale(scale);
    }

    inline bool DecomposeModelMatrix(const glm::mat4x4 &matrix, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale)
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
