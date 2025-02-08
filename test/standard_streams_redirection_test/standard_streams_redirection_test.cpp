
#include <filesystem>
#include <fstream>
#include <gmock/gmock.h>
#include <iostream>

#if defined __linux__
#elif defined __APPLE__
# include <mach-o/dyld.h>
#elif defined _WIN32
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

    static void initTestSuitePaths() {  // TODO: review all versions - handle long paths
#if defined __linux__
        char    path[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
        if (count != -1) {
            path[count] = '\0';
            return std::string(path).substr(0, std::string(path).find_last_of(PATH_SEPARATOR));
        }
        return "";
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
        testOutputDirPath     = testExecutableDirPath / "test_output";

        free(pathBuffer);
#elif defined _WIN32
#endif  // OS
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
        testOutputFile      = fopen(testOutputFilePath.c_str(), "w");
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
