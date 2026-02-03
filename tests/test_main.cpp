/**
 * test_main.cpp - Google Test Main Entry Point
 *
 * This file provides the main() function for all unit tests.
 * Google Test will automatically discover and run all tests.
 */

#include <gtest/gtest.h>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
