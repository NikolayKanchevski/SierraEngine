//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Application.h"

void Application::Start()
{
    // Start up the engine
    World::Initialize();

    // Create renderer
    UniquePtr<Window> window = Window::Create({ .title = "Sierra Engine v1.0.0" });
    UniquePtr<VulkanRenderer> renderer = DeferredVulkanRenderer::Create({ .window = window, .createImGuiInstance = true, .createImGuizmoLayer = true });

    auto ID = AssetManager::ImportModel("Models/Chieftain/T95_FV4201_Chieftain.fbx");
    auto entity = AssetManager::GetModelCollection().GetResource(ID)->Spawn();

    // Create camera
    Entity cameraEntity = Entity("Camera");
    camera = cameraEntity.AddComponent<Camera>();
    cameraEntity.GetComponent<Transform>().SetWorldPosition(NO_CHANGE, NO_CHANGE, 5.0f);
    cameraEntity.GetComponent<Transform>().SetWorldRotation(270.0f, NO_CHANGE, NO_CHANGE);

    // Create directional light
    DirectionalLight &directionalLight = Entity("Directional Light").AddComponent<DirectionalLight>();
    directionalLight.intensity = 2.0f;
    directionalLight.GetComponent<Transform>().SetWorldRotation(NO_CHANGE, 90.0f, NO_CHANGE);


    AssetManager::GetDefaultCollection().GetModel(ModelType::CUBE)->Spawn();

    // Loop while renderer is active
    while (!window->IsClosed())
    {
        // Prepare utility classes
        World::Update();

        // Prepare renderer for next frame
        renderer->Prepare();

        // Do per-frame actions
        RenderLoop(renderer);

        // Render the scene
        renderer->Render();
    }

    // Destroy created resources
    renderer->Destroy();
    window->Destroy();

    // Deallocate world memory
    World::Shutdown();
}

void Application::RenderLoop(UniquePtr<VulkanRenderer> &renderer)
{
    // If the window of the renderer is required to be focused but is not return before executing useless code
    if (renderer->GetWindow()->IsFocusRequired() && !renderer->GetWindow()->IsFocused()) return;

    // Calculate camera movement
    DoCameraMovement();
}

void Application::DoCameraMovement()
{
    // Toggle cursor visibility on every escape key press
    if (Input::GetKeyPressed(Key::ESCAPE))
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
        if (Input::GetKeyHeld(Key::W)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetFrontDirection());
        if (Input::GetKeyHeld(Key::S)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetBackDirection());
        if (Input::GetKeyHeld(Key::A)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetLeftDirection());
        if (Input::GetKeyHeld(Key::D)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetRightDirection());
        if (Input::GetKeyHeld(Key::E) || Input::GetKeyHeld(Key::SPACE)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetUpDirection());
        if (Input::GetKeyHeld(Key::Q) || Input::GetKeyHeld(Key::LEFT_CONTROL)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetDownDirection());

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
            if (Input::GetGamePadButtonHeld(GamePadButton::A)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetUpDirection());
            if (Input::GetGamePadButtonHeld(GamePadButton::X)) cameraTransform.SetPosition(cameraTransform.GetPosition() + CAMERA_MOVE_SPEED * Time::GetDeltaTime() * camera.GetDownDirection());

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