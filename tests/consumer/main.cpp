#include <suchi/Suchi.hpp>

int main() {
    suchi::Logging::initialize();
    suchi::Logging::getLogger("consumer-test").info("installed package works");
    suchi::Logging::shutdown();
}
