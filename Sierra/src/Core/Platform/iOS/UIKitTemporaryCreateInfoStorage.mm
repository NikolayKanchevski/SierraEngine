//
// Created by Nikolay Kanchevski on 10.11.23.
//

#include "UIKitTemporaryCreateInfoStorage.h"

namespace Sierra
{

    /* --- SETTER METHODS --- */

    void UIKitTemporaryCreateInfoStorage::Push(const UIKitApplicationCreateInfo &createInfo)
    {
        storageQueue.push(createInfo);
    }

    /* --- GETTER METHODS --- */

    UIKitApplicationCreateInfo UIKitTemporaryCreateInfoStorage::MoveFront()
    {
        UIKitApplicationCreateInfo createInfo = std::move(storageQueue.front());
        storageQueue.pop();
        return createInfo;
    }


}