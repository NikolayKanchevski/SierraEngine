//
// Created by Nikolay Kanchevski on 17.11.26.
//

#include "ModalManager.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    Modal& ModalManager::PushModal(std::unique_ptr<Modal>&& modal)
    {
        queue.push(std::move(modal));
        return *queue.back();
    }

    void ModalManager::Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame)
    {
        if (queue.empty())
            return;

        Modal& modal = *queue.front();
        if (!modal.Draw(applicationFrame, drawFrame))
        {
            queue.pop();
        }
    }

}