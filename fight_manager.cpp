#include "fight_manager.h"
#include "npc.h"
#include "druid.h"
#include "elf.h"
#include "black_knight.h"
#include <random>
#include <iostream>

FightEvent::FightEvent(std::shared_ptr<NPC> a, std::shared_ptr<NPC> d): attacker(a), defender(d) {}

FightManager& FightManager::get() {
    static FightManager instance;
    return instance;
}

void FightManager::add_event(FightEvent&& event) {
    std::lock_guard<std::mutex> lock(mtx);
    events.push(std::move(event));
    cv.notify_one();
}

class FightVisitor : public IFightVisitor {
private:
    std::shared_ptr<NPC> attacker;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dice;
    
public:
    FightVisitor(std::shared_ptr<NPC> att) 
        : attacker(att), gen(rd()), dice(1, 6) {}
    
    bool visit(const std::shared_ptr<Druid>& defender) override {
        int attack_power = dice(gen);
        int defense_power = dice(gen);
        bool win = attack_power > defense_power;
        if (win) defender->must_die();
        attacker->fight_notify(defender, win);
        return win;
    }
    
    bool visit(const std::shared_ptr<Elf>& defender) override {
        int attack_power = dice(gen);
        int defense_power = dice(gen);
        bool win = attack_power > defense_power;
        if (win) defender->must_die();
        attacker->fight_notify(defender, win);
        return win;
    }
    
    bool visit(const std::shared_ptr<BlackKnight>& defender) override {
        int attack_power = dice(gen);
        int defense_power = dice(gen);
        bool win = attack_power > defense_power;
        if (win) defender->must_die();
        attacker->fight_notify(defender, win);
        return win;
    }
};

void FightManager::process_events() {
    while (!stop_flag) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::milliseconds(100), 
                   [this](){ return !events.empty() || stop_flag; });
        
        if (stop_flag) break;
        
        if (!events.empty()) {
            auto event = events.front();
            events.pop();
            lock.unlock();
            
            auto visitor = std::make_shared<FightVisitor>(event.attacker);
            event.defender->accept(visitor);
        }
    }
}

void FightManager::stop() {
    std::lock_guard<std::mutex> lock(mtx);
    stop_flag = true;
    cv.notify_all();
}