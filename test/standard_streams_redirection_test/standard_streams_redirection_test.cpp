
#define _CRT_SECURE_NO_WARNINGS

#include <filesystem>
#include <fstream>
#include <gmock/gmock.h>
#include <iostream>

#if defined __linux__
#include <limits.h>
#elif defined __APPLE__
# include <mach-o/dyld.h>
#elif defined _WIN32
# include <windows.h>
#endif  // OS

#include "errors.h"
#include "standard_streams_redirection.h"

using namespace testing;
namespace fs = std::filesystem;

class StandardStreamsRedirectionTest : public ::testing::Test {
protected:
    static inline fs::path testExecutableDirPath;
    static inline fs::path testOutputDirPath;
    std::string            testFullName;
    FILE                  *testOutputFile;

public:
    static void SetUpTestSuite() {
        initTestSuitePaths();

        if (!fs::exists(testOutputDirPath)) {
            fs::create_directories(testOutputDirPath);
        }
    }

    void SetUp() override {
        initTestFullName();
    }

    static void initTestSuitePaths() {
#if defined __linux__
        char    pathBuffer[PATH_MAX];
        ssize_t count;

        count = readlink("/proc/self/exe", pathBuffer, sizeof(pathBuffer));
        ASSERT_NE(count, -1);
        ASSERT_NE(count, PATH_MAX);

        pathBuffer[count] = '\0';
        testExecutableDirPath = fs::path{pathBuffer}.parent_path();
#elif defined __APPLE__
        char    *pathBuffer    = nullptr;
        uint32_t pathBufferLen = 0;
        int      retVal;

        retVal = _NSGetExecutablePath(pathBuffer, &pathBufferLen);
        ASSERT_EQ(retVal, -1);
        pathBuffer = static_cast<decltype(pathBuffer)>(malloc(pathBufferLen));
        ASSERT_NE(pathBuffer, nullptr);

        retVal = _NSGetExecutablePath(pathBuffer, &pathBufferLen);
        ASSERT_EQ(retVal, 0);

        testExecutableDirPath = fs::path{pathBuffer}.parent_path();
        free(pathBuffer);
#elif defined _WIN32
        testExecutableDirPath = fs::path{_pgmptr}.parent_path();
#endif  // OS
        testOutputDirPath     = testExecutableDirPath / "test_output";
    }

    void initTestFullName() {
        const ::testing::TestInfo *testInfo;

        testInfo = UnitTest::GetInstance()->current_test_info();
        ASSERT_NE(testInfo, nullptr) << "Can't retrieve test info\n";

        testFullName += testInfo->test_suite_name();
        testFullName += "_";
        testFullName += testInfo->name();
    }

    void openTestOutputFile() {
        fs::path testOutputFilePath;

        testOutputFilePath  = testOutputDirPath / testFullName;
        testOutputFilePath += "_file.txt";
        testOutputFile      = fopen(testOutputFilePath.string().c_str(), "w");
        ASSERT_NE(testOutputFile, nullptr) << "Failed to open file: " << testOutputFilePath << "\n";
    }
};

TEST_F(StandardStreamsRedirectionTest, InitAndDeinit) {
    ASSERT_NO_FATAL_FAILURE(openTestOutputFile());

    ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDOUT, testOutputFile), SUCCESS);
    std::cout << "What's up?\n";
    ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDOUT), SUCCESS);
    std::cout << "And now?\n";

    fclose(testOutputFile);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
