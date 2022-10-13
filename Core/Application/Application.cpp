//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Application.h"
#include <iostream>

/* --- POLLING METHODS --- */
void Application::Start()
{
    // Create renderer
    VulkanRenderer renderer("Sierra Engine v1.0.0", false);

    // Get a reference to the window of the renderer
    Window &window = renderer.GetWindow();

    Cursor::SetCursorVisibility(true);

    // Initialize the world
    World::Start();

    // Loop while renderer is active
    while (renderer.IsActive())
    {
        // Prepare utility classes
        World::Prepare(renderer);

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

    // Build UI
    DisplayUI(renderer);

    // Calculate camera movement
    DoCameraMovement();
}

void Application::DisplayUI(VulkanRenderer &renderer)
{
    const ImGuiWindowFlags WINDOW_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing |
                                          ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    ImGui::SetNextWindowPos({ renderer.GetWindow().GetWidth() - 10.0f, 10.0f }, ImGuiCond_Always, { 1, 0 });
    ImGui::SetNextWindowSizeConstraints({ (float) renderer.GetWindow().GetWidth() / 8, (float) renderer.GetWindow().GetHeight() / 8 }, { 10000, 10000 });

    // Draw renderer information tab
    bool rendererInfoOpen = true;
    if (ImGui::Begin("Renderer Information", &rendererInfoOpen, WINDOW_FLAGS) || rendererInfoOpen)
    {
        ImGui::Text("%s", ("CPU Frame Time: " + std::to_string(Time::GetFPS()) + " FPS").c_str());
        ImGui::Text("%s", ("GPU Draw Time: " + std::to_string(renderer.GetRendererInfo().drawTime) + " ms").c_str());
        ImGui::Separator();
        ImGui::Text("%s", ("Total meshes being drawn: " + std::to_string(renderer.GetRendererInfo().meshesDrawn)).c_str());
        ImGui::Text("%s", ("Total vertices in scene: " + std::to_string(renderer.GetRendererInfo().verticesDrawn)).c_str());

        ImGui::End();
    }


    // If game pad (player 0) is connected render a tab with its properties
    if (Input::GetGamePadConnected())
    {
        bool gamePadInfoOpen = true;
        if (ImGui::Begin("Game Pad Data", &gamePadInfoOpen, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", ("Game pad [" + Input::GetGamePadName() + "] properties:").c_str());
            ImGui::Text("%s", ("Left gamepad stick: [" + std::to_string(Input::GetGamePadLeftStickAxis().x) + " || " + std::to_string(Input::GetGamePadLeftStickAxis().y) + "]").c_str());
            ImGui::Text("%s", ("Right gamepad stick: [" + std::to_string(Input::GetGamePadRightStickAxis().x) + " || " + std::to_string(Input::GetGamePadRightStickAxis().y) + "]").c_str());
            ImGui::Text("%s", ("Left trigger: [" + std::to_string(Input::GetGamePadLeftTriggerAxis()) + "]").c_str());
            ImGui::Text("%s", ("Right trigger: [" + std::to_string(Input::GetGamePadRightTriggerAxis()) + "]").c_str());
            ImGui::RadioButton("\"A\" pressed", Input::GetGamePadButtonPressed(GLFW_GAMEPAD_BUTTON_A));
            ImGui::RadioButton("\"A\" held", Input::GetGamePadButtonHeld(GLFW_GAMEPAD_BUTTON_A));
            ImGui::RadioButton("\"A\" released", Input::GetGamePadButtonReleased(GLFW_GAMEPAD_BUTTON_A));

            ImGui::End();
        }
    }
}


void Application::DoCameraMovement()
{
    // Toggle cursor visibility on every escape key press
    if (Input::GetKeyPressed(GLFW_KEY_ESCAPE))
    {
        Cursor::SetCursorVisibility(!Cursor::IsCursorShown());
    }

    // If the cursor is visible return
    if (Cursor::IsCursorShown()) return;

    // Move camera accordingly
    if (Input::GetKeyHeld(GLFW_KEY_W)) camera.transform.position += camera.GetFrontDirection() * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKeyHeld(GLFW_KEY_S)) camera.transform.position += camera.GetBackDirection() * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKeyHeld(GLFW_KEY_A)) camera.transform.position += camera.GetLeftDirection() * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKeyHeld(GLFW_KEY_D)) camera.transform.position += camera.GetRightDirection() * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKeyHeld(GLFW_KEY_E) || Input::GetKeyHeld(GLFW_KEY_SPACE)) camera.transform.position += camera.GetUpDirection() * CAMERA_MOVE_SPEED * Time::GetDeltaTime();
    if (Input::GetKeyHeld(GLFW_KEY_Q) || Input::GetKeyHeld(GLFW_KEY_LEFT_CONTROL)) camera.transform.position += camera.GetDownDirection() * CAMERA_MOVE_SPEED * Time::GetDeltaTime();

    // Apply camera rotation based on mouse movement
    yaw += Cursor::GetHorizontalCursorOffset() * CAMERA_LOOK_SPEED;
    pitch -= Cursor::GetVerticalCursorOffset() * CAMERA_LOOK_SPEED;

    // Check if a game pad (player 0) is connected
    if (Input::GetGamePadConnected())
    {
        // Get the left stick's axis and calculate movement based on it
        camera.transform.position += Input::GetVerticalGamePadLeftStickAxis() * CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetFrontDirection();
        camera.transform.position -= Input::GetHorizontalGamePadLeftStickAxis() * CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetLeftDirection();

        // Depending on what buttons are held move the camera
        if (Input::GetGamePadButtonHeld(GLFW_GAMEPAD_BUTTON_A)) camera.transform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetUpDirection();
        if (Input::GetGamePadButtonHeld(GLFW_GAMEPAD_BUTTON_X)) camera.transform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetDownDirection();

        // Rotate the camera based on the right stick's axis
        yaw -= Input::GetHorizontalGamePadRightStickAxis() * GAMEPAD_CAMERA_LOOK_SPEED;
        pitch += Input::GetVerticalGamePadRightStickAxis() * GAMEPAD_CAMERA_LOOK_SPEED;

        // Change camera FOV depending on game pad's right trigger
        camera.fov = Math::Clamp(Input::GetGamePadRightTriggerAxis() * 45.0f, 45.0f, 90.0f);
    }

    // Clamp camera pith between -89.0f and +89.0f
    pitch = Math::Clamp(pitch, -89.0f, 89.0f);

    // Apply transformations to camera
    glm::vec3 newCameraFrontDirection;
    newCameraFrontDirection.x = (float) (glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)));
    newCameraFrontDirection.y = (float) (glm::sin(glm::radians(pitch)));
    newCameraFrontDirection.z = (float) (glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch)));
    camera.SetFrontDirection(glm::normalize(newCameraFrontDirection));
}