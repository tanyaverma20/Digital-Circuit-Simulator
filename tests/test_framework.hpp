#ifndef TESTS_TEST_FRAMEWORK_HPP
#define TESTS_TEST_FRAMEWORK_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <sstream>

namespace test_fw {

struct TestCase {
    std::string suite;
    std::string name;
    std::function<void()> func;
};

class TestRegistry {
public:
    static TestRegistry& instance() {
        static TestRegistry reg;
        return reg;
    }

    void addTest(const std::string& suite, const std::string& name, std::function<void()> func) {
        m_tests.push_back({suite, name, std::move(func)});
    }

    int runAll() {
        int passed = 0;
        int failed = 0;
        auto startTime = std::chrono::high_resolution_clock::now();

        std::cout << "\n========================================================\n";
        std::cout << "  DIGITAL CIRCUIT SIMULATOR - AUTOMATED TEST SUITE\n";
        std::cout << "========================================================\n\n";

        std::string currentSuite = "";
        for (const auto& test : m_tests) {
            if (test.suite != currentSuite) {
                currentSuite = test.suite;
                std::cout << "\n--- [ " << currentSuite << " ] ---\n";
            }

            bool testPassed = true;
            std::string failureMsg = "";

            try {
                test.func();
            } catch (const std::exception& e) {
                testPassed = false;
                failureMsg = e.what();
            } catch (...) {
                testPassed = false;
                failureMsg = "Unknown non-standard exception thrown";
            }

            if (testPassed) {
                passed++;
                std::cout << "  [PASS] " << test.name << "\n";
            } else {
                failed++;
                std::cout << "  [FAIL] " << test.name << "\n";
                std::cout << "         Assertion Error: " << failureMsg << "\n";
            }
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

        std::cout << "\n========================================================\n";
        std::cout << "  TEST EXECUTION SUMMARY\n";
        std::cout << "========================================================\n";
        std::cout << "  Total Tests: " << (passed + failed) << "\n";
        std::cout << "  Passed:      " << passed << "\n";
        std::cout << "  Failed:      " << failed << "\n";
        std::cout << "  Time:        " << elapsedMs << " ms\n";
        std::cout << "========================================================\n";

        if (failed == 0) {
            std::cout << "  RESULT: ALL TESTS PASSED!\n\n";
            return 0;
        } else {
            std::cout << "  RESULT: TEST SUITE FAILED (" << failed << " failures)\n\n";
            return 1;
        }
    }

private:
    std::vector<TestCase> m_tests;
};

struct TestRegistrar {
    TestRegistrar(const std::string& suite, const std::string& name, std::function<void()> func) {
        TestRegistry::instance().addTest(suite, name, std::move(func));
    }
};

class AssertionException : public std::runtime_error {
public:
    AssertionException(const std::string& msg) : std::runtime_error(msg) {}
};

inline void assertTrue(bool cond, const std::string& expr, const char* file, int line, const std::string& customMsg = "") {
    if (!cond) {
        std::ostringstream ss;
        ss << file << ":" << line << " Expected true: (" << expr << ")";
        if (!customMsg.empty()) ss << " - " << customMsg;
        throw AssertionException(ss.str());
    }
}

template <typename T, typename U>
inline void assertEq(const T& a, const U& b, const std::string& exprA, const std::string& exprB, const char* file, int line) {
    if (!(a == b)) {
        std::ostringstream ss;
        ss << file << ":" << line << " Assertion Failed: " << exprA << " == " << exprB
           << " [Actual: " << a << " vs Expected: " << b << "]";
        throw AssertionException(ss.str());
    }
}

} // namespace test_fw

#define TEST_CASE(suite, name) \
    void test_##suite##_##name(); \
    static test_fw::TestRegistrar reg_##suite##_##name(#suite, #name, test_##suite##_##name); \
    void test_##suite##_##name()

#define ASSERT_TRUE(expr) test_fw::assertTrue((expr), #expr, __FILE__, __LINE__)
#define ASSERT_FALSE(expr) test_fw::assertTrue(!(expr), "!" #expr, __FILE__, __LINE__)
#define ASSERT_EQ(a, b) test_fw::assertEq((a), (b), #a, #b, __FILE__, __LINE__)
#define ASSERT_NE(a, b) test_fw::assertTrue(!((a) == (b)), #a " != " #b, __FILE__, __LINE__)

#endif // TESTS_TEST_FRAMEWORK_HPP
