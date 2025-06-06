#include <gtest/gtest.h>
#include "strobe.h"

TEST(StrobeTest, PauseIntervalNoAdjustment) {
    auto res = computePauseInterval(100.0, 0.0f);
    int expected_interval = 1000000 / (100.0 + 0.0);
    int expected_pause = expected_interval / 5;
    expected_interval -= expected_pause;
    EXPECT_EQ(res.first, expected_pause);
    EXPECT_EQ(res.second, expected_interval);
}

TEST(StrobeTest, PauseIntervalWithAdjustment) {
    auto res = computePauseInterval(50.0, 10.0f);
    int expected_interval = 1000000 / (50.0 + 10.0);
    int expected_pause = expected_interval / 5;
    expected_interval -= expected_pause;
    EXPECT_EQ(res.first, expected_pause);
    EXPECT_EQ(res.second, expected_interval);
}

TEST(StrobeTest, NegativeAdjustment) {
    auto res = computePauseInterval(200.0, -20.0f);
    int expected_interval = 1000000 / (200.0 - 20.0);
    int expected_pause = expected_interval / 5;
    expected_interval -= expected_pause;
    EXPECT_EQ(res.first, expected_pause);
    EXPECT_EQ(res.second, expected_interval);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
