
#include <gmock/gmock.h>
#include <iostream>
#include <limits.h>

#include "errors.h"
#include "standard_streams_redirection.h"

using namespace testing;

class StandardStreamsRedirectionTest : public ::testing::Test {};

TEST_F(StandardStreamsRedirectionTest, InitAndDeinit) {
    ASSERT_EQ(StandardStreamsRedirection_init(), SUCCESS);
    ASSERT_EQ(StandardStreamsRedirection_deinit(), SUCCESS);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
