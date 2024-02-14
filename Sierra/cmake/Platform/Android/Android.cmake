function(BuildAndroidApplication SOURCE_FILES)
    add_library(${SIERRA_APPLICATION_NAME} SHARED ${SOURCE_FILES})
endfunction()