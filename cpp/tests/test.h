#pragma once

#include <iostream>
#include <string>
#include <stdexcept>

static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define REQUIRE(cond) \
    do { \
        if (!(cond)) { \
            std::cout << "FAILED at line " << __LINE__ << ": " #cond << std::endl; \
            ++g_tests_failed; \
            throw std::runtime_error("test failure"); \
        } \
    } while (0)

#define REQUIRE_FALSE(cond) REQUIRE(!(cond))

#define REQUIRE_THROWS_AS(stmt, exc) \
    do { \
        bool threw = false; \
        try { stmt; } catch (const exc&) { threw = true; } \
        if (!threw) { \
            std::cout << "FAILED at line " << __LINE__ << ": expected " #exc << std::endl; \
            ++g_tests_failed; \
            throw std::runtime_error("test failure"); \
        } \
    } while (0)

#define RUN_TEST(fn) \
    do { \
        std::cout << "  Running " #fn "... "; \
        try { fn(); std::cout << "PASSED" << std::endl; ++g_tests_passed; } \
        catch (...) { std::cout << "FAILED" << std::endl; } \
    } while (0)
