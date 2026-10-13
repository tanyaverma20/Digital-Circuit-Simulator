#include "test_framework.hpp"

int main() {
    return test_fw::TestRegistry::instance().runAll();
}
