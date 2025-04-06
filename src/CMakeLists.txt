include(fetch_content_wrapper)
cmake_language(CALL ${PROJECT_NAME}_fetch_content_wrapper
    mdn_status
    GIT_REPOSITORY https://github.com/meni-adin/c-mdn-status.git
    GIT_TAG        origin/main
)

add_subdirectory(standard_streams_redirection)
