#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <stdlib.h>
#include <random>
using namespace geode::prelude;

int random(int min, int max) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return dist(rng);
}

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
struct Fields {
    bool m_doOriginal = false;
    bool m_isRandom = false;
    int m_randMin = 0;
    int m_randMax = 100; // Default probability range
    bool m_randomInputEnabled = false;
};

bool init() {
    if (!GJBaseGameLayer::init()) return false;
    
    m_fields->m_isRandom = Mod::get()->getSettingValue<bool>("random-toggle");
    m_fields->m_randMin = Mod::get()->getSettingValue<int64_t>("rand-min");
    m_fields->m_randMax = Mod::get()->getSettingValue<int64_t>("rand-max");
    
    return true;
}

void handleButton(bool down, int button, bool isPlayer1) {
    // If random input is completely disabled, use original behavior
    if (!m_fields->m_isRandom) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
        return;
    }
    
    // Generate a random number to determine if input should be triggered
    int randomTrigger = random(0, 100);
    
    // Check if the random number falls within the specified range
    if (randomTrigger >= m_fields->m_randMin && randomTrigger <= m_fields->m_randMax) {
        std::thread triggerThread([=]() {
            queueInMainThread([=]() {
                if (PlayLayer::get()) {
                    m_fields->m_doOriginal = true;
                    GJBaseGameLayer::handleButton(down, button, isPlayer1);
                    m_fields->m_doOriginal = false;
                }
            });
        });
        triggerThread.detach();
    }
}
};