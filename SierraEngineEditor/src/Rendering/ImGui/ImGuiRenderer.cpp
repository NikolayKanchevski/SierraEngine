//
// Created by Nikolay Kanchevski on 28.01.24.
//

#include "ImGuiRenderer.h"

#include "Shaders/ImGuiShader.h"

namespace SierraEngine
{

    namespace
    {
        constexpr Sierra::ImageUsage TEXTURE_USAGE = Sierra::ImageUsage::Sample | Sierra::ImageUsage::DestinationMemory;

        constexpr std::pair<Sierra::Key, ImGuiKey> KEY_MAP[]
        {
            { Sierra::Key::Tab,             ImGuiKey_Tab },
            { Sierra::Key::LeftArrow,       ImGuiKey_LeftArrow },
            { Sierra::Key::RightArrow,      ImGuiKey_RightArrow },
            { Sierra::Key::UpArrow,         ImGuiKey_UpArrow },
            { Sierra::Key::DownArrow,       ImGuiKey_DownArrow },
            { Sierra::Key::PageUp,          ImGuiKey_PageUp },
            { Sierra::Key::PageDown,        ImGuiKey_PageDown },
            { Sierra::Key::Home,            ImGuiKey_Home },
            { Sierra::Key::End,             ImGuiKey_End },
            { Sierra::Key::Insert,          ImGuiKey_Insert },
            { Sierra::Key::Delete,          ImGuiKey_Delete },
            { Sierra::Key::Backspace,       ImGuiKey_Backspace },
            { Sierra::Key::Space,           ImGuiKey_Space },
            { Sierra::Key::Enter,           ImGuiKey_Enter },
            { Sierra::Key::Escape,          ImGuiKey_Escape },
            { Sierra::Key::LeftControl,     ImGuiKey_LeftCtrl },
            { Sierra::Key::LeftShift,       ImGuiKey_LeftShift },
            { Sierra::Key::LeftAlt,         ImGuiKey_LeftAlt },
            { Sierra::Key::LeftSystem,      ImGuiKey_LeftSuper },
            { Sierra::Key::RightControl,    ImGuiKey_RightCtrl },
            { Sierra::Key::RightShift,      ImGuiKey_RightShift },
            { Sierra::Key::RightAlt,        ImGuiKey_RightAlt },
            { Sierra::Key::RightSystem,     ImGuiKey_RightSuper },
            { Sierra::Key::Menu,            ImGuiKey_Menu },
            { Sierra::Key::Number0,         ImGuiKey_0 },
            { Sierra::Key::Number1,         ImGuiKey_1 },
            { Sierra::Key::Number2,         ImGuiKey_2 },
            { Sierra::Key::Number3,         ImGuiKey_3 },
            { Sierra::Key::Number4,         ImGuiKey_4 },
            { Sierra::Key::Number5,         ImGuiKey_5 },
            { Sierra::Key::Number6,         ImGuiKey_6 },
            { Sierra::Key::Number7,         ImGuiKey_7 },
            { Sierra::Key::Number8,         ImGuiKey_8 },
            { Sierra::Key::Number9,         ImGuiKey_9 },
            { Sierra::Key::A,               ImGuiKey_A },
            { Sierra::Key::B,               ImGuiKey_B },
            { Sierra::Key::C,               ImGuiKey_C },
            { Sierra::Key::D,               ImGuiKey_D },
            { Sierra::Key::E,               ImGuiKey_E },
            { Sierra::Key::F,               ImGuiKey_F },
            { Sierra::Key::G,               ImGuiKey_G },
            { Sierra::Key::H,               ImGuiKey_H },
            { Sierra::Key::I,               ImGuiKey_I },
            { Sierra::Key::J,               ImGuiKey_J },
            { Sierra::Key::K,               ImGuiKey_K },
            { Sierra::Key::L,               ImGuiKey_L },
            { Sierra::Key::M,               ImGuiKey_M },
            { Sierra::Key::N,               ImGuiKey_N },
            { Sierra::Key::O,               ImGuiKey_O },
            { Sierra::Key::P,               ImGuiKey_P },
            { Sierra::Key::Q,               ImGuiKey_Q },
            { Sierra::Key::R,               ImGuiKey_R },
            { Sierra::Key::S,               ImGuiKey_S },
            { Sierra::Key::T,               ImGuiKey_T },
            { Sierra::Key::U,               ImGuiKey_U },
            { Sierra::Key::V,               ImGuiKey_V },
            { Sierra::Key::W,               ImGuiKey_W },
            { Sierra::Key::X,               ImGuiKey_X },
            { Sierra::Key::Y,               ImGuiKey_Y },
            { Sierra::Key::Z,               ImGuiKey_Z },
            { Sierra::Key::F1,              ImGuiKey_F1 },
            { Sierra::Key::F2,              ImGuiKey_F2 },
            { Sierra::Key::F3,              ImGuiKey_F3 },
            { Sierra::Key::F4,              ImGuiKey_F4 },
            { Sierra::Key::F5,              ImGuiKey_F5 },
            { Sierra::Key::F6,              ImGuiKey_F6 },
            { Sierra::Key::F7,              ImGuiKey_F7 },
            { Sierra::Key::F8,              ImGuiKey_F8 },
            { Sierra::Key::F9,              ImGuiKey_F9 },
            { Sierra::Key::F10,             ImGuiKey_F10 },
            { Sierra::Key::F11,             ImGuiKey_F11 },
            { Sierra::Key::F12,             ImGuiKey_F12 },
            { Sierra::Key::F13,             ImGuiKey_F13 },
            { Sierra::Key::F14,             ImGuiKey_F14 },
            { Sierra::Key::F15,             ImGuiKey_F15 },
            { Sierra::Key::F16,             ImGuiKey_F16 },
            { Sierra::Key::F17,             ImGuiKey_F17 },
            { Sierra::Key::F18,             ImGuiKey_F18 },
            { Sierra::Key::F19,             ImGuiKey_F19 },
            { Sierra::Key::F20,             ImGuiKey_F20 },
            { Sierra::Key::F21,             ImGuiKey_F21 },
            { Sierra::Key::F22,             ImGuiKey_F22 },
            { Sierra::Key::F23,             ImGuiKey_F23 },
            { Sierra::Key::F24,             ImGuiKey_F24 },
            { Sierra::Key::Apostrophe,      ImGuiKey_Apostrophe },
            { Sierra::Key::Comma,           ImGuiKey_Comma },
            { Sierra::Key::Minus,           ImGuiKey_Minus },
            { Sierra::Key::Period,          ImGuiKey_Period },
            { Sierra::Key::Slash,           ImGuiKey_Slash },
            { Sierra::Key::Semicolon,       ImGuiKey_Semicolon },
            { Sierra::Key::Equal,           ImGuiKey_Equal },
            { Sierra::Key::LeftBracket,     ImGuiKey_LeftBracket },
            { Sierra::Key::Backslash,       ImGuiKey_Backslash },
            { Sierra::Key::RightBracket,    ImGuiKey_RightBracket },
            { Sierra::Key::Grave,           ImGuiKey_GraveAccent },
            { Sierra::Key::CapsLock,        ImGuiKey_CapsLock },
            { Sierra::Key::ScrollLock,      ImGuiKey_ScrollLock },
            { Sierra::Key::NumpadLock,      ImGuiKey_NumLock },
            { Sierra::Key::PrintScreen,     ImGuiKey_PrintScreen },
            { Sierra::Key::Pause,           ImGuiKey_Pause },
            { Sierra::Key::KeypadNumber0,   ImGuiKey_Keypad0 },
            { Sierra::Key::KeypadNumber5,   ImGuiKey_Keypad5 },
            { Sierra::Key::KeypadPeriod,    ImGuiKey_KeypadDecimal },
            { Sierra::Key::KeypadSlash,     ImGuiKey_KeypadDivide },
            { Sierra::Key::KeypadAsterisk,  ImGuiKey_KeypadMultiply },
            { Sierra::Key::KeypadMinus,     ImGuiKey_KeypadSubtract },
            { Sierra::Key::KeypadPlus,      ImGuiKey_KeypadAdd },
            { Sierra::Key::KeypadEnter,     ImGuiKey_KeypadEnter },
            { Sierra::Key::KeypadEqual,     ImGuiKey_KeypadEqual }
        };

        // Render state is fully set for every draw call, so there is never anything to reset
        void ResetRenderStateCallback(const ImDrawList*, const ImDrawCmd*)
        {

        }

        void ReserveBuffer(const DrawFrame& drawFrame, std::unique_ptr<Sierra::Buffer>& buffer, const uint64 requiredMemorySize, const Sierra::BufferUsage usage, const std::string_view name)
        {
            const uint64 currentMemorySize = buffer != nullptr ? buffer->GetMemorySize() : 0;
            if (requiredMemorySize <= currentMemorySize)
                return;

            std::unique_ptr<Sierra::Buffer> newBuffer = drawFrame.device.CreateBuffer({
                .name = name,
                .memorySize = glm::max(requiredMemorySize, currentMemorySize * 2),
                .usage = usage,
                .memoryLocation = Sierra::BufferMemoryLocation::RAM
            });

            if (buffer != nullptr)
            {
                drawFrame.destructionScheduler.QueueResource(std::move(buffer));
            }
            buffer = std::move(newBuffer);
        }

        void UploadTexturePixels(const DrawFrame& drawFrame, ImTextureData& textureData, const Sierra::Image& image, const ImTextureRect& rect, const bool initialUpload)
        {
            if (rect.w == 0 || rect.h == 0)
                return;

            const uint64 rowMemorySize = static_cast<uint64>(rect.w) * textureData.BytesPerPixel;
            std::unique_ptr<Sierra::Buffer> stagingBuffer = drawFrame.device.CreateBuffer({
                .name = "ImGui Texture Staging Buffer",
                .memorySize = rowMemorySize * rect.h,
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::RAM
            });

            // ImGui keeps every texture's pixels on the CPU, so only the rows of the rectangle are staged, tightly packed
            if (rect.w == textureData.Width)
            {
                stagingBuffer->Write(0, { static_cast<const uint8*>(textureData.GetPixelsAt(0, rect.y)), rowMemorySize * rect.h });
            }
            else
            {
                for (uint32 row = 0; row < rect.h; row++)
                {
                    stagingBuffer->Write(row * rowMemorySize, { static_cast<const uint8*>(textureData.GetPixelsAt(rect.x, rect.y + row)), rowMemorySize });
                }
            }

            drawFrame.commandBuffer.SynchronizeImageUsage(image, { .previousUsage = initialUpload ? Sierra::ImageCommandUsage::None : Sierra::ImageCommandUsage::GraphicsRead, .nextUsage = Sierra::ImageCommandUsage::MemoryWrite });
            drawFrame.commandBuffer.CopyBufferToImage(*stagingBuffer, image, { .destinationPixelOffset = { rect.x, rect.y, 0 }, .pixelRange = { rect.w, rect.h, 1 } });
            drawFrame.commandBuffer.SynchronizeImageUsage(image, { .previousUsage = Sierra::ImageCommandUsage::MemoryWrite, .nextUsage = Sierra::ImageCommandUsage::GraphicsRead });

            drawFrame.destructionScheduler.QueueResource(std::move(stagingBuffer));
        }
    }

    /* --- CONSTRUCTORS --- */

    ImGuiRenderer::ImGuiRenderer(const ImGuiRendererCreateInfo& createInfo)
        : resourceTable(createInfo.resourceTable), concurrentFrameCount(createInfo.concurrentFrameCount)
    {
        APP_THROW_IF(createInfo.fontCreateInfos.empty(), Sierra::InvalidValueError("Cannot create ImGui renderer, as specified fonts must not be empty"));
        APP_THROW_IF(createInfo.concurrentFrameCount == 0, Sierra::InvalidValueError("Cannot create ImGui renderer, as specified concurrent frame count must be greater than [0]"));

        context = ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | (SR_PLATFORM_MOBILE ? ImGuiConfigFlags_IsTouchScreen : ImGuiConfigFlags_None);
        io.BackendFlags = ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasTextures;
        io.BackendPlatformName = "Sierra";
        io.BackendRendererName = "Sierra";
        io.MouseSingleClickDelay = 0.6f;
        io.MouseDoubleClickTime = 0.5f;
        io.KeyRepeatRate = 0.2f;
        io.KeyRepeatDelay = 0.3f;

        ImGui::GetPlatformIO().DrawCallback_ResetRenderState = ResetRenderStateCallback;
        io.Fonts->TexDesiredFormat = createInfo.device.IsImageFormatSupported(Sierra::ImageFormat::R8_UNorm, TEXTURE_USAGE) ? ImTextureFormat_Alpha8 : ImTextureFormat_RGBA32;

        // Glyphs are rasterized on demand, at the scale of the display, so fonts only need their logical size
        ImFontConfig fontConfig = { };
        fontConfig.FontDataOwnedByAtlas = false;
        for (const ImGuiFontCreateInfo& fontCreateInfo : createInfo.fontCreateInfos)
        {
            const ImFont* font = io.Fonts->AddFontFromMemoryTTF(const_cast<uint8*>(fontCreateInfo.ttfBuffer.data()), static_cast<int>(fontCreateInfo.ttfBuffer.size_bytes()), fontCreateInfo.size, &fontConfig);
            APP_THROW_IF(font == nullptr, Sierra::InvalidValueError("Cannot create ImGui renderer, as a specified font could not be loaded"));

            fontConfig.MergeMode = true;
        }

        renderPass = createInfo.device.CreateRenderPass({
            .name = "ImGui Render Pass",
            .attachments = {
                {{
                    .format = createInfo.format
                }}
            },
            .subpassDescriptions = {
                {{ .renderTargets = {{ 0 }} }}
            }
        });

        constexpr std::span<const uint8> SHADER_BLOB = IMGUI_RENDERER_SHADER_BLOB;
        std::span<const uint8> vertexCode = { };
        std::span<const uint8> fragmentCode = { };

        switch (createInfo.device.GetBackendType())
        {
            case Sierra::RenderingBackendType::Vulkan:
            {
                vertexCode = SHADER_BLOB.subspan(IMGUI_VERTEX_SHADER_SPIRV_OFFSET, IMGUI_VERTEX_SHADER_SPIRV_MEMORY_SIZE);
                fragmentCode = SHADER_BLOB.subspan(IMGUI_FRAGMENT_SHADER_SPIRV_OFFSET, IMGUI_FRAGMENT_SHADER_SPIRV_MEMORY_SIZE);
                break;
            }
            case Sierra::RenderingBackendType::Metal:
            {
                #if SR_PLATFORM_iOS
                    vertexCode = SHADER_BLOB.subspan(IMGUI_VERTEX_SHADER_IOS_METAL_SOURCE_OFFSET, IMGUI_VERTEX_SHADER_IOS_METAL_SOURCE_MEMORY_SIZE);
                    fragmentCode = SHADER_BLOB.subspan(IMGUI_FRAGMENT_SHADER_IOS_METAL_SOURCE_OFFSET, IMGUI_FRAGMENT_SHADER_IOS_METAL_SOURCE_MEMORY_SIZE);
                #else
                    vertexCode = SHADER_BLOB.subspan(IMGUI_VERTEX_SHADER_MACOS_METAL_SOURCE_OFFSET, IMGUI_VERTEX_SHADER_MACOS_METAL_SOURCE_MEMORY_SIZE);
                    fragmentCode = SHADER_BLOB.subspan(IMGUI_FRAGMENT_SHADER_MACOS_METAL_SOURCE_OFFSET, IMGUI_FRAGMENT_SHADER_MACOS_METAL_SOURCE_MEMORY_SIZE);
                #endif
                break;
            }
            default:
            {
                break;
            }
        }

        const std::unique_ptr<Sierra::Shader> vertexShader = createInfo.device.CreateShader({
            .name = "ImGui Vertex Shader",
            .shaderType = Sierra::ShaderType::Vertex,
            .code = vertexCode
        });

        const std::unique_ptr<Sierra::Shader> fragmentShader = createInfo.device.CreateShader({
            .name = "ImGui Fragment Shader",
            .shaderType = Sierra::ShaderType::Fragment,
            .code = fragmentCode
        });

        pipeline = createInfo.device.CreateGraphicsPipeline({
            .name = "ImGui Pipeline",
            .vertexInputs = {{ Sierra::VertexInput::Position_2D, Sierra::VertexInput::UV, Sierra::VertexInput::Color }},
            .vertexShader = *vertexShader,
            .fragmentShader = fragmentShader.get(),
            .pushConstantSize = sizeof(PushConstant),
            .templateRenderPass = *renderPass,
            .blendMode = Sierra::BlendMode::Alpha,
            .cullMode = Sierra::CullMode::None
        });

        vertexBuffers.resize(concurrentFrameCount);
        indexBuffers.resize(concurrentFrameCount);

        sampler = createInfo.device.CreateSampler({ .name = "ImGui Sampler", .magFilter = Sierra::SamplerFilter::Linear, .minFilter = Sierra::SamplerFilter::Linear });
        samplerID = resourceTable.BindSampler(*sampler);
    }

    /* --- POLLING METHODS --- */

    void ImGuiRenderer::Update(const ApplicationFrame& applicationFrame, const Vector2UInt framebufferSize, const float32 scaling) const
    {
        // UI is laid out in logical units, so it is the same size on every display. The framebuffer is those units scaled up, which is where glyphs get rasterized
        const float32 width = static_cast<float32>(framebufferSize.x) / scaling;
        const float32 height = static_cast<float32>(framebufferSize.y) / scaling;

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = { width, height };
        io.DisplayFramebufferScale = { scaling, scaling };

        // Times double clicks, key repeat, tooltips and animations (ImGui assumes 1/60 of a second otherwise), and must be positive
        if (const float32 deltaTime = applicationFrame.timeStep; deltaTime > 0.0f)
        {
            io.DeltaTime = deltaTime;
        }

        // Sierra's window coordinates start from the bottom-left corner, while ImGui's start from the top-left one
        const auto ToImGuiPosition = [scaling, height](const float32 x, const float32 y) -> ImVec2 { return { x / scaling, height - y / scaling }; };

        if (const Sierra::InputManager* inputManager = applicationFrame.inputManager; inputManager != nullptr)
        {
            for (const auto [key, imGuiKey] : KEY_MAP)
            {
                io.AddKeyEvent(imGuiKey, inputManager->IsKeyHeld(key));
            }

            // Modifiers are tracked separately from their keys, and are what shortcuts and multi-selection check (on macOS, ImGui swaps Ctrl and Cmd itself)
            io.AddKeyEvent(ImGuiMod_Ctrl, inputManager->IsKeyHeld(Sierra::Key::LeftControl) || inputManager->IsKeyHeld(Sierra::Key::RightControl));
            io.AddKeyEvent(ImGuiMod_Shift, inputManager->IsKeyHeld(Sierra::Key::LeftShift) || inputManager->IsKeyHeld(Sierra::Key::RightShift));
            io.AddKeyEvent(ImGuiMod_Alt, inputManager->IsKeyHeld(Sierra::Key::LeftAlt) || inputManager->IsKeyHeld(Sierra::Key::RightAlt));
            io.AddKeyEvent(ImGuiMod_Super, inputManager->IsKeyHeld(Sierra::Key::LeftSystem) || inputManager->IsKeyHeld(Sierra::Key::RightSystem));

            // Sierra has no text input events, so characters are derived from the keys pressed this frame
            const bool shiftHeld = inputManager->IsKeyHeld(Sierra::Key::LeftShift) || inputManager->IsKeyHeld(Sierra::Key::RightShift);
            for (uint8 keyCode = static_cast<uint8>(Sierra::Key::A); keyCode < static_cast<uint8>(Sierra::Key::RightSystem); keyCode++)
            {
                const Sierra::Key key = static_cast<Sierra::Key>(keyCode);
                if (!inputManager->IsKeyPressed(key))
                    continue;

                if (const char character = Sierra::KeyToCharacter(key); character != '\0')
                {
                    io.AddInputCharacter(shiftHeld ? std::toupper(character) : character);
                }
            }
        }

        // A touch takes over the pointer for as long as it lasts
        if (const Sierra::TouchManager* touchManager = applicationFrame.touchManager; touchManager != nullptr && !touchManager->GetTouches().empty())
        {
            const Sierra::Touch& touch = touchManager->GetTouches().front();
            const ImVec2 position = ToImGuiPosition(touch.GetPosition().x, touch.GetPosition().y);

            io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
            io.AddMousePosEvent(position.x, position.y);
            io.AddMouseButtonEvent(ImGuiMouseButton_Left, touch.GetType() == Sierra::TouchType::Press);
        }
        else
        {
            io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);

            if (const Sierra::CursorManager* cursorManager = applicationFrame.cursorManager; cursorManager != nullptr)
            {
                if (cursorManager->IsCursorVisible())
                {
                    const ImVec2 position = ToImGuiPosition(static_cast<float32>(cursorManager->GetCursorPosition().x), static_cast<float32>(cursorManager->GetCursorPosition().y));
                    io.AddMousePosEvent(position.x, position.y);
                }
                else
                {
                    // The value ImGui reserves for "no mouse"
                    io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
                }
            }

            if (const Sierra::InputManager* inputManager = applicationFrame.inputManager; inputManager != nullptr)
            {
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, inputManager->IsMouseButtonHeld(Sierra::MouseButton::Left));
                io.AddMouseButtonEvent(ImGuiMouseButton_Right, inputManager->IsMouseButtonHeld(Sierra::MouseButton::Right));
                io.AddMouseButtonEvent(ImGuiMouseButton_Middle, inputManager->IsMouseButtonHeld(Sierra::MouseButton::Middle));
                io.AddMouseWheelEvent(inputManager->GetMouseScroll().x / 10.0f, inputManager->GetMouseScroll().y / 100.0f);
            }
        }

        ImGui::NewFrame();
    }

    void ImGuiRenderer::Render(const DrawFrame& drawFrame, const Sierra::Framebuffer& framebuffer)
    {
        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();

        const uint32 frameIndex = currentFrame;
        currentFrame = (currentFrame + 1) % concurrentFrameCount;

        // Texture requests are honoured even when nothing gets drawn, as ImGui waits for them to be acknowledged
        if (drawData->Textures != nullptr)
        {
            for (ImTextureData* textureData : *drawData->Textures)
            {
                if (textureData->Status != ImTextureStatus_OK)
                {
                    UpdateTexture(drawFrame, *textureData);
                }
            }
        }

        // Skip drawing if there is nothing to draw, or if the window is minimized
        const ImVec2 framebufferSize = drawData->DisplaySize * drawData->FramebufferScale;
        if (!drawData->Valid || drawData->TotalVtxCount <= 0 || framebufferSize.x <= 0.0f || framebufferSize.y <= 0.0f)
        {
            return;
        }

        std::unique_ptr<Sierra::Buffer>& vertexBuffer = vertexBuffers[frameIndex];
        std::unique_ptr<Sierra::Buffer>& indexBuffer = indexBuffers[frameIndex];
        ReserveBuffer(drawFrame, vertexBuffer, static_cast<uint64>(drawData->TotalVtxCount) * sizeof(ImDrawVert), Sierra::BufferUsage::Vertex, "ImGui Vertex Buffer");
        ReserveBuffer(drawFrame, indexBuffer, static_cast<uint64>(drawData->TotalIdxCount) * sizeof(ImDrawIdx), Sierra::BufferUsage::Index, "ImGui Index Buffer");

        drawFrame.commandBuffer.BeginRenderPass(*renderPass, framebuffer);
        drawFrame.commandBuffer.BeginGraphicsPipeline(*pipeline);
        drawFrame.commandBuffer.BindVertexBuffer(*vertexBuffer, 0);
        drawFrame.commandBuffer.BindIndexBuffer(*indexBuffer, 0);

        PushConstant pushConstant
        {
            .samplerIndex = samplerID.GetValue(),
            .scale = { 2.0f / drawData->DisplaySize.x, -2.0f / drawData->DisplaySize.y }
        };

        uint32 vertexOffset = 0;
        uint32 indexOffset = 0;
        for (const ImDrawList* drawList : drawData->CmdLists)
        {
            // Geometry of all draw lists is packed into this frame's buffers, one after another
            vertexBuffer->Write(static_cast<uint64>(vertexOffset) * sizeof(ImDrawVert), { reinterpret_cast<const uint8*>(drawList->VtxBuffer.Data), static_cast<size>(drawList->VtxBuffer.size_in_bytes()) });
            indexBuffer->Write(static_cast<uint64>(indexOffset) * sizeof(ImDrawIdx), { reinterpret_cast<const uint8*>(drawList->IdxBuffer.Data), static_cast<size>(drawList->IdxBuffer.size_in_bytes()) });

            for (const ImDrawCmd& drawCommand : drawList->CmdBuffer)
            {
                if (drawCommand.UserCallback != nullptr)
                {
                    drawCommand.UserCallback(drawList, &drawCommand);
                    continue;
                }

                // Clip rectangles are in UI space, and may reach past the framebuffer, which a scissor must not
                const ImVec2 clipMin = ImMax((ImVec2(drawCommand.ClipRect.x, drawCommand.ClipRect.y) - drawData->DisplayPos) * drawData->FramebufferScale, ImVec2(0.0f, 0.0f));
                const ImVec2 clipMax = ImMin((ImVec2(drawCommand.ClipRect.z, drawCommand.ClipRect.w) - drawData->DisplayPos) * drawData->FramebufferScale, framebufferSize);
                if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
                    continue;

                drawFrame.commandBuffer.SetScissor(Vector4UInt(clipMin.x, clipMin.y, clipMax.x - clipMin.x, clipMax.y - clipMin.y));

                pushConstant.textureIndex = static_cast<uint32>(drawCommand.GetTexID());
                drawFrame.commandBuffer.PushConstantsAs(pushConstant);

                drawFrame.commandBuffer.DrawIndexed(drawCommand.ElemCount, drawCommand.IdxOffset + indexOffset, drawCommand.VtxOffset + vertexOffset);
            }

            vertexOffset += drawList->VtxBuffer.Size;
            indexOffset += drawList->IdxBuffer.Size;
        }

        drawFrame.commandBuffer.EndGraphicsPipeline();
        drawFrame.commandBuffer.EndRenderPass();
    }

    /* --- PRIVATE METHODS --- */

    void ImGuiRenderer::UpdateTexture(const DrawFrame& drawFrame, ImTextureData& textureData)
    {
        switch (textureData.Status)
        {
            case ImTextureStatus_WantCreate:
            {
                const bool alphaOnly = textureData.Format == ImTextureFormat_Alpha8;

                Sierra::ImageCreateInfo imageCreateInfo
                {
                    .name = "ImGui Texture",
                    .width = static_cast<uint32>(textureData.Width),
                    .height = static_cast<uint32>(textureData.Height),
                    .format = alphaOnly ? Sierra::ImageFormat::R8_UNorm : Sierra::ImageFormat::R8G8B8A8_UNorm,
                    .usage = TEXTURE_USAGE,
                    .memoryLocation = Sierra::ImageMemoryLocation::GPU
                };

                if (alphaOnly)
                {
                    // Only coverage is stored, so it is sampled as white with that coverage as alpha
                    std::ranges::copy(std::array { Sierra::ImageChannelSwizzling::One, Sierra::ImageChannelSwizzling::One, Sierra::ImageChannelSwizzling::One, Sierra::ImageChannelSwizzling::Red }, imageCreateInfo.swizzling);
                }

                std::unique_ptr<Texture> texture = std::make_unique<Texture>();
                texture->image = drawFrame.device.CreateImage(imageCreateInfo);

                const ImTextureRect wholeRect = { 0, 0, static_cast<unsigned short>(textureData.Width), static_cast<unsigned short>(textureData.Height) };
                UploadTexturePixels(drawFrame, textureData, *texture->image, wholeRect, true);

                texture->ID = resourceTable.BindSampledImage(*texture->image);
                textureData.SetTexID(texture->ID.GetValue());
                textureData.BackendUserData = texture.get();
                textureData.SetStatus(ImTextureStatus_OK);

                textures.push_back(std::move(texture));
                break;
            }
            case ImTextureStatus_WantUpdates:
            {
                // ImGui only ever writes to regions no draw call has sampled yet, so frames in flight are unaffected
                const Texture& texture = *static_cast<const Texture*>(textureData.BackendUserData);
                UploadTexturePixels(drawFrame, textureData, *texture.image, textureData.UpdateRect, false);

                textureData.SetStatus(ImTextureStatus_OK);
                break;
            }
            case ImTextureStatus_WantDestroy:
            {
                // Its slot in the resource table is freed right away, so every frame that could still be sampling it must have finished first
                if (textureData.UnusedFrames < static_cast<int>(concurrentFrameCount))
                    break;

                DestroyTexture(textureData, &drawFrame.destructionScheduler);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void ImGuiRenderer::DestroyTexture(ImTextureData& textureData, Sierra::DestructionScheduler* const destructionScheduler)
    {
        Texture* const texture = static_cast<Texture*>(textureData.BackendUserData);
        if (texture == nullptr)
            return;

        resourceTable.FreeSampledImage(texture->ID);
        if (destructionScheduler != nullptr)
        {
            destructionScheduler->QueueResource(std::move(texture->image));
        }
        std::erase_if(textures, [texture](const std::unique_ptr<Texture>& ownedTexture) -> bool { return ownedTexture.get() == texture; });

        textureData.SetTexID(ImTextureID_Invalid);
        textureData.BackendUserData = nullptr;
        textureData.SetStatus(ImTextureStatus_Destroyed);
    }

    /* --- DESTRUCTOR --- */

    ImGuiRenderer::~ImGuiRenderer() noexcept
    {
        // The GPU is idle by the time the renderer is destroyed, so textures can be released immediately
        for (ImTextureData* textureData : ImGui::GetPlatformIO().Textures)
        {
            DestroyTexture(*textureData, nullptr);
        }
        resourceTable.FreeSampler(samplerID);

        ImGui::DestroyContext(context);
    }

}
