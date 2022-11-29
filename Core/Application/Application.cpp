//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Application.h"
#include <iostream>

#define MODEL_GRID_SIZE_X 1
#define MODEL_GRID_SIZE_Y 1
#define MODEL_GRID_SIZE_Z 1

#define MODEL_SPACING_FACTOR_X 10
#define MODEL_SPACING_FACTOR_Y 10
#define MODEL_SPACING_FACTOR_Z 15

/* --- POLLING METHODS --- */

void Application::Start()
{
    // Create renderer
    VulkanRenderer renderer("Sierra Engine v1.0.0", true, true);

    // Get a reference to the window of the renderer
    Window &window = renderer.GetWindow();

    // Initialize the world
    World::Start();

    // Create camera
    Entity cameraEntity = Entity("Camera");
    camera = cameraEntity.AddComponent<Camera>();
    cameraEntity.GetTransform().position = { 0.0f, 1.75f, 10.0f };

    // Create directional light
    DirectionalLight directionalLight = Entity("Directional Light").AddComponent<DirectionalLight>();
    directionalLight.direction = glm::normalize(camera.GetComponent<Transform>().position - glm::vec3(0, 0, 0));

    // Load 3D models in a grid view
    for (uint32_t i = MODEL_GRID_SIZE_X; i--;)
    {
        int x = (i * MODEL_SPACING_FACTOR_X) - (MODEL_GRID_SIZE_X * MODEL_SPACING_FACTOR_X) / 2;

        for (uint32_t j = MODEL_GRID_SIZE_Z; j--;)
        {
            int z = (j * MODEL_SPACING_FACTOR_Z) - (MODEL_GRID_SIZE_Z * MODEL_SPACING_FACTOR_Z) / 2;

            for (uint32_t k = MODEL_GRID_SIZE_Y; k--;)
            {
                int y = (k * MODEL_SPACING_FACTOR_Y) - (MODEL_GRID_SIZE_Y * MODEL_SPACING_FACTOR_Y) / 2;

                tankModels.push_back(Model::Load("Models/Chieftain/T95_FV4201_Chieftain.fbx"));
                for (uint32_t l = tankModels.back()->GetMeshCount(); l--;)
                {
                    tankModels.back()->GetMesh(l).GetComponent<Transform>().position = { x, y, z };
                }
            }
        }
    }

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

    // Update world objects
    UpdateObjects();

    // Calculate camera movement
    DoCameraMovement();

    // Update UI
    DisplayUI(renderer);
}

void Application::UpdateObjects()
{
    // Calculate an animated timeSin value and update meshes
    float timeSin = glm::sin(Time::GetUpTime());

    // Rotate tank models
    for (const auto &tankModel : tankModels)
    {
        tankModel->GetMesh(3).GetComponent<Transform>().rotation.x = timeSin * 45.0f;
        tankModel->GetMesh(4).GetComponent<Transform>().rotation.x = timeSin * 45.0f;
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

    // Cache the transform of the camera
    Transform &cameraTransform = camera.GetComponent<Transform>();

    // Move camera accordingly
    if (Input::GetKeyHeld(GLFW_KEY_W)) cameraTransform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetFrontDirection();
    if (Input::GetKeyHeld(GLFW_KEY_S)) cameraTransform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetBackDirection();
    if (Input::GetKeyHeld(GLFW_KEY_A)) cameraTransform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetLeftDirection();
    if (Input::GetKeyHeld(GLFW_KEY_D)) cameraTransform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetRightDirection();
    if (Input::GetKeyHeld(GLFW_KEY_E) || Input::GetKeyHeld(GLFW_KEY_SPACE)) cameraTransform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetUpDirection();
    if (Input::GetKeyHeld(GLFW_KEY_Q) || Input::GetKeyHeld(GLFW_KEY_LEFT_CONTROL)) cameraTransform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetDownDirection();

    // Apply camera rotation based on mouse movement
    yaw += Cursor::GetHorizontalCursorOffset() * CAMERA_LOOK_SPEED;
    pitch -= Cursor::GetVerticalCursorOffset() * CAMERA_LOOK_SPEED;

    // Check if a game pad (player 0) is connected
    if (Input::GetGamePadConnected())
    {
        // Get the left stick's axis and calculate movement based on it
        cameraTransform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * Input::GetVerticalGamePadLeftStickAxis() * camera.GetFrontDirection();
        cameraTransform.position -= CAMERA_MOVE_SPEED * Time::GetDeltaTime() * Input::GetHorizontalGamePadLeftStickAxis() * camera.GetLeftDirection();

        // Depending on what buttons are held move the camera
        if (Input::GetGamePadButtonHeld(GLFW_GAMEPAD_BUTTON_A)) cameraTransform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetUpDirection();
        if (Input::GetGamePadButtonHeld(GLFW_GAMEPAD_BUTTON_X)) cameraTransform.position += CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetDownDirection();

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