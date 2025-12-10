#pragma once
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

class NPC;
class Druid;
class Elf;
class BlackKnight;

struct FightEvent {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
    
    FightEvent(std::shared_ptr<NPC> a, std::shared_ptr<NPC> d);
};

class FightManager {
private:
    std::queue<FightEvent> events;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop_flag{false};
    
    FightManager() = default;
    
public:
    static FightManager& get();
    
    void add_event(FightEvent&& event);
    void process_events();
    void stop();
};