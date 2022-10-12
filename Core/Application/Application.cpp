//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Application.h"
#include <iostream>

using namespace Sierra::Core;
using namespace Sierra::Core::Rendering;
using namespace Sierra::Engine::Classes;

/* --- POLLING METHODS --- */
void Application::Start()
{
    // Create the renderer
    VulkanRenderer renderer("Sierra Engine v1.0.0", true, true, true);

    // Get a reference to the window of the renderer
    Window &window = renderer.GetWindow();

    // Initialize the world
    World::Start();

    // Loop while renderer is active
    while (renderer.IsActive())
    {
        // Prepare utility classes
        World::Prepare(renderer);

        // Set the window title to display current FPS
        window.SetTitle("FPS: " + std::to_string(Time::GetFPS()) + " | Draw Time: " + std::to_string(renderer.GetDrawTime()) + "ms");

        // Do per-frame actions
        RenderLoop(renderer);

        // Update and render world
        World::Update(renderer);
    }

    // Deallocate world memory
    World::Shutdown();
}

void Application::RenderLoop(VulkanRenderer &renderer)
{
    // If the window of the renderer is required to be focused but is not return before executing useless code
    if (renderer.GetWindow().IsFocusRequired() && !renderer.GetWindow().IsFocused()) return;

    ImGui::ShowDemoWindow();

    camera.transform.rotation = { 90, 0, 0 };
    if (Input::GetKeyHeld(GLFW_KEY_W)) camera.transform.position += camera.GetFrontDirection() * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKeyHeld(GLFW_KEY_S)) camera.transform.position -= camera.GetFrontDirection() * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKeyHeld(GLFW_KEY_A)) camera.transform.position += camera.GetLeftDirection() * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKeyHeld(GLFW_KEY_D)) camera.transform.position -= camera.GetLeftDirection() * Time::GetDeltaTime();

    yaw += Cursor::GetHorizontalCursorOffset() * CAMERA_LOOK_SPEED;
    pitch -= Cursor::GetVerticalCursorOffset() * CAMERA_LOOK_SPEED;
    pitch = Math::Clamp(pitch, -89.0f, 89.0f);

    glm::vec3 newCameraFrontDirection;
    newCameraFrontDirection.x = (float) (glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)));
    newCameraFrontDirection.y = (float) (glm::sin(glm::radians(pitch)));
    newCameraFrontDirection.z = (float) (glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch)));
    camera.frontDirection = glm::normalize(newCameraFrontDirection);

//    std::cout << pitch << "\n";
//    std::cout << Mesh::worldMeshes[0]->transform.position.x << "\n";
//    std::cout << Camera::GetMainCamera()->transform.position.x << "\n";
//    std::cout << "Direction: " << camera.frontDirection.x << " || " << camera.frontDirection.y << " || " << camera.frontDirection.z << "\n";
    std::cout
    << "Position: " << Camera::GetMainCamera()->transform.position.x << " || " << Camera::GetMainCamera()->transform.position.y << " || " << Camera::GetMainCamera()->transform.position.z << "\n";
//    << "Rotation: " << Camera::GetMainCamera()->frontDirection.x << " || " << Camera::GetMainCamera()->frontDirection.y << " || " << Camera::GetMainCamera()->frontDirection.z << "\n";
}