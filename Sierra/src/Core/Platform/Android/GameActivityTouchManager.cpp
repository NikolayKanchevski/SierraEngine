//
// Created by Nikolay Kanchevski on 12/14/23.
//

#include "GameActivityTouchManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    GameActivityTouchManager::GameActivityTouchManager(const GameActivityContext &gameActivityContext, const TouchManagerCreateInfo &createInfo)
        : TouchManager(createInfo), gameActivityContext(gameActivityContext)
    {

    }

    /* --- GETTER METHODS --- */

    const Touch& GameActivityTouchManager::GetTouch(const uint32 touchIndex) const
    {
        if (touchIndex >= activeTouches.size()) SR_ERROR("Touch index [{0}] out of range! Make sure to use TouchManager::GetTouchCount() and retrieve touches within the returned range.", touchIndex);
        return activeTouches[touchIndex];
    }

    /* --- PRIVATE METHODS --- */

    void GameActivityTouchManager::Update()
    {
        android_input_buffer* inputBuffer = android_app_swap_input_buffers(gameActivityContext.GetApp());
        if (inputBuffer == nullptr) return;

        for (size i = 0; i < inputBuffer->motionEventsCount; i++)
        {
            const GameActivityMotionEvent &motionEvent = inputBuffer->motionEvents[i];
            const GameActivityPointerAxes &pointer = motionEvent.pointers[(motionEvent.action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT];

            switch (motionEvent.action)
            {
                case AMOTION_EVENT_ACTION_DOWN:
                {
                    // Create touch
                    const Touch touch = Touch({
                        .tapTime = TimePoint::Now(),
                        .tapCount = 1,
                        .force = GameActivityPointerAxes_getPressure(&pointer),
                        .position = Vector2(GameActivityPointerAxes_getX(&pointer), static_cast<float32>(gameActivityContext.GetPrimaryScreen().GetHeight()) - GameActivityPointerAxes_getY(&pointer)),
                        .deltaPosition = { 0.0f, 0.0f },
                        .ID = reinterpret_cast<void*>(pointer.id)
                    });

                    // Try to find the touch in the vector
                    auto iterator = std::find(activeTouches.begin(), activeTouches.end(), touch);

                    // Add if missing, otherwise overwrite it
                    if (iterator == activeTouches.end())
                    {
                        activeTouches.push_back(touch);
                        iterator = activeTouches.end() - 1;
                    }
                    else
                    {
                        *iterator = touch;
                    }

                    // Dispatch events
                    GetTouchTapDispatcher().DispatchEvent(*iterator);
                    break;
                }
                case AMOTION_EVENT_ACTION_MOVE:
                {
                    // Check if touch has been stored before
                    auto iterator = std::find_if(activeTouches.begin(), activeTouches.end(), [&pointer](const Touch &item) { return uintptr_t(item.GetID()) == pointer.id; });
                    if (iterator == activeTouches.end()) continue;

                    // Get position within the screen and flip Y coordinate
                    const Vector2 position = Vector2(GameActivityPointerAxes_getX(&pointer), static_cast<float32>(gameActivityContext.GetPrimaryScreen().GetHeight()) - GameActivityPointerAxes_getY(&pointer));

                    // Create touch
                    const Touch touch = Touch({
                        .tapTime = TimePoint::Now() - iterator.GetHoldDuration(),
                        .tapCount = 1,
                        .force = GameActivityPointerAxes_getPressure(&pointer),
                        .position = position,
                        .deltaPosition = position - iterator.GetPosition(),
                        .ID = reinterpret_cast<void*>(pointer.id)
                    });

                    // Overwrite old touch
                    *iterator = touch;

                    // Dispatch events
                    GetTouchMoveDispatcher().DispatchEvent(*iterator);
                    break;
                }
                case AMOTION_EVENT_ACTION_CANCEL:
                case AMOTION_EVENT_ACTION_UP:
                {
                    // Remove ended touches from the vector
                    activeTouches.erase(std::ranges::remove_if(activeTouches.begin(), activeTouches.end(), [&pointer, this](const Touch &item)
                    {
                        if (uintptr_t(item.GetID()) == pointer.id)
                        {
                            GetTouchEndDispatcher().DispatchEvent(item);
                            return true;
                        }
                        return false;
                    }), activeTouches.end());
                    break;
                }
                default:
                {
                    continue;
                }
            }
        }
        android_app_clear_motion_events(inputBuffer);
    }

}