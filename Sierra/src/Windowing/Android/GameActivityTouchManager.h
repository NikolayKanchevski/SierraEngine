//
// Created by Nikolay Kanchevski on 12/14/23.
//

#pragma once

#include "../../TouchManager.h"

#include "GameActivityContext.h"

namespace Sierra
{

    class SIERRA_API GameActivityTouchManager final : public TouchManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        GameActivityTouchManager(const GameActivityContext& gameActivityContext, const TouchManagerCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetTouchCount() const override { return activeTouches.size(); };
        [[nodiscard]] const Touch& GetTouch(uint32 touchIndex) const override;

    private:
        const GameActivityContext& gameActivityContext;
        std::deque<Touch> activeTouches;

        friend class GameActivityWindow;
        void Update();

    };

}