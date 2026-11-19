//
// Created by Nikolay Kanchevski on 18.11.26.
//

#include "SlangShaderCompiler.h"

#include <slang.h>
#include <slang-com-ptr.h>
#include <spirv_msl.hpp>

namespace SierraEngine
{

    namespace
    {
        // Must match the bindless layout of Sierra.slang and the resource tables of the Metal backend (MetalResourceTable.h and MetalDevice.h)
        constexpr uint32 BINDLESS_SET = 0;
        constexpr uint32 PUSH_CONSTANT_BUFFER_INDEX = 1;

        constexpr uint32 UNIFORM_BUFFER_CAPACITY = 8192;
        constexpr uint32 STORAGE_BUFFER_CAPACITY = 8192;
        constexpr uint32 SAMPLED_IMAGE_CAPACITY = 8192;
        constexpr uint32 STORAGE_IMAGE_CAPACITY = 8192;
        constexpr uint32 SAMPLER_CAPACITY = 1024;

        constexpr std::array<spirv_cross::MSLResourceBinding, 5> BINDLESS_BINDINGS
        {
            spirv_cross::MSLResourceBinding { .basetype = spirv_cross::SPIRType::Void, .desc_set = BINDLESS_SET, .binding = 0, .count = UNIFORM_BUFFER_CAPACITY },
            spirv_cross::MSLResourceBinding { .basetype = spirv_cross::SPIRType::Void, .desc_set = BINDLESS_SET, .binding = 1, .count = STORAGE_BUFFER_CAPACITY },
            spirv_cross::MSLResourceBinding { .basetype = spirv_cross::SPIRType::SampledImage, .desc_set = BINDLESS_SET, .binding = 2, .count = SAMPLED_IMAGE_CAPACITY },
            spirv_cross::MSLResourceBinding { .basetype = spirv_cross::SPIRType::Image, .desc_set = BINDLESS_SET, .binding = 3, .count = STORAGE_IMAGE_CAPACITY },
            spirv_cross::MSLResourceBinding { .basetype = spirv_cross::SPIRType::Sampler, .desc_set = BINDLESS_SET, .binding = 4, .count = SAMPLER_CAPACITY }
        };

        enum class MetalPlatform : bool
        {
            macOS,
            iOS
        };

        struct SlangSource
        {
            std::string filePath = { };
            std::string moduleName = { };
            std::string code = { };
        };

        [[nodiscard]] std::string_view BlobToString(slang::IBlob* blob) noexcept
        {
            if (blob == nullptr) return { };
            return { static_cast<const char*>(blob->getBufferPointer()), blob->getBufferSize() };
        }

        [[nodiscard]] Sierra::ShaderType SlangStageToShaderType(const SlangStage stage) noexcept
        {
            switch (stage)
            {
                case SLANG_STAGE_VERTEX:        return Sierra::ShaderType::Vertex;
                case SLANG_STAGE_FRAGMENT:      return Sierra::ShaderType::Fragment;
                case SLANG_STAGE_COMPUTE:       return Sierra::ShaderType::Compute;
                default:                        break;
            }

            return Sierra::ShaderType::Undefined;
        }

        void LogDiagnostics(slang::IBlob* diagnostics, const std::string_view sourceFilePath)
        {
            if (const std::string_view text = BlobToString(diagnostics); !text.empty())
            {
                APP_WARNING("Shader [{0}] compiled with diagnostics: {1}", sourceFilePath, text);
            }
        }

        [[nodiscard]] spirv_cross::CompilerMSL::Options CreateMetalOptions(const MetalPlatform platform) noexcept
        {
            spirv_cross::CompilerMSL::Options options = { };
            options.platform = platform == MetalPlatform::macOS ? spirv_cross::CompilerMSL::Options::macOS : spirv_cross::CompilerMSL::Options::iOS;
            options.msl_version = spirv_cross::CompilerMSL::Options::make_msl_version(3, 0);

            // Indices of buffers the translation may add itself, kept clear of the push constant (1) and vertex (30) buffers, and the bindless argument buffer (0)
            options.texel_buffer_texture_width = 4096;
            options.r32ui_linear_texture_alignment = 4;
            options.r32ui_alignment_constant_id = 65535;
            options.swizzle_buffer_index = 29;
            options.indirect_params_buffer_index = 28;
            options.shader_output_buffer_index = 27;
            options.shader_patch_output_buffer_index = 26;
            options.shader_tess_factor_buffer_index = 25;
            options.buffer_size_buffer_index = 24;
            options.view_mask_buffer_index = 23;
            options.dynamic_offsets_buffer_index = 22;
            options.shader_input_buffer_index = 21;
            options.shader_index_buffer_index = 20;
            options.shader_patch_input_buffer_index = 19;

            options.enable_point_size_builtin = true;
            options.enable_frag_depth_builtin = true;
            options.enable_frag_stencil_ref_builtin = true;
            options.multiview_layered_rendering = true;
            options.emulate_cube_array = true;
            options.enable_clip_distance_user_varying = true;
            options.ios_use_simdgroup_functions = true;
            options.vertex_index_type = spirv_cross::CompilerMSL::Options::IndexType::UInt32;
            options.manual_helper_invocation_updates = true;
            options.readwrite_texture_fences = true;

            // Every resource of the bindless layout lives in one argument buffer, at fixed indices, whether or not the shader uses it
            options.argument_buffers = true;
            options.argument_buffers_tier = spirv_cross::CompilerMSL::Options::ArgumentBuffersTier::Tier2;
            options.force_active_argument_buffer_resources = true;
            options.pad_argument_buffer_resources = true;

            return options;
        }

        [[nodiscard]] std::optional<std::string> TranslateToMetal(const std::span<const uint32> spirvCode, const MetalPlatform platform, const std::string_view entryPointName, const std::string_view sourceFilePath)
        {
            try
            {
                spirv_cross::CompilerMSL compiler(spirvCode.data(), spirvCode.size());
                compiler.set_msl_options(CreateMetalOptions(platform));

                const spv::ExecutionModel executionModel = compiler.get_execution_model();
                compiler.set_argument_buffer_device_address_space(BINDLESS_SET, true);

                uint32 currentIndex = 0;
                for (spirv_cross::MSLResourceBinding binding : BINDLESS_BINDINGS)
                {
                    binding.stage = executionModel;
                    binding.msl_buffer = currentIndex;
                    binding.msl_texture = currentIndex;
                    binding.msl_sampler = currentIndex;
                    compiler.add_msl_resource_binding(binding);

                    currentIndex += binding.count;
                }

                spirv_cross::MSLResourceBinding pushConstantBinding = { };
                pushConstantBinding.basetype = spirv_cross::SPIRType::Void;
                pushConstantBinding.stage = executionModel;
                pushConstantBinding.desc_set = spirv_cross::kPushConstDescSet;
                pushConstantBinding.binding = spirv_cross::kPushConstBinding;
                pushConstantBinding.msl_buffer = PUSH_CONSTANT_BUFFER_INDEX;
                compiler.add_msl_resource_binding(pushConstantBinding);

                return compiler.compile();
            }
            catch (const spirv_cross::CompilerError& error)
            {
                APP_ERROR("Could not translate entry point [{0}] of Slang shader [{1}] to Metal ({2}): {3}", entryPointName, sourceFilePath, platform == MetalPlatform::macOS ? "macOS" : "iOS", error.what());
                return std::nullopt;
            }
        }

        [[nodiscard]] bool WriteEntryPoint(const std::span<const uint32> spirvCode, ShaderEntryPoint& entryPoint, const std::string_view sourceFilePath, Sierra::WriteStream& outStream)
        {
            const std::optional<std::string> macOSMetalCode = TranslateToMetal(spirvCode, MetalPlatform::macOS, entryPoint.name, sourceFilePath);
            if (!macOSMetalCode.has_value()) return false;

            const std::optional<std::string> iOSMetalCode = TranslateToMetal(spirvCode, MetalPlatform::iOS, entryPoint.name, sourceFilePath);
            if (!iOSMetalCode.has_value()) return false;

            // Allocated up front, as all three parts are written one after another (offsets are taken from where writing is at, as the size already includes them)
            outStream.Resize(outStream.GetSize() + spirvCode.size_bytes() + macOSMetalCode->size() + iOSMetalCode->size());

            entryPoint.spirvCodeOffset = outStream.GetOffset();
            entryPoint.spirvCodeMemorySize = spirvCode.size_bytes();
            if (outStream.Write({ reinterpret_cast<const uint8*>(spirvCode.data()), spirvCode.size_bytes() }) != Sierra::StreamResult::Success)
                return false;

            entryPoint.macOSMetalCodeOffset = outStream.GetOffset();
            entryPoint.macOSMetalCodeMemorySize = macOSMetalCode->size();
            if (outStream.Write({ reinterpret_cast<const uint8*>(macOSMetalCode->data()), macOSMetalCode->size() }) != Sierra::StreamResult::Success)
                return false;

            entryPoint.iOSMetalCodeOffset = outStream.GetOffset();
            entryPoint.iOSMetalCodeMemorySize = iOSMetalCode->size();
            if (outStream.Write({ reinterpret_cast<const uint8*>(iOSMetalCode->data()), iOSMetalCode->size() }) != Sierra::StreamResult::Success)
                return false;

            return true;
        }

        [[nodiscard]] std::optional<std::vector<ShaderEntryPoint>> WriteEntryPoints(slang::ISession& session, const SlangSource& source, Sierra::WriteStream& outStream)
        {
            Slang::ComPtr<slang::IBlob> diagnostics = { };
            slang::IModule* module = session.loadModuleFromSourceString(source.moduleName.c_str(), source.filePath.c_str(), source.code.c_str(), diagnostics.writeRef());
            if (module == nullptr)
            {
                APP_ERROR("Could not compile Slang shader [{0}]: {1}", source.filePath, BlobToString(diagnostics));
                return std::nullopt;
            }
            LogDiagnostics(diagnostics, source.filePath);

            const SlangInt32 entryPointCount = module->getDefinedEntryPointCount();
            if (entryPointCount == 0)
            {
                APP_ERROR("Cannot compile Slang shader [{0}], as it defines no entry points (they are marked with [shader(\"vertex\")], [shader(\"fragment\")] or [shader(\"compute\")])", source.filePath);
                return std::nullopt;
            }

            std::vector<ShaderEntryPoint> entryPoints = { };
            entryPoints.reserve(entryPointCount);

            for (SlangInt32 i = 0; i < entryPointCount; i++)
            {
                Slang::ComPtr<slang::IEntryPoint> slangEntryPoint = { };
                if (SLANG_FAILED(module->getDefinedEntryPoint(i, slangEntryPoint.writeRef())))
                {
                    APP_ERROR("Cannot compile Slang shader [{0}], as retrieving its entry point at index [{1}] failed", source.filePath, i);
                    return std::nullopt;
                }

                Slang::ComPtr<slang::IComponentType> program = { };
                const std::array<slang::IComponentType*, 2> components = { module, slangEntryPoint.get() };
                if (SLANG_FAILED(session.createCompositeComponentType(components.data(), static_cast<SlangInt>(components.size()), program.writeRef(), diagnostics.writeRef())))
                {
                    APP_ERROR("Could not compile Slang shader [{0}]: {1}", source.filePath, BlobToString(diagnostics));
                    return std::nullopt;
                }

                Slang::ComPtr<slang::IComponentType> linkedProgram = { };
                if (SLANG_FAILED(program->link(linkedProgram.writeRef(), diagnostics.writeRef())))
                {
                    APP_ERROR("Could not link Slang shader [{0}]: {1}", source.filePath, BlobToString(diagnostics));
                    return std::nullopt;
                }
                LogDiagnostics(diagnostics, source.filePath);

                Slang::ComPtr<slang::IBlob> code = { };
                if (SLANG_FAILED(linkedProgram->getEntryPointCode(0, 0, code.writeRef(), diagnostics.writeRef())) || code == nullptr)
                {
                    APP_ERROR("Could not generate SPIR-V for Slang shader [{0}]: {1}", source.filePath, BlobToString(diagnostics));
                    return std::nullopt;
                }
                LogDiagnostics(diagnostics, source.filePath);

                // NOTE: The entry point is named [main] within the SPIR-V (as Vulkan expects), and [main0] within the Metal sources, so its source name is only kept here
                slang::EntryPointReflection* const reflection = linkedProgram->getLayout()->getEntryPointByIndex(0);
                ShaderEntryPoint entryPoint = { .name = reflection->getName(), .type = SlangStageToShaderType(reflection->getStage()) };
                if (entryPoint.type == Sierra::ShaderType::Undefined)
                {
                    APP_ERROR("Could not compile Slang shader [{0}], as its entry point [{1}] is of unsupported type", source.filePath, entryPoint.name);
                    return std::nullopt;
                }

                const std::span<const uint32> spirvCode = { static_cast<const uint32*>(code->getBufferPointer()), code->getBufferSize() / sizeof(uint32) };
                if (!WriteEntryPoint(spirvCode, entryPoint, source.filePath, outStream))
                {
                    APP_ERROR("Could not write compiled code of entry point [{0}] of Slang shader [{1}]", entryPoint.name, source.filePath);
                    return std::nullopt;
                }

                entryPoints.emplace_back(std::move(entryPoint));
            }

            return entryPoints;
        }
    }

    /* --- CONSTRUCTORS --- */

    SlangShaderCompiler::SlangShaderCompiler()
    {
        const SlangResult result = slang_createGlobalSession(SLANG_API_VERSION, &globalSession);
        APP_THROW_IF(SLANG_FAILED(result) || globalSession == nullptr, Sierra::InvalidOperationError("Cannot create Slang shader compiler, as creating its global session failed"));
    }

    /* --- POLLING METHODS --- */

    std::optional<CompiledShader> SlangShaderCompiler::Compile(const ShaderCompileInfo& compileInfo) const
    {
        // Everything about the source is derived from the path of its stream
        const Sierra::Path& sourceFilePath = compileInfo.fileStream.GetFilePath();
        SlangSource source
        {
            .filePath = sourceFilePath.GetString(),
            .moduleName = sourceFilePath.GetStem()
        };

        std::vector<uint8> sourceCode = { };
        if (compileInfo.fileStream.ReadToEnd(sourceCode) != Sierra::StreamResult::Success)
        {
            APP_WARNING("Cannot compile Slang shader [{0}], as no data could be read from its stream", source.filePath);
            return std::nullopt;
        }
        source.code = std::string(reinterpret_cast<const char*>(sourceCode.data()), sourceCode.size()); // Slang expects it null-terminated

        // Includes are searched for next to the source, and then within Sierra's include directory, so that any shader can #include "Sierra/Sierra.slang"
        std::vector<std::string> searchPathStrings = { };
        searchPathStrings.reserve(2);
        if (sourceFilePath.HasParentPath())
        {
            searchPathStrings.emplace_back(sourceFilePath.GetParentPath().GetString());
        }
        if (const Sierra::Path sierraIncludeDirectoryPath = SR_EDITOR_SHADER_INCLUDE_DIRECTORY_PATH; compileInfo.fileManager.IsDirectory(sierraIncludeDirectoryPath))
        {
            searchPathStrings.emplace_back(sierraIncludeDirectoryPath.GetString());
        }

        std::vector<const char*> searchPaths = { };
        searchPaths.reserve(searchPathStrings.size());
        for (const std::string& searchPath : searchPathStrings)
        {
            searchPaths.emplace_back(searchPath.c_str());
        }

        slang::TargetDesc targetDesc = { };
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = globalSession->findProfile("spirv_1_4");

        // NOTE: Optimizing and validating SPIR-V needs SPIRV-Tools, which Slang is built without, so neither is attempted
        // Warning 41012 reports every capability an entry point may use beyond the profile, which is noise, as only those actually used are declared
        // Warning 39001 reports overlapping bindings, which bindless resource arrays of different types (all images of a binding, for example) intentionally do
        constexpr std::array<slang::CompilerOptionEntry, 3> compilerOptions
        {
            slang::CompilerOptionEntry { .name = slang::CompilerOptionName::Optimization, .value = { .kind = slang::CompilerOptionValueKind::Int, .intValue0 = SLANG_OPTIMIZATION_LEVEL_NONE } },
            slang::CompilerOptionEntry { .name = slang::CompilerOptionName::SkipSPIRVValidation, .value = { .kind = slang::CompilerOptionValueKind::Int, .intValue0 = 1 } },
            slang::CompilerOptionEntry { .name = slang::CompilerOptionName::DisableWarnings, .value = { .kind = slang::CompilerOptionValueKind::String, .stringValue0 = "41012,39001" } }
        };

        slang::SessionDesc sessionDesc = { };
        sessionDesc.compilerOptionEntries = compilerOptions.data();
        sessionDesc.compilerOptionEntryCount = static_cast<uint32>(compilerOptions.size());
        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.searchPaths = searchPaths.data();
        sessionDesc.searchPathCount = static_cast<SlangInt>(searchPaths.size());
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

        Slang::ComPtr<slang::ISession> session = { };
        if (SLANG_FAILED(globalSession->createSession(sessionDesc, session.writeRef())))
        {
            APP_ERROR("Cannot compile Slang shader [{0}], as creating a Slang session failed", source.filePath);
            return std::nullopt;
        }

        Sierra::WriteStream buffer = { };
        std::optional<std::vector<ShaderEntryPoint>> entryPoints = WriteEntryPoints(*session, source, buffer);
        if (!entryPoints.has_value())
        {
            return std::nullopt;
        }

        CompiledShader compiledShader
        {
            .entryPoints = std::move(*entryPoints),
            .buffer = buffer.Release()
        };

        return compiledShader;
    }

    /* --- DESTRUCTOR --- */

    SlangShaderCompiler::~SlangShaderCompiler() noexcept
    {
        if (globalSession != nullptr) globalSession->release();
    }

}
