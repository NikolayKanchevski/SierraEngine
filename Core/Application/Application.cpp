//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Application.h"

#define MODEL_GRID_SIZE_X 1
#define MODEL_GRID_SIZE_Y 1
#define MODEL_GRID_SIZE_Z 1

#define MODEL_SPACING_FACTOR_X 10
#define MODEL_SPACING_FACTOR_Y 10
#define MODEL_SPACING_FACTOR_Z 15

/* --- POLLING METHODS --- */

void Application::Start()
{
    // Start up the engine
    EngineCore::Initialize();

    // Create renderer
    std::unique_ptr<Window> window = Window::Create({ });
    std::unique_ptr<MainVulkanRenderer> renderer = MainVulkanRenderer::Create({ .window = window, .createImGuiInstance = true, .createImGuizmoLayer = true });

    // Initialize the world
    World::Start();

    // Create camera
    Entity cameraEntity = Entity("Camera");
    camera = cameraEntity.AddComponent<Camera>();
    cameraEntity.GetTransform().position = { -5.0f, -2.5f, 5.0f };

    // Create directional light
    DirectionalLight &directionalLight = Entity("Directional Light").AddComponent<DirectionalLight>();
    directionalLight.direction = glm::normalize(camera.GetComponent<Transform>().position - glm::vec3(-7, 5, -7));
    directionalLight.intensity = 2.0f;

    // Create point light
    pointLight = &Entity("Point Light").AddComponent<PointLight>();
    pointLight->intensity = 3.0f;

    // Load 3D models in a grid view
    tankModels.reserve(MODEL_GRID_SIZE_X * MODEL_GRID_SIZE_Y * MODEL_GRID_SIZE_Z);
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

                pointLight->GetComponent<Transform>().position = { x, y + 3, z };
            }
        }
    }

    // Loop while renderer is active
    while (!window->IsClosed())
    {
        // Prepare utility classes
        World::Prepare();

        // Prepare renderer for next frame
        renderer->Prepare();

        // Do per-frame actions
        RenderLoop(renderer);

        renderer->Update();

        // Push updates to renderer
        World::Update();

        // Update and render world
        renderer->Render();
    }

    renderer->Destroy();
    window->Destroy();

    // Deallocate world memory
    EngineCore::Terminate();
}

void Application::RenderLoop(std::unique_ptr<MainVulkanRenderer> &renderer)
{
    // If the window of the renderer is required to be focused but is not return before executing useless code
    if (renderer->GetWindow()->IsFocusRequired() && !renderer->GetWindow()->IsFocused()) return;

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

    // Move point light
    const static float startingZ = pointLight->GetComponent<Transform>().position.z;
    pointLight->GetComponent<Transform>().position.z = (2 * timeSin) + startingZ;
}

void Application::DoCameraMovement()
{
    // Toggle cursor visibility on every escape key press
    if (Input::GetKeyPressed(GLFW_KEY_ESCAPE))
    {
        Cursor::SetCursorVisibility(!Cursor::IsCursorShown());
    }

    // If the cursor is visible return
    if (!Cursor::IsCursorShown())
    {
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
        pitch += Cursor::GetVerticalCursorOffset() * CAMERA_LOOK_SPEED;

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

        // Clamp camera pith between -85.0f and +85.0f
        pitch = Math::Clamp(pitch, -85.0f, 85.0f);
    }

    // Apply transformations to camera
    camera.GetComponent<Transform>().rotation = { yaw, pitch, 0.0f };
}