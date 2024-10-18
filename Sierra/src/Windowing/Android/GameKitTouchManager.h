//
// Created by Nikolay Kanchevski on 12/14/23.
//

#pragma once

#include "../TouchManager.h"

#include "../../Platform/Android/GameKitContext.h"

namespace Sierra
{

    class SIERRA_API GameKitTouchManager final : public TouchManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        GameKitTouchManager() = default;

        /* --- POLLING METHODS --- */
        void RegisterTouchPress(const Touch& touch) override;
        void RegisterTouchMove(TouchID touch, Vector2 position) override;
        void RegisterTouchRelease(TouchID ID) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::span<const Touch> GetTouches() const noexcept override { return touches; }
        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::GameKit; }

        /* --- MOVE SEMANTICS --- */
        GameKitTouchManager(GameKitTouchManager&&) noexcept = delete;
        GameKitTouchManager& operator=(GameKitTouchManager&&) noexcept = delete;

        /* --- COPY SEMANTICS --- */
        GameKitTouchManager(const GameKitTouchManager&) = delete;
        GameKitTouchManager& operator=(const GameKitTouchManager&) = delete;

        /* --- DESTRUCTOR --- */
        ~GameKitTouchManager() noexcept override = default;

    private:
        std::vector<Touch> touches = { };

        friend class GameKitWindow;
        void Update(const android_input_buffer& inputBuffer);

    };

}