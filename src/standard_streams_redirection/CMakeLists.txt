set(TARGET_NAME standard_streams_redirection)

set(TARGET_SOURCES
    "standard_streams_redirection.c"
)

add_library(${TARGET_NAME} STATIC
    ${TARGET_SOURCES}
)

target_include_directories(${TARGET_NAME}
    PUBLIC "include" ${INCLUDE_DIRECTORIES_TO_PROPAGATE}
)

target_link_libraries(${TARGET_NAME}
    mdn_status
)

cmake_language(CALL ${PROJECT_NAME}_set_target_c_compiler_flags ${TARGET_NAME})
