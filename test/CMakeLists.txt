include(fetch_content_wrapper)
cmake_language(CALL ${PROJECT_NAME}_fetch_content_wrapper
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        origin/main
)
cmake_language(CALL ${PROJECT_NAME}_fetch_content_wrapper
    gtest_extension
    GIT_REPOSITORY https://github.com/meni-adin/cpp-mdn-gtest-extension.git
    GIT_TAG        origin/main
)

enable_testing()

add_subdirectory(standard_streams_redirection_test)
