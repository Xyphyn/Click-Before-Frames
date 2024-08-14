#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <stdlib.h> 
#include <random>

using namespace geode::prelude;

int random(int min, int max) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(min,max); 

    return dist(rng);
}

class $modify(MyGJBaseGameLayer, GJBaseGameLayer){

    struct Fields {
        bool m_doOriginal = false;
        int m_lastRand = 0;
        bool m_isRandom = false;
        int m_randMin = 0;
        int m_randMax = 0;
        int m_delay = 0;
    };

    bool init(){
        if(!GJBaseGameLayer::init()) return false;
        m_fields->m_isRandom = Mod::get()->getSettingValue<bool>("random-toggle");
        m_fields->m_randMin = Mod::get()->getSettingValue<int64_t>("rand-min");
        m_fields->m_randMax = Mod::get()->getSettingValue<int64_t>("rand-max");
        m_fields->m_delay = Mod::get()->getSettingValue<int64_t>("delay");
        return true;
    }

    void handleButton(bool down, int button, bool isPlayer1){
        if (m_fields->m_doOriginal || Mod::get()->getSettingValue<bool>("soft-toggle")) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }
        else {
            std::thread waitThread = std::thread{[=]() {
                int ms = 0;
                if (m_fields->m_isRandom) {
                    if (down) {
                        if (m_fields->m_randMin <= m_fields->m_randMax) {
                            ms = random(m_fields->m_randMin, m_fields->m_randMax);
                            m_fields->m_lastRand = ms;
                        }
                    }
                    else ms = m_fields->m_lastRand;
                }
                else ms = m_fields->m_delay;
                std::this_thread::sleep_for(std::chrono::milliseconds{ms});
                m_fields->m_doOriginal = true;
                GJBaseGameLayer::handleButton(down, button, isPlayer1);
                m_fields->m_doOriginal = false;
            }};
            waitThread.detach();
        }
    }
};