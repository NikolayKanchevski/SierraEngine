set(BASIS_UNIVERSAL_DIRECTORY_PATH ../../../../vendor/basis_universal)
target_sources(SierraEngineEditor PRIVATE
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_backend.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_backend.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_basis_file.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_basis_file.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_comp.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_comp.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_enc.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_enc.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_etc.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_etc.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_frontend.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_frontend.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_gpu_texture.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_gpu_texture.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_pvrtc1_4.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_pvrtc1_4.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_resampler.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_resampler.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_resample_filters.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_ssim.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_ssim.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_uastc_enc.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_uastc_enc.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_bc7enc.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_bc7enc.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/jpgd.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/jpgd.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_kernels_sse.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_opencl.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/basisu_opencl.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/pvpngreader.cpp
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/encoder/pvpngreader.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/transcoder/basisu_transcoder.h
        ${BASIS_UNIVERSAL_DIRECTORY_PATH}/transcoder/basisu_transcoder.cpp
)

target_include_directories(SierraEngineEditor PRIVATE ${BASIS_UNIVERSAL_DIRECTORY_PATH})
target_compile_definitions(SierraEngineEditor PRIVATE "BASISD_SUPPORT_KTX2_ZSTD=0")

target_sources(SierraEngineEditor PRIVATE
    BasisUniversalCompressor.cpp
    BasisUniversalCompressor.h
)