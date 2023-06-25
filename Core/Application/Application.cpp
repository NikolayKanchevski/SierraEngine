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

void Application::Start()
{
    // Start up the engine
    EngineCore::Initialize();

    // Initialize the world
    World::Start();

    // Create renderer
    UniquePtr<Window> window = Window::Create({ });
    UniquePtr<VulkanRenderer> renderer = DeferredVulkanRenderer::Create({ .window = window, .createImGuiInstance = true, .createImGuizmoLayer = true });

    // Create camera
    Entity cameraEntity = Entity("Camera");
    camera = cameraEntity.AddComponent<Camera>();
    cameraEntity.GetComponent<Transform>().SetWorldPosition(-5.0f, -2.5f, 5.0f);
    cameraEntity.GetComponent<Transform>().SetWorldRotation(270.0f, NO_CHANGE, NO_CHANGE);

    // Create directional light
    DirectionalLight &directionalLight = Entity("Directional Light").AddComponent<DirectionalLight>();
    directionalLight.intensity = 2.0f;
    directionalLight.GetComponent<Transform>().SetWorldRotation(NO_CHANGE, 90.0f, NO_CHANGE);

    // Load 3D models in a grid view
    tankModels.reserve(MODEL_GRID_SIZE_X * MODEL_GRID_SIZE_Y * MODEL_GRID_SIZE_Z);
    for (int i = MODEL_GRID_SIZE_X; i--;)
    {
        int x = (i * MODEL_SPACING_FACTOR_X) - (MODEL_GRID_SIZE_X * MODEL_SPACING_FACTOR_X) / 2;

        for (int j = MODEL_GRID_SIZE_Z; j--;)
        {
            int z = (j * MODEL_SPACING_FACTOR_Z) - (MODEL_GRID_SIZE_Z * MODEL_SPACING_FACTOR_Z) / 2;

            for (int k = MODEL_GRID_SIZE_Y; k--;)
            {
                int y = (k * MODEL_SPACING_FACTOR_Y) - (MODEL_GRID_SIZE_Y * MODEL_SPACING_FACTOR_Y) / 2;

                tankModels.push_back(Model::Load(File::OUTPUT_FOLDER_PATH + "Models/Chieftain/T95_FV4201_Chieftain.fbx"));
                tankModels.back()->GetOriginEntity().GetComponent<Transform>().SetWorldPosition({ x, y, z });
            }
        }
    }

    auto cubeModel = Model::Load(File::OUTPUT_FOLDER_PATH + "Models/DefaultCube/DefaultCube.fbx");
    cubeModel->GetMesh(0).GetComponent<Transform>().SetPosition(0.0f, -6.0f, 0.0f);
    cubeModel->GetMesh(0).GetComponent<Transform>().SetScale(30.0, 1.0f, 30.0f);

    // Loop while renderer is active
    while (!window->IsClosed())
    {
        // Prepare utility classes
        World::Prepare();

        // Prepare renderer for next frame
        renderer->Prepare();

        // Do per-frame actions
        RenderLoop(renderer);

        World::Update();

        // Update and render world
        renderer->Render();
    }

    // Destroy created resources
    renderer->Destroy();
    window->Destroy();

    // Deallocate world memory
    World::Shutdown();

    // Deallocate other rendering memory
    EngineCore::Terminate();
}

void Application::RenderLoop(UniquePtr<VulkanRenderer> &renderer)
{
    // If the window of the renderer is required to be focused but is not return before executing useless code
    if (renderer->GetWindow()->IsFocusRequired() && !renderer->GetWindow()->IsFocused()) return;

    // Update world objects
    UpdateObjects();

    // Calculate camera movement
    DoCameraMovement();
}

void Application::UpdateObjects()
{
    // Calculate an animated timeSin value and update meshes
    float timeSin = glm::sin(Time::GetUpTime());

    // Rotate tank models
    for (const auto &tankModel : tankModels)
    {
        tankModel->GetMesh(3).GetComponent<Transform>().SetRotation(timeSin * 45.0f, NO_CHANGE, NO_CHANGE);
        tankModel->GetMesh(4).GetComponent<Transform>().SetRotation(timeSin * 45.0f, NO_CHANGE, NO_CHANGE);
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
    if (!Cursor::IsCursorShown())
    {
        // Cache the transform of the camera
        Transform &cameraTransform = camera.GetComponent<Transform>();
        Vector3 newCameraRotation = cameraTransform.GetRotation();

        // Move camera accordingly
        if (Input::GetKeyHeld(GLFW_KEY_W)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetFrontDirection());
        if (Input::GetKeyHeld(GLFW_KEY_S)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetBackDirection());
        if (Input::GetKeyHeld(GLFW_KEY_A)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetLeftDirection());
        if (Input::GetKeyHeld(GLFW_KEY_D)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetRightDirection());
        if (Input::GetKeyHeld(GLFW_KEY_E) || Input::GetKeyHeld(GLFW_KEY_SPACE)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetUpDirection());
        if (Input::GetKeyHeld(GLFW_KEY_Q) || Input::GetKeyHeld(GLFW_KEY_LEFT_CONTROL)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetDownDirection());

        // Apply camera rotation based on mouse movement
        newCameraRotation.x += Cursor::GetHorizontalCursorOffset() * CAMERA_LOOK_SPEED;
        newCameraRotation.y += Cursor::GetVerticalCursorOffset() * CAMERA_LOOK_SPEED;

        // Check if a game pad (player 0) is connected
        if (Input::GetGamePadConnected())
        {
            // Get the left stick's axis and calculate movement based on it
            cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * Input::GetVerticalGamePadLeftStickAxis() * camera.GetFrontDirection());
            cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * Input::GetHorizontalGamePadLeftStickAxis() * camera.GetRightDirection());

            // Depending on what buttons are held move the camera
            if (Input::GetGamePadButtonHeld(GLFW_GAMEPAD_BUTTON_A)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetUpDirection());
            if (Input::GetGamePadButtonHeld(GLFW_GAMEPAD_BUTTON_X)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetDownDirection());

            // Rotate the camera based on the right stick's axis
            newCameraRotation.x += Input::GetHorizontalGamePadRightStickAxis() * GAMEPAD_CAMERA_LOOK_SPEED;
            newCameraRotation.y += Input::GetVerticalGamePadRightStickAxis() * GAMEPAD_CAMERA_LOOK_SPEED;

            // Change camera FOV depending on game pad's right trigger
            camera.SetFOV(Math::Clamp(Input::GetGamePadRightTriggerAxis() * 90.0f, 45.0f, 90.0f));
        }

        // Clamp camera pith between -85.0f and +85.0f
        newCameraRotation.y = Math::Clamp(newCameraRotation.y, -85.0f, 85.0f);

        // Apply transformations to camera
        camera.GetComponent<Transform>().SetRotation(newCameraRotation);
    }
}