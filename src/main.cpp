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
    };

    void handleButton(bool down, int button, bool isPlayer1){
        if (m_fields->m_doOriginal || Mod::get()->getSettingValue<bool>("soft-toggle")) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }
        else {
            std::thread t1 = std::thread{[=]() {
                int ms = 0;
                bool doRandom = Mod::get()->getSettingValue<bool>("random-toggle");
                if(doRandom){
                    int min = Mod::get()->getSettingValue<int64_t>("rand-min");
                    int max = Mod::get()->getSettingValue<int64_t>("rand-max");

                    if(max >= min){
                        ms = random(min, max);
                    }
                    if (down) {
                        m_fields->m_lastRand = ms;
                    }
                    else {
                        ms = m_fields->m_lastRand;
                    }
                }
                else{
                    ms = Mod::get()->getSettingValue<int64_t>("delay");
                }
                std::this_thread::sleep_for(std::chrono::milliseconds{ms});
                queueInMainThread([=]() {
                    m_fields->m_doOriginal = true;
                    GJBaseGameLayer::handleButton(down, button, isPlayer1);
                    m_fields->m_doOriginal = false;
                });
            }};
            t1.detach();
        }
    }
};