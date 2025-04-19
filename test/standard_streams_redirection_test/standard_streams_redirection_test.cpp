
#define _CRT_SECURE_NO_WARNINGS  // NOLINT

#include <filesystem>
#include <fstream>
#include <gmock/gmock.h>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#if defined __linux__
# include <limits.h>  // NOLINT
#elif defined __APPLE__
# include <mach-o/dyld.h>
#elif defined _WIN32
# include <Windows.h>
#endif  // OS

#include "mdn/gtest_extension.hpp"
#include "mdn/standard_streams_redirection.h"
#include "mdn/status.h"

using namespace testing;
namespace fs = std::filesystem;

class StandardStreamsRedirectionTest : public mdn::GTestExtension {
public:
    static inline bool              testWithinTest = false;
    static inline const std::string testWithinTestFlag{"--test_within_test"};

protected:
    static inline const std::string stdinCommonEmptyFileName{"stdinCommonEmptyFile"};
    static inline const std::string stdinRedirectionFileName{"stdinRedirectionFile"};
    static inline const std::string stdoutRedirectionFileName{"stdoutRedirectionFile"};
    static inline const std::string stderrRedirectionFileName{"stderrRedirectionFile"};
    static inline const std::string stdinCliRedirectionFileName{"stdinCliRedirectionFile"};
    static inline const std::string stdoutCliRedirectionFileName{"stdoutCliRedirectionFile"};
    static inline const std::string stderrCliRedirectionFileName{"stderrCliRedirectionFile"};
    static inline const std::string textFileExtension{".txt"};

    static inline fs::path stdinCommonEmptyFilePath;
    fs::path               stdinRedirectionFilePath;
    fs::path               stdoutRedirectionFilePath;
    fs::path               stderrRedirectionFilePath;
    fs::path               stdinCliRedirectionFilePath;
    fs::path               stdoutCliRedirectionFilePath;
    fs::path               stderrCliRedirectionFilePath;

    FILE *stdinRedirectionFile  = nullptr;
    FILE *stdoutRedirectionFile = nullptr;
    FILE *stderrRedirectionFile = nullptr;

    static inline const std::string stdin1stLine{"stdin 1st line\n"};
    static inline const std::string stdin2ndLine{"stdin 2nd line\n"};
    static inline const std::string stdout1stLine{"stdout 1st line\n"};
    static inline const std::string stdout2ndLine{"stdout 2nd line\n"};
    static inline const std::string stderr1stLine{"stderr 1st line\n"};
    static inline const std::string stderr2ndLine{"stderr 2nd line\n"};

    static inline const std::string stdinRedirectionCliOperator{"<"};
    static inline const std::string stdoutRedirectionCliOperator{">"};
    static inline const std::string stderrRedirectionCliOperator{"2>"};

    std::string stdinRedirectionFileContent1;
    std::string stdinRedirectionFileContent2;
    std::string stdoutRedirectionFileContent;
    std::string stderrRedirectionFileContent;
    std::string stdinCliRedirectionFileContent;
    std::string stdoutCliRedirectionFileContent;
    std::string stderrCliRedirectionFileContent;

public:
    static void SetUpTestSuite() {
        ASSERT_NO_FATAL_FAILURE(mdn::GTestExtension::SetUpTestSuite());

        stdinCommonEmptyFilePath = testOutputDirPath / (testSuiteName + "_" + stdinCommonEmptyFileName + textFileExtension);
        writeStringToFile(std::string{"\n"}, stdinCommonEmptyFilePath);
    }

    static void TearDownTestSuite() {
        ASSERT_NO_FATAL_FAILURE(mdn::GTestExtension::TearDownTestSuite());
    }

    void SetUp() override {
        ASSERT_NO_FATAL_FAILURE(mdn::GTestExtension::SetUp());

        stdinRedirectionFilePath     = generateTestFilePath(stdinRedirectionFileName);
        stdoutRedirectionFilePath    = generateTestFilePath(stdoutRedirectionFileName);
        stderrRedirectionFilePath    = generateTestFilePath(stderrRedirectionFileName);
        stdinCliRedirectionFilePath  = generateTestFilePath(stdinCliRedirectionFileName);
        stdoutCliRedirectionFilePath = generateTestFilePath(stdoutCliRedirectionFileName);
        stderrCliRedirectionFilePath = generateTestFilePath(stderrCliRedirectionFileName);
    }

    void TearDown() override {
        ASSERT_NO_FATAL_FAILURE(mdn::GTestExtension::TearDown());
    }

protected:
    static void openTestFileForWriting(const fs::path &filePath, FILE *&openedFile) {
        openedFile = fopen(filePath.string().c_str(), "w");
        ASSERT_NE(openedFile, nullptr) << "Failed to open file: " << filePath << "\n";
    }

    static void openTestFileForReading(const fs::path &filePath, FILE *&openedFile) {
        openedFile = fopen(filePath.string().c_str(), "r");
        ASSERT_NE(openedFile, nullptr) << "Failed to open file: " << filePath << "\n";
    }

    static void readFileToString(const fs::path &filePath, std::string &content) {
        std::ifstream file(filePath);
        ASSERT_TRUE(file.is_open()) << "Failed to open " << filePath;
        content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    static void writeStringToFile(const std::string &content, const fs::path &filePath) {
        std::ofstream file(filePath);
        ASSERT_TRUE(file.is_open()) << "Failed to open " << filePath;
        file << content;
    }

    static void readStreamToString(std::istream &inputStream, std::string &content) {
        std::stringstream buffer;

        buffer << inputStream.rdbuf();
        content = buffer.str();
    }

    static std::string concatenateStringsWithSpaces(const std::vector<std::string> &stringsVector) {
        if (stringsVector.empty()) {
            return "";
        }

        return std::accumulate(std::next(stringsVector.begin()), stringsVector.end(), stringsVector[0], [](std::string firstString, const std::string &secondString) {
            return std::move(firstString) + " " + secondString;
        });
    }

    static void closeTestFile(FILE *file) {
        ASSERT_NE(fclose(file), EOF);
    }

    fs::path generateTestFilePath(const std::string &fileName) {
        return testOutputDirPath / (testFullName + "_" + fileName + textFileExtension);
    }

    void openStdoutRedirectionFileForWriting() {
        ASSERT_NO_FATAL_FAILURE(openTestFileForWriting(stdoutRedirectionFilePath, stdoutRedirectionFile));
    }

    void openStderrRedirectionFileForWriting() {
        ASSERT_NO_FATAL_FAILURE(openTestFileForWriting(stderrRedirectionFilePath, stderrRedirectionFile));
    }

    void openStdinRedirectionFileForReading() {
        ASSERT_NO_FATAL_FAILURE(openTestFileForReading(stdinRedirectionFilePath, stdinRedirectionFile));
    }

    std::string getGtestFilterForCurrentTest() {
        return std::string{"--gtest_filter="} + testFullName;
    }

    std::string generateCommand(bool useEmptyStdin = true) {
        const std::vector<std::string> commandElements{
            testExecutablePath.string(),
            getGtestFilterForCurrentTest(),
            testWithinTestFlag,
            stdinRedirectionCliOperator,
            useEmptyStdin ? stdinCommonEmptyFilePath.string() : stdinCliRedirectionFilePath.string(),
            stdoutRedirectionCliOperator,
            stdoutCliRedirectionFilePath.string(),
            stderrRedirectionCliOperator,
            stderrCliRedirectionFilePath.string(),
        };

        return concatenateStringsWithSpaces(commandElements);
    }
};

TEST_F(StandardStreamsRedirectionTest, StdinRedirection) {
    ASSERT_NO_FATAL_FAILURE(writeStringToFile(stdin1stLine, stdinRedirectionFilePath));

    ASSERT_NO_FATAL_FAILURE(openStdinRedirectionFileForReading());

    ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDIN, stdinRedirectionFile), MDN_STATUS_SUCCESS);
    readStreamToString(std::cin, stdinRedirectionFileContent1);
    ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDIN), MDN_STATUS_SUCCESS);

    ASSERT_NO_FATAL_FAILURE(closeTestFile(stdinRedirectionFile)) << "Failed to close file: " << stdinRedirectionFileName;

    ASSERT_EQ(stdinRedirectionFileContent1, stdin1stLine);
}

TEST_F(StandardStreamsRedirectionTest, StdoutRedirection) {
    ASSERT_NO_FATAL_FAILURE(openStdoutRedirectionFileForWriting());

    ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDOUT, stdoutRedirectionFile), MDN_STATUS_SUCCESS);
    std::cout << stdout1stLine;
    ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDOUT), MDN_STATUS_SUCCESS);

    ASSERT_NO_FATAL_FAILURE(closeTestFile(stdoutRedirectionFile)) << "Failed to close file: " << stdoutRedirectionFileName;

    ASSERT_NO_FATAL_FAILURE(readFileToString(stdoutRedirectionFilePath, stdoutRedirectionFileContent));
    ASSERT_EQ(stdoutRedirectionFileContent, stdout1stLine);
}

TEST_F(StandardStreamsRedirectionTest, StderrRedirection) {
    ASSERT_NO_FATAL_FAILURE(openStderrRedirectionFileForWriting());

    ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDERR, stderrRedirectionFile), MDN_STATUS_SUCCESS);
    std::cerr << stderr1stLine;
    ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDERR), MDN_STATUS_SUCCESS);

    ASSERT_NO_FATAL_FAILURE(closeTestFile(stderrRedirectionFile)) << "Failed to close file: " << stderrRedirectionFileName;

    ASSERT_NO_FATAL_FAILURE(readFileToString(stderrRedirectionFilePath, stderrRedirectionFileContent));
    ASSERT_EQ(stderrRedirectionFileContent, stderr1stLine);
}

TEST_F(StandardStreamsRedirectionTest, StdinStdoutStderrRedirection) {
    ASSERT_NO_FATAL_FAILURE(writeStringToFile(stdin1stLine, stdinRedirectionFilePath));

    ASSERT_NO_FATAL_FAILURE(openStdinRedirectionFileForReading());
    ASSERT_NO_FATAL_FAILURE(openStdoutRedirectionFileForWriting());
    ASSERT_NO_FATAL_FAILURE(openStderrRedirectionFileForWriting());

    ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDIN, stdinRedirectionFile), MDN_STATUS_SUCCESS);
    ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDOUT, stdoutRedirectionFile), MDN_STATUS_SUCCESS);
    ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDERR, stderrRedirectionFile), MDN_STATUS_SUCCESS);
    readStreamToString(std::cin, stdinRedirectionFileContent1);
    std::cout << stdout1stLine;
    std::cerr << stderr1stLine;
    ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDIN), MDN_STATUS_SUCCESS);
    ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDOUT), MDN_STATUS_SUCCESS);
    ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDERR), MDN_STATUS_SUCCESS);

    ASSERT_NO_FATAL_FAILURE(closeTestFile(stdinRedirectionFile)) << "Failed to close file: " << stdinRedirectionFileName;
    ASSERT_NO_FATAL_FAILURE(closeTestFile(stdoutRedirectionFile)) << "Failed to close file: " << stdoutRedirectionFileName;
    ASSERT_NO_FATAL_FAILURE(closeTestFile(stderrRedirectionFile)) << "Failed to close file: " << stderrRedirectionFileName;

    ASSERT_NO_FATAL_FAILURE(readFileToString(stdoutRedirectionFilePath, stdoutRedirectionFileContent));
    ASSERT_NO_FATAL_FAILURE(readFileToString(stderrRedirectionFilePath, stderrRedirectionFileContent));
    ASSERT_EQ(stdinRedirectionFileContent1, stdin1stLine);
    ASSERT_EQ(stdoutRedirectionFileContent, stdout1stLine);
    ASSERT_EQ(stderrRedirectionFileContent, stderr1stLine);
}

TEST_F(StandardStreamsRedirectionTest, StdinRedirectionRestore) {
    if (!testWithinTest) {
        ASSERT_NO_FATAL_FAILURE(writeStringToFile(stdin1stLine, stdinRedirectionFilePath));
        ASSERT_NO_FATAL_FAILURE(writeStringToFile(stdin2ndLine, stdinCliRedirectionFilePath));
        const std::string command = generateCommand(false);

        const int errorCode = system(command.c_str());  // NOLINT
        ASSERT_EQ(errorCode, 0) << "Inner test failed with error code " << errorCode;
    } else {
        ASSERT_NO_FATAL_FAILURE(openStdinRedirectionFileForReading());

        ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDIN, stdinRedirectionFile), MDN_STATUS_SUCCESS);
        readStreamToString(std::cin, stdinRedirectionFileContent1);
        ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDIN), MDN_STATUS_SUCCESS);
        readStreamToString(std::cin, stdinRedirectionFileContent2);

        ASSERT_NO_FATAL_FAILURE(closeTestFile(stdinRedirectionFile)) << "Failed to close file: " << stdinRedirectionFileName;

        ASSERT_EQ(stdinRedirectionFileContent1, stdin1stLine);
        ASSERT_EQ(stdinRedirectionFileContent2, stdin2ndLine);
    }
}

TEST_F(StandardStreamsRedirectionTest, StdoutRedirectionRestore) {
    if (!testWithinTest) {
        const std::string command = generateCommand();

        const int errorCode = system(command.c_str());  // NOLINT
        ASSERT_EQ(errorCode, 0) << "Inner test failed with error code " << errorCode;

        ASSERT_NO_FATAL_FAILURE(readFileToString(stdoutCliRedirectionFilePath, stdoutCliRedirectionFileContent));
        EXPECT_NE(stdoutCliRedirectionFileContent.find(stdout2ndLine), std::string::npos);
    } else {
        ASSERT_NO_FATAL_FAILURE(openStdoutRedirectionFileForWriting());

        ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDOUT, stdoutRedirectionFile), MDN_STATUS_SUCCESS);
        std::cout << stdout1stLine;
        ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDOUT), MDN_STATUS_SUCCESS);
        std::cout << stdout2ndLine;

        ASSERT_NO_FATAL_FAILURE(closeTestFile(stdoutRedirectionFile)) << "Failed to close file: " << stdoutRedirectionFileName;

        ASSERT_NO_FATAL_FAILURE(readFileToString(stdoutRedirectionFilePath, stdoutRedirectionFileContent));
        ASSERT_EQ(stdoutRedirectionFileContent, stdout1stLine);
    }
}

TEST_F(StandardStreamsRedirectionTest, StderrRedirectionRestore) {
    if (!testWithinTest) {
        const std::string command = generateCommand();

        const int errorCode = system(command.c_str());  // NOLINT
        ASSERT_EQ(errorCode, 0) << "Inner test failed with error code " << errorCode;

        ASSERT_NO_FATAL_FAILURE(readFileToString(stderrCliRedirectionFilePath, stderrCliRedirectionFileContent));
        EXPECT_NE(stderrCliRedirectionFileContent.find(stderr2ndLine), std::string::npos);
    } else {
        ASSERT_NO_FATAL_FAILURE(openStderrRedirectionFileForWriting());

        ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDERR, stderrRedirectionFile), MDN_STATUS_SUCCESS);
        std::cerr << stderr1stLine;
        ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDERR), MDN_STATUS_SUCCESS);
        std::cerr << stderr2ndLine;

        ASSERT_NO_FATAL_FAILURE(closeTestFile(stderrRedirectionFile)) << "Failed to close file: " << stderrRedirectionFileName;

        ASSERT_NO_FATAL_FAILURE(readFileToString(stderrRedirectionFilePath, stderrRedirectionFileContent));
        ASSERT_EQ(stderrRedirectionFileContent, stderr1stLine);
    }
}

TEST_F(StandardStreamsRedirectionTest, StdinStdoutStderrRedirectionRestore) {
    if (!testWithinTest) {
        ASSERT_NO_FATAL_FAILURE(writeStringToFile(stdin1stLine, stdinRedirectionFilePath));
        ASSERT_NO_FATAL_FAILURE(writeStringToFile(stdin2ndLine, stdinCliRedirectionFilePath));
        const std::string command = generateCommand(false);

        const int errorCode = system(command.c_str());  // NOLINT
        ASSERT_EQ(errorCode, 0) << "Inner test failed with error code " << errorCode;

        ASSERT_NO_FATAL_FAILURE(readFileToString(stdoutCliRedirectionFilePath, stdoutCliRedirectionFileContent));
        ASSERT_NO_FATAL_FAILURE(readFileToString(stderrCliRedirectionFilePath, stderrCliRedirectionFileContent));
        EXPECT_NE(stdoutCliRedirectionFileContent.find(stdout2ndLine), std::string::npos);
        EXPECT_NE(stderrCliRedirectionFileContent.find(stderr2ndLine), std::string::npos);
    } else {
        ASSERT_NO_FATAL_FAILURE(openStdinRedirectionFileForReading());
        ASSERT_NO_FATAL_FAILURE(openStdoutRedirectionFileForWriting());
        ASSERT_NO_FATAL_FAILURE(openStderrRedirectionFileForWriting());

        ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDIN, stdinRedirectionFile), MDN_STATUS_SUCCESS);
        ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDOUT, stdoutRedirectionFile), MDN_STATUS_SUCCESS);
        ASSERT_EQ(StandardStreamsRedirection_start(STREAM_ID_STDERR, stderrRedirectionFile), MDN_STATUS_SUCCESS);
        readStreamToString(std::cin, stdinRedirectionFileContent1);
        std::cout << stdout1stLine;
        std::cerr << stderr1stLine;
        ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDIN), MDN_STATUS_SUCCESS);
        ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDOUT), MDN_STATUS_SUCCESS);
        ASSERT_EQ(StandardStreamsRedirection_stop(STREAM_ID_STDERR), MDN_STATUS_SUCCESS);
        readStreamToString(std::cin, stdinRedirectionFileContent2);
        std::cout << stdout2ndLine;
        std::cerr << stderr2ndLine;

        ASSERT_NO_FATAL_FAILURE(closeTestFile(stdinRedirectionFile)) << "Failed to close file: " << stdinRedirectionFileName;
        ASSERT_NO_FATAL_FAILURE(closeTestFile(stdoutRedirectionFile)) << "Failed to close file: " << stdoutRedirectionFileName;
        ASSERT_NO_FATAL_FAILURE(closeTestFile(stderrRedirectionFile)) << "Failed to close file: " << stderrRedirectionFileName;

        ASSERT_NO_FATAL_FAILURE(readFileToString(stdoutRedirectionFilePath, stdoutRedirectionFileContent));
        ASSERT_NO_FATAL_FAILURE(readFileToString(stderrRedirectionFilePath, stderrRedirectionFileContent));
        ASSERT_EQ(stdinRedirectionFileContent1, stdin1stLine);
        ASSERT_EQ(stdinRedirectionFileContent2, stdin2ndLine);
        ASSERT_EQ(stdoutRedirectionFileContent, stdout1stLine);
        ASSERT_EQ(stderrRedirectionFileContent, stderr1stLine);
    }
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    if ((argc > 1) && (std::string{argv[1]} == StandardStreamsRedirectionTest::testWithinTestFlag)) {
        StandardStreamsRedirectionTest::testWithinTest = true;
    }

    return RUN_ALL_TESTS();
}
