//
// Created by Nikolay Kanchevski on 28.01.24.
//

#include "ImGuiRenderTask.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    ImGuiRenderTask::ImGuiRenderTask(const RenderingContext &renderingContext, const ImGuiRenderTaskCreateInfo &createInfo)
        : renderingContext(renderingContext), framebufferSize({ static_cast<float32>(createInfo.templateImage->GetWidth()) / createInfo.scaling, static_cast<float32>(createInfo.templateImage->GetHeight()) / createInfo.scaling }), scaling(createInfo.scaling), style(createInfo.style)
    {
        // Create render pass
        renderPass = renderingContext.CreateRenderPass({
            .name = "Render Pass of ImGui Render Task [" + std::string(createInfo.name) + "]",
            .attachments = {
                { .templateImage = createInfo.templateImage }
            },
            .subpassDescriptions = {
                { .renderTargets = { 0 } }
            }
        });

        // Create shared graphics pipeline
        pipeline = renderingContext.CreateGraphicsPipeline({
            .name = "Pipeline of ImGui Render Task [" + std::string(createInfo.name) + "]",
            .vertexInputs = { VertexInput::Position_2D, VertexInput::UV, VertexInput::Color },
            .vertexShader = sharedResources.vertexShader,
            .fragmentShader = sharedResources.fragmentShader,
            .pushConstantSize = sizeof(PushConstant),
            .templateRenderPass = renderPass,
            .blendMode = BlendMode::Alpha,
            .cullMode = CullMode::None
        });

        // Create initial vertex buffer
        vertexBuffer = renderingContext.CreateBuffer({
            .name = "Vertex Buffer of ImGui Render Task",
            .memorySize = 4096 * sizeof(ImDrawVert),
            .usage = BufferUsage::Vertex,
            .memoryLocation = BufferMemoryLocation::CPU
        });

        // Create initial index buffer
        indexBuffer = renderingContext.CreateBuffer({
            .name = "Index Buffer of ImGui Render Task",
            .memorySize = 8192 * sizeof(ImDrawIdx),
            .usage = BufferUsage::Index,
            .memoryLocation = BufferMemoryLocation::CPU
        });
    }

    /* --- POLLING METHODS --- */

    void ImGuiRenderTask::CreateResources(const RenderingContext &renderingContext, std::unique_ptr<ResourceTable> &resourceTable, const uint32 fontAtlasIndex, const uint32 fontSamplerIndex, std::unique_ptr<CommandBuffer> &commandBuffer)
    {
        // Create context
        sharedResources.context = ImGui::CreateContext();
        ImGui::SetCurrentContext(sharedResources.context);

        // Configure context
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | (SR_PLATFORM_MOBILE * ImGuiConfigFlags_IsTouchScreen);
        io.BackendFlags = ImGuiBackendFlags_RendererHasVtxOffset;

        // Create shared shaders
        sharedResources.vertexShader = renderingContext.CreateShader({ .name = "Shared ImGui Render Task vertex Shader", .shaderBundlePath = File::GetResourcesDirectoryPath() / "shaders/ImGuiRenderTask.vert.shader",  .shaderType = ShaderType::Vertex });
        sharedResources.fragmentShader = renderingContext.CreateShader({ .name = "Shared ImGui Render Task fragment Shader", .shaderBundlePath = File::GetResourcesDirectoryPath() / "shaders/ImGuiRenderTask.frag.shader",  .shaderType = ShaderType::Fragment });

        // Create font sampler
        sharedResources.fontSampler = renderingContext.CreateSampler({ .name = "Shared ImGui Render Task font Sampler" });

        // Load font atlas
        int atlasWidth, atlasHeight;
        uchar* atlasMemory = nullptr;
        ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&atlasMemory, &atlasWidth, &atlasHeight);

        constexpr ImageFormat FONT_ATLAS_IMAGE_FORMAT = ImageFormat::R8_UNorm;
        constexpr ImageUsage FONT_ATLAS_IMAGE_USAGE = ImageUsage::Sample | ImageUsage::DestinationMemory;

        // Create default font atlas image
        SR_ERROR_IF(!renderingContext.GetDevice().IsImageFormatSupported(FONT_ATLAS_IMAGE_FORMAT, FONT_ATLAS_IMAGE_USAGE), "Cannot create default ImGui font, as required image format [ .channels = ImageChannels::R, .memoryType = ImageMemoryType::UNorm8 ] is unsupported!");
        sharedResources.defaultFontAtlas = renderingContext.CreateImage({
            .name = "Shared ImGui Render Task font atlas",
            .width = static_cast<uint32>(atlasWidth),
            .height = static_cast<uint32>(atlasHeight),
            .format = ImageFormat::R8_UNorm,
            .usage = ImageUsage::DestinationMemory | ImageUsage::Sample
        });

        // Prepare image for writing
        commandBuffer->SynchronizeImageUsage(sharedResources.defaultFontAtlas, ImageCommandUsage::None, ImageCommandUsage::MemoryWrite);

        // Create staging buffer to hold atlas data
        auto defaultFontStagingBuffer = renderingContext.CreateBuffer({
            .name = "Default ImGui Render Task font staging buffer",
            .memorySize = static_cast<uint64>(atlasWidth) * atlasHeight * 1 * 1,
            .usage = BufferUsage::SourceMemory,
            .memoryLocation = BufferMemoryLocation::CPU
        });
        defaultFontStagingBuffer->CopyFromMemory(atlasMemory);

        // Copy atlas to image
        commandBuffer->CopyBufferToImage(defaultFontStagingBuffer, sharedResources.defaultFontAtlas);
        commandBuffer->QueueBufferForDestruction(std::move(defaultFontStagingBuffer));

        // Prepare image for shader reading
        commandBuffer->SynchronizeImageUsage(sharedResources.defaultFontAtlas, ImageCommandUsage::MemoryWrite, ImageCommandUsage::GraphicsRead);

        // Bind resources
        {
            SR_ERROR_IF(fontAtlasIndex >= resourceTable->GetSampledImageCapacity(), "Cannot create ImGui render task resources, as specified font atlas index [{0}] is exceeds the sampled image capacity of the specified resource table [{1}], which is [{2}]!", fontAtlasIndex, resourceTable->GetName(), resourceTable->GetSampledImageCapacity());
            resourceTable->BindSampledImage(fontAtlasIndex, sharedResources.defaultFontAtlas);

            SR_ERROR_IF(fontSamplerIndex >= resourceTable->GetSamplerCapacity(), "Cannot create ImGui render task resources, as specified font sampler index [{0}] is exceeds the sampler capacity of the specified resource table [{1}], which is [{2}]!", fontSamplerIndex, resourceTable->GetName(), resourceTable->GetSamplerCapacity());
            resourceTable->BindSampler(fontSamplerIndex, sharedResources.fontSampler);
        }
    }

    void ImGuiRenderTask::DestroyResources()
    {
        ImGui::DestroyContext(sharedResources.context);
        sharedResources.context = nullptr;

        sharedResources.vertexShader = nullptr;
        sharedResources.fragmentShader = nullptr;
        sharedResources.fontSampler = nullptr;
        sharedResources.defaultFontAtlas = nullptr;
    }

    void ImGuiRenderTask::Update(const std::optional<std::reference_wrapper<const InputManager>> &inputManagerReference, const std::optional<std::reference_wrapper<const CursorManager>> &cursorManagerReference, const std::optional<std::reference_wrapper<const TouchManager>> &touchManagerReference)
    {
        ImGuiIO &io = ImGui::GetIO();
        if (inputManagerReference.has_value())
        {
            // Update key map
            const InputManager &inputManager = inputManagerReference.value();
            io.AddKeyEvent(ImGuiKey_Tab,                inputManager.IsKeyHeld(Key::Tab));
            io.AddKeyEvent(ImGuiKey_LeftArrow,          inputManager.IsKeyHeld(Key::LeftArrow));
            io.AddKeyEvent(ImGuiKey_RightArrow,         inputManager.IsKeyHeld(Key::RightArrow));
            io.AddKeyEvent(ImGuiKey_UpArrow,            inputManager.IsKeyHeld(Key::UpArrow));
            io.AddKeyEvent(ImGuiKey_DownArrow,          inputManager.IsKeyHeld(Key::DownArrow));
            io.AddKeyEvent(ImGuiKey_PageUp,             inputManager.IsKeyHeld(Key::PageUp));
            io.AddKeyEvent(ImGuiKey_PageDown,           inputManager.IsKeyHeld(Key::PageDown));
            io.AddKeyEvent(ImGuiKey_Home,               inputManager.IsKeyHeld(Key::Home));
            io.AddKeyEvent(ImGuiKey_End,                inputManager.IsKeyHeld(Key::End));
            io.AddKeyEvent(ImGuiKey_Insert,             inputManager.IsKeyHeld(Key::Insert));
            io.AddKeyEvent(ImGuiKey_Delete,             inputManager.IsKeyHeld(Key::Delete));
            io.AddKeyEvent(ImGuiKey_Backspace,          inputManager.IsKeyHeld(Key::Backspace));
            io.AddKeyEvent(ImGuiKey_Space,              inputManager.IsKeyHeld(Key::Space));
            io.AddKeyEvent(ImGuiKey_Enter,              inputManager.IsKeyHeld(Key::Enter));
            io.AddKeyEvent(ImGuiKey_Escape,             inputManager.IsKeyHeld(Key::Escape));
            io.AddKeyEvent(ImGuiKey_LeftCtrl,           inputManager.IsKeyHeld(Key::LeftControl));
            io.AddKeyEvent(ImGuiKey_LeftShift,          inputManager.IsKeyHeld(Key::LeftShift));
            io.AddKeyEvent(ImGuiKey_LeftAlt,            inputManager.IsKeyHeld(Key::LeftAlt));
            io.AddKeyEvent(ImGuiKey_LeftSuper,          inputManager.IsKeyHeld(Key::LeftSystem));
            io.AddKeyEvent(ImGuiKey_RightCtrl,          inputManager.IsKeyHeld(Key::RightControl));
            io.AddKeyEvent(ImGuiKey_RightShift,         inputManager.IsKeyHeld(Key::RightShift));
            io.AddKeyEvent(ImGuiKey_RightAlt,           inputManager.IsKeyHeld(Key::RightAlt));
            io.AddKeyEvent(ImGuiKey_RightSuper,         inputManager.IsKeyHeld(Key::RightSystem));
            io.AddKeyEvent(ImGuiKey_Menu,               inputManager.IsKeyHeld(Key::Menu));
            io.AddKeyEvent(ImGuiKey_0,                  inputManager.IsKeyHeld(Key::Number0));
            io.AddKeyEvent(ImGuiKey_1,                  inputManager.IsKeyHeld(Key::Number1));
            io.AddKeyEvent(ImGuiKey_2,                  inputManager.IsKeyHeld(Key::Number2));
            io.AddKeyEvent(ImGuiKey_3,                  inputManager.IsKeyHeld(Key::Number3));
            io.AddKeyEvent(ImGuiKey_4,                  inputManager.IsKeyHeld(Key::Number4));
            io.AddKeyEvent(ImGuiKey_5,                  inputManager.IsKeyHeld(Key::Number5));
            io.AddKeyEvent(ImGuiKey_6,                  inputManager.IsKeyHeld(Key::Number6));
            io.AddKeyEvent(ImGuiKey_7,                  inputManager.IsKeyHeld(Key::Number7));
            io.AddKeyEvent(ImGuiKey_8,                  inputManager.IsKeyHeld(Key::Number8));
            io.AddKeyEvent(ImGuiKey_9,                  inputManager.IsKeyHeld(Key::Number9));
            io.AddKeyEvent(ImGuiKey_A,                  inputManager.IsKeyHeld(Key::A));
            io.AddKeyEvent(ImGuiKey_B,                  inputManager.IsKeyHeld(Key::B));
            io.AddKeyEvent(ImGuiKey_C,                  inputManager.IsKeyHeld(Key::C));
            io.AddKeyEvent(ImGuiKey_D,                  inputManager.IsKeyHeld(Key::D));
            io.AddKeyEvent(ImGuiKey_E,                  inputManager.IsKeyHeld(Key::E));
            io.AddKeyEvent(ImGuiKey_F,                  inputManager.IsKeyHeld(Key::F));
            io.AddKeyEvent(ImGuiKey_G,                  inputManager.IsKeyHeld(Key::G));
            io.AddKeyEvent(ImGuiKey_H,                  inputManager.IsKeyHeld(Key::H));
            io.AddKeyEvent(ImGuiKey_I,                  inputManager.IsKeyHeld(Key::I));
            io.AddKeyEvent(ImGuiKey_J,                  inputManager.IsKeyHeld(Key::J));
            io.AddKeyEvent(ImGuiKey_K,                  inputManager.IsKeyHeld(Key::K));
            io.AddKeyEvent(ImGuiKey_L,                  inputManager.IsKeyHeld(Key::L));
            io.AddKeyEvent(ImGuiKey_M,                  inputManager.IsKeyHeld(Key::M));
            io.AddKeyEvent(ImGuiKey_N,                  inputManager.IsKeyHeld(Key::N));
            io.AddKeyEvent(ImGuiKey_O,                  inputManager.IsKeyHeld(Key::O));
            io.AddKeyEvent(ImGuiKey_P,                  inputManager.IsKeyHeld(Key::P));
            io.AddKeyEvent(ImGuiKey_Q,                  inputManager.IsKeyHeld(Key::Q));
            io.AddKeyEvent(ImGuiKey_R,                  inputManager.IsKeyHeld(Key::R));
            io.AddKeyEvent(ImGuiKey_S,                  inputManager.IsKeyHeld(Key::S));
            io.AddKeyEvent(ImGuiKey_T,                  inputManager.IsKeyHeld(Key::T));
            io.AddKeyEvent(ImGuiKey_U,                  inputManager.IsKeyHeld(Key::U));
            io.AddKeyEvent(ImGuiKey_V,                  inputManager.IsKeyHeld(Key::V));
            io.AddKeyEvent(ImGuiKey_W,                  inputManager.IsKeyHeld(Key::W));
            io.AddKeyEvent(ImGuiKey_X,                  inputManager.IsKeyHeld(Key::X));
            io.AddKeyEvent(ImGuiKey_Y,                  inputManager.IsKeyHeld(Key::Y));
            io.AddKeyEvent(ImGuiKey_Z,                  inputManager.IsKeyHeld(Key::Z));
            io.AddKeyEvent(ImGuiKey_F1,                 inputManager.IsKeyHeld(Key::F1));
            io.AddKeyEvent(ImGuiKey_F2,                 inputManager.IsKeyHeld(Key::F2));
            io.AddKeyEvent(ImGuiKey_F3,                 inputManager.IsKeyHeld(Key::F3));
            io.AddKeyEvent(ImGuiKey_F4,                 inputManager.IsKeyHeld(Key::F4));
            io.AddKeyEvent(ImGuiKey_F5,                 inputManager.IsKeyHeld(Key::F5));
            io.AddKeyEvent(ImGuiKey_F6,                 inputManager.IsKeyHeld(Key::F6));
            io.AddKeyEvent(ImGuiKey_F7,                 inputManager.IsKeyHeld(Key::F7));
            io.AddKeyEvent(ImGuiKey_F8,                 inputManager.IsKeyHeld(Key::F8));
            io.AddKeyEvent(ImGuiKey_F9,                 inputManager.IsKeyHeld(Key::F9));
            io.AddKeyEvent(ImGuiKey_F10,                inputManager.IsKeyHeld(Key::F10));
            io.AddKeyEvent(ImGuiKey_F11,                inputManager.IsKeyHeld(Key::F11));
            io.AddKeyEvent(ImGuiKey_F12,                inputManager.IsKeyHeld(Key::F12));
            io.AddKeyEvent(ImGuiKey_F13,                inputManager.IsKeyHeld(Key::F13));
            io.AddKeyEvent(ImGuiKey_F14,                inputManager.IsKeyHeld(Key::F14));
            io.AddKeyEvent(ImGuiKey_F15,                inputManager.IsKeyHeld(Key::F15));
            io.AddKeyEvent(ImGuiKey_F16,                inputManager.IsKeyHeld(Key::F16));
            io.AddKeyEvent(ImGuiKey_F17,                inputManager.IsKeyHeld(Key::F17));
            io.AddKeyEvent(ImGuiKey_F18,                inputManager.IsKeyHeld(Key::F18));
            io.AddKeyEvent(ImGuiKey_F19,                inputManager.IsKeyHeld(Key::F19));
            io.AddKeyEvent(ImGuiKey_F20,                inputManager.IsKeyHeld(Key::F20));
            io.AddKeyEvent(ImGuiKey_F21,                inputManager.IsKeyHeld(Key::F21));
            io.AddKeyEvent(ImGuiKey_F22,                inputManager.IsKeyHeld(Key::F22));
            io.AddKeyEvent(ImGuiKey_F23,                inputManager.IsKeyHeld(Key::F23));
            io.AddKeyEvent(ImGuiKey_F24,                inputManager.IsKeyHeld(Key::F24));
            io.AddKeyEvent(ImGuiKey_Apostrophe,         inputManager.IsKeyHeld(Key::Apostrophe));
            io.AddKeyEvent(ImGuiKey_Comma,              inputManager.IsKeyHeld(Key::Comma));
            io.AddKeyEvent(ImGuiKey_Minus,              inputManager.IsKeyHeld(Key::Minus));
            io.AddKeyEvent(ImGuiKey_Period,             inputManager.IsKeyHeld(Key::Period));
            io.AddKeyEvent(ImGuiKey_Slash,              inputManager.IsKeyHeld(Key::Slash));
            io.AddKeyEvent(ImGuiKey_Semicolon,          inputManager.IsKeyHeld(Key::Semicolon));
            io.AddKeyEvent(ImGuiKey_Equal,              inputManager.IsKeyHeld(Key::Equals));
            io.AddKeyEvent(ImGuiKey_LeftBracket,        inputManager.IsKeyHeld(Key::LeftBracket));
            io.AddKeyEvent(ImGuiKey_Backslash,          inputManager.IsKeyHeld(Key::Backslash));
            io.AddKeyEvent(ImGuiKey_RightBracket,       inputManager.IsKeyHeld(Key::RightBracket));
            io.AddKeyEvent(ImGuiKey_GraveAccent,        inputManager.IsKeyHeld(Key::Grave));
            io.AddKeyEvent(ImGuiKey_CapsLock,           inputManager.IsKeyHeld(Key::CapsLock));
            io.AddKeyEvent(ImGuiKey_ScrollLock,         inputManager.IsKeyHeld(Key::ScrollLock));
            io.AddKeyEvent(ImGuiKey_NumLock,            inputManager.IsKeyHeld(Key::NumpadLock));
            io.AddKeyEvent(ImGuiKey_PrintScreen,        inputManager.IsKeyHeld(Key::PrintScreen));
            io.AddKeyEvent(ImGuiKey_Pause,              inputManager.IsKeyHeld(Key::Pause));
            io.AddKeyEvent(ImGuiKey_Keypad0,            inputManager.IsKeyHeld(Key::KeypadNumber0));
            io.AddKeyEvent(ImGuiKey_Keypad5,            inputManager.IsKeyHeld(Key::KeypadNumber5));
            io.AddKeyEvent(ImGuiKey_KeypadDecimal,      inputManager.IsKeyHeld(Key::KeypadDecimal));
            io.AddKeyEvent(ImGuiKey_KeypadDivide,       inputManager.IsKeyHeld(Key::KeypadDivide));
            io.AddKeyEvent(ImGuiKey_KeypadMultiply,     inputManager.IsKeyHeld(Key::KeypadMultiply));
            io.AddKeyEvent(ImGuiKey_KeypadSubtract,     inputManager.IsKeyHeld(Key::KeypadSubtract));
            io.AddKeyEvent(ImGuiKey_KeypadAdd,          inputManager.IsKeyHeld(Key::KeypadAdd));
            io.AddKeyEvent(ImGuiKey_KeypadEnter,        inputManager.IsKeyHeld(Key::KeypadEnter));
            io.AddKeyEvent(ImGuiKey_KeypadEqual,        inputManager.IsKeyHeld(Key::KeypadEquals));
            io.AddKeyEvent(ImGuiKey_AppBack,            false);
            io.AddKeyEvent(ImGuiKey_AppForward,         false);

            // Update modifier keys
            io.KeyCtrl  = inputManager.IsKeyHeld(Key::LeftControl) || inputManager.IsKeyHeld(Key::RightControl);
            io.KeyShift = inputManager.IsKeyHeld(Key::LeftShift)   || inputManager.IsKeyHeld(Key::RightShift);
            io.KeyAlt   = inputManager.IsKeyHeld(Key::LeftAlt)     || inputManager.IsKeyHeld(Key::RightAlt);
            io.KeySuper = inputManager.IsKeyHeld(Key::LeftSystem)  || inputManager.IsKeyHeld(Key::RightSystem);

            // Handle character entering
            if (inputManager.IsKeyPressed(Key::A)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::A) : std::toupper(GetKeyCharacter(Key::A))); }
            if (inputManager.IsKeyPressed(Key::B)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::B) : std::toupper(GetKeyCharacter(Key::B))); }
            if (inputManager.IsKeyPressed(Key::C)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::C) : std::toupper(GetKeyCharacter(Key::C))); }
            if (inputManager.IsKeyPressed(Key::D)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::D) : std::toupper(GetKeyCharacter(Key::D))); }
            if (inputManager.IsKeyPressed(Key::E)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::E) : std::toupper(GetKeyCharacter(Key::E))); }
            if (inputManager.IsKeyPressed(Key::F)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::F) : std::toupper(GetKeyCharacter(Key::F))); }
            if (inputManager.IsKeyPressed(Key::G)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::G) : std::toupper(GetKeyCharacter(Key::G))); }
            if (inputManager.IsKeyPressed(Key::H)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::H) : std::toupper(GetKeyCharacter(Key::H))); }
            if (inputManager.IsKeyPressed(Key::I)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::I) : std::toupper(GetKeyCharacter(Key::I))); }
            if (inputManager.IsKeyPressed(Key::J)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::J) : std::toupper(GetKeyCharacter(Key::J))); }
            if (inputManager.IsKeyPressed(Key::K)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::K) : std::toupper(GetKeyCharacter(Key::K))); }
            if (inputManager.IsKeyPressed(Key::L)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::L) : std::toupper(GetKeyCharacter(Key::L))); }
            if (inputManager.IsKeyPressed(Key::M)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::M) : std::toupper(GetKeyCharacter(Key::M))); }
            if (inputManager.IsKeyPressed(Key::N)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::N) : std::toupper(GetKeyCharacter(Key::N))); }
            if (inputManager.IsKeyPressed(Key::O)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::O) : std::toupper(GetKeyCharacter(Key::O))); }
            if (inputManager.IsKeyPressed(Key::P)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::P) : std::toupper(GetKeyCharacter(Key::P))); }
            if (inputManager.IsKeyPressed(Key::Q)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::Q) : std::toupper(GetKeyCharacter(Key::Q))); }
            if (inputManager.IsKeyPressed(Key::R)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::R) : std::toupper(GetKeyCharacter(Key::R))); }
            if (inputManager.IsKeyPressed(Key::S)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::S) : std::toupper(GetKeyCharacter(Key::S))); }
            if (inputManager.IsKeyPressed(Key::T)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::T) : std::toupper(GetKeyCharacter(Key::T))); }
            if (inputManager.IsKeyPressed(Key::U)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::U) : std::toupper(GetKeyCharacter(Key::U))); }
            if (inputManager.IsKeyPressed(Key::V)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::V) : std::toupper(GetKeyCharacter(Key::V))); }
            if (inputManager.IsKeyPressed(Key::W)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::W) : std::toupper(GetKeyCharacter(Key::W))); }
            if (inputManager.IsKeyPressed(Key::X)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::X) : std::toupper(GetKeyCharacter(Key::X))); }
            if (inputManager.IsKeyPressed(Key::Y)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::Y) : std::toupper(GetKeyCharacter(Key::Y))); }
            if (inputManager.IsKeyPressed(Key::Z)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::Z) : std::toupper(GetKeyCharacter(Key::Z))); }
            if (inputManager.IsKeyPressed(Key::Space)) { io.AddInputCharacter(!io.KeyShift ? GetKeyCharacter(Key::Space) : std::toupper(GetKeyCharacter(Key::Space))); }

            // Update mouse buttons
            io.MouseDown[ImGuiMouseButton_Left]       = inputManager.IsMouseButtonHeld(MouseButton::Left);
            io.MouseDown[ImGuiMouseButton_Right]      = inputManager.IsMouseButtonHeld(MouseButton::Right);
            io.MouseDown[ImGuiMouseButton_Middle]     = inputManager.IsMouseButtonHeld(MouseButton::Middle);
            io.MouseReleased[ImGuiMouseButton_Left]   = inputManager.IsMouseButtonReleased(MouseButton::Left);
            io.MouseReleased[ImGuiMouseButton_Right]  = inputManager.IsMouseButtonReleased(MouseButton::Right);
            io.MouseReleased[ImGuiMouseButton_Middle] = inputManager.IsMouseButtonReleased(MouseButton::Middle);
            io.MouseWheel                             = inputManager.GetMouseScroll().y / 10.0f;
            io.MouseWheelH                            = inputManager.GetMouseScroll().x / 10.0f;
        }
        if (cursorManagerReference.has_value())
        {
            // Update other mouse data
            const CursorManager &cursorManager = cursorManagerReference.value();
            io.MousePos = !cursorManager.IsCursorHidden() ? ImVec2(cursorManager.GetCursorPosition().x, framebufferSize.y - cursorManager.GetCursorPosition().y) : ImVec2(std::numeric_limits<float32>::min(), std::numeric_limits<float32>::min());
            io.MouseDelta = { cursorManager.GetHorizontalDelta(), -cursorManager.GetVerticalDelta() };
        }
        if (touchManagerReference.has_value())
        {
            const TouchManager &touchManager = touchManagerReference.value();
            if (touchManager.GetTouchCount() > 0)
            {
                io.MousePos = ImVec2(touchManager.GetTouch(0).GetPosition().x, framebufferSize.y - touchManager.GetTouch(0).GetPosition().y);
                if (touchManager.GetTouch(0).GetType() == TouchType::Press) io.MouseDown[ImGuiMouseButton_Left] = true;
                else io.MouseReleased[ImGuiMouseButton_Left] = true;
            }
        }

        // Update display settings
        io.DisplaySize             = { framebufferSize.x, framebufferSize.y };
        io.DisplayFramebufferScale = { scaling, scaling };

        // Update style
        ImGui::GetStyle() = style;

        ImGui::NewFrame();
    }

    void ImGuiRenderTask::Render(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Image> &image)
    {
        ImGui::Render();

        // If ImGui rendered nothing, return, saving GPU time
        const ImDrawData* drawData = ImGui::GetDrawData();
        if (drawData == nullptr || drawData->TotalVtxCount <= 0) return;

        // Begin rendering to image
        commandBuffer->BeginRenderPass(renderPass, { { .image = image } });
        commandBuffer->BeginGraphicsPipeline(pipeline);

        // Bind resources
        commandBuffer->BindVertexBuffer(vertexBuffer);
        commandBuffer->BindIndexBuffer(indexBuffer);

        // Bind perspective settings
        const PushConstant pushConstant
        {
            .fontAtlasIndex = sharedResources.resourceTableFontAtlasIndex,
            .fontSamplerIndex = sharedResources.resourceTableFontSamplerIndex,
            .translation = { -1.0f, 1.0f },
            .scale = { 2.0f / drawData->DisplaySize.x, -2.0f / drawData->DisplaySize.y }
        };
        commandBuffer->PushConstants(&pushConstant, sizeof(PushConstant));

        uint32 vertexOffset = 0;
        uint32 indexOffset = 0;
        for (int i = 0; i < drawData->CmdListsCount; i++)
        {
            const ImDrawList* commandList = drawData->CmdLists[i];

            const uint64 requiredVertexMemorySize = commandList->VtxBuffer.Size * sizeof(ImDrawVert);
            if (requiredVertexMemorySize > vertexBuffer->GetMemorySize())
            {
                // Create new buffer with bigger memory
                std::unique_ptr<Buffer> newVertexBuffer = renderingContext.CreateBuffer({
                    .name = "Vertex Buffer of ImGui Render Task",
                    .memorySize = requiredVertexMemorySize,
                    .usage = BufferUsage::Vertex,
                    .memoryLocation = BufferMemoryLocation::CPU
                });

                // Swap out old buffer for the new one
                commandBuffer->QueueBufferForDestruction(std::move(vertexBuffer));
                vertexBuffer = std::move(newVertexBuffer);

                // Re-bind new buffer
                commandBuffer->BindVertexBuffer(vertexBuffer);
            }

            const uint64 requiredIndexMemorySize = commandList->IdxBuffer.Size * sizeof(ImDrawIdx);
            if (requiredIndexMemorySize > indexBuffer->GetMemorySize())
            {
                // Create new buffer with bigger memory
                std::unique_ptr<Buffer> newIndexBuffer = renderingContext.CreateBuffer({
                    .name = "Index Buffer of ImGui Render Task",
                    .memorySize = requiredIndexMemorySize,
                    .usage = BufferUsage::Index,
                    .memoryLocation = BufferMemoryLocation::CPU
                });

                // Swap out old buffer for the new one
                commandBuffer->QueueBufferForDestruction(std::move(indexBuffer));
                indexBuffer = std::move(newIndexBuffer);

                // Re-bind new buffer
                commandBuffer->BindIndexBuffer(indexBuffer);
            }

            // Copy buffer data to global buffers
            vertexBuffer->CopyFromMemory(commandList->VtxBuffer.Data, requiredVertexMemorySize, 0, vertexOffset * sizeof(ImDrawVert));
            indexBuffer->CopyFromMemory(commandList->IdxBuffer.Data, requiredIndexMemorySize, 0, indexOffset * sizeof(ImDrawIdx));

            for (int j = 0; j < commandList->CmdBuffer.Size; j++)
            {
                const ImDrawCmd &drawCommand = commandList->CmdBuffer[j];

                // Configure scissor
                Vector4UInt scissor = Vector4UInt(
                    (drawCommand.ClipRect.x - drawData->DisplayPos.x) * drawData->FramebufferScale.x,
                    (drawCommand.ClipRect.y - drawData->DisplayPos.y) * drawData->FramebufferScale.y,
                    (drawCommand.ClipRect.z - drawData->DisplayPos.x) * drawData->FramebufferScale.x,
                    (drawCommand.ClipRect.w - drawData->DisplayPos.y) * drawData->FramebufferScale.y
                );

                scissor.z -= scissor.x;
                scissor.w -= scissor.y;

                // Apply scissor
                commandBuffer->SetScissor(scissor);

                // Issue draw call
                commandBuffer->DrawIndexed(drawCommand.ElemCount, drawCommand.IdxOffset + indexOffset, drawCommand.VtxOffset + vertexOffset);
            }

            // Increment global indices
            vertexOffset += commandList->VtxBuffer.Size;
            indexOffset += commandList->IdxBuffer.Size;
        }

        // End rendering
        commandBuffer->EndGraphicsPipeline(pipeline);
        commandBuffer->EndRenderPass(renderPass);
    }

    void ImGuiRenderTask::Resize(const uint32 width, const uint32 height)
    {
        framebufferSize = { width, height };
        renderPass->Resize(width, height);
    }

}