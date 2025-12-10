#include "npc.h"
#include "druid.h"
#include "elf.h"
#include "black_knight.h"
#include "fight_manager.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <random>
#include <atomic>

std::mutex print_mutex;
std::atomic<bool> game_running{true};

void safe_print(const std::string& message) {
    std::lock_guard<std::mutex> lock(print_mutex);
    std::cout << message;
}

class TextObserver : public IFightObserver {
public:
    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win) override {
        if (win) {
            std::stringstream ss;
            ss << "\n Murder: " << attacker->get_name() << " (" << attacker->get_type_name()
               << ") attacked " << defender->get_name() << " (" << defender->get_type_name()
               << ") - " << (win ? "killed" : "MISSED") << std::endl;
            safe_print(ss.str());
        }
    }
};

std::shared_ptr<NPC> create_npc(NpcType type, const std::string& name, int x, int y) {
    std::shared_ptr<NPC> result;
    switch (type) {
        case DruidType:
            result = std::make_shared<Druid>(name, x, y);
            break;
        case ElfType:
            result = std::make_shared<Elf>(name, x, y);
            break;
        case BlackKnightType:
            result = std::make_shared<BlackKnight>(name, x, y);
            break;
    }
    if (result) {
        static auto observer = std::make_shared<TextObserver>();
        result->subscribe(observer);
    }
    return result;
}

void movement_thread(set_t& npcs) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> move_dist(-1, 1);
    
    while (game_running) {
        for (const auto& npc : npcs) {
            if (npc->is_alive()) {
                int max_move = npc->get_move_distance();
                int dx = move_dist(gen) * (max_move / 2);
                int dy = move_dist(gen) * (max_move / 2);
                npc->move(dx, dy);
            }
        }
        std::vector<std::shared_ptr<NPC>> alive_npcs;
        for (const auto& npc : npcs) {
            if (npc->is_alive()) alive_npcs.push_back(npc);
        }
        for (size_t i = 0; i < alive_npcs.size(); ++i) {
            for (size_t j = i + 1; j < alive_npcs.size(); ++j) {
                auto& npc1 = alive_npcs[i];
                auto& npc2 = alive_npcs[j];
                if (npc1->is_close(npc2, npc1->get_fight_distance()) ||
                    npc2->is_close(npc1, npc2->get_fight_distance())) {
                    FightManager::get().add_event(FightEvent(npc1, npc2));
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void print_map(const set_t& npcs) {
    std::lock_guard<std::mutex> lock(print_mutex);
    
    // очистка консоли
    std::cout << "\033[2J\033[1;1H";
    char map[MAP_HEIGHT][MAP_WIDTH];
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            map[y][x] = '.';
        }
    }
    
    int alive_count = 0;
    for (const auto& npc : npcs) {
        if (npc->is_alive()) {
            int x = npc->get_x();
            int y = npc->get_y();
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                map[y][x] = npc->get_symbol();
            }
            alive_count++;
        }
    }

    std::cout << "=== BALAGUR FATE 3 ===\n";
    std::cout << "Alive NPCs: " << alive_count << "\n";
    std::cout << "+" << std::string(MAP_WIDTH, '-') << "+\n";
    
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        std::cout << "|";
        for (int x = 0; x < MAP_WIDTH; ++x) {
            std::cout << map[y][x];
        }
        std::cout << "|\n";
    }
    std::cout << "+" << std::string(MAP_WIDTH, '-') << "+\n";
    std::cout << "Legend: D-Druid, E-Elf, K-Knight, .-empty\n";
}

void display_thread(const set_t& npcs) {
    while (game_running) {
        print_map(npcs);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

int main() {
    set_t npcs;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> coord_dist(0, MAP_WIDTH - 1);
    std::uniform_int_distribution<> type_dist(1, 3);
    
    std::string names[] = {"Aragorn", "Legolas", "Gimli", "Gandalf", "Frodo", 
                          "Sam", "Merry", "Pippin", "Boromir", "Faramir"};
    
    safe_print("generating 50 NPC\n");
    for (int i = 0; i < 50; ++i) {
        std::string name = names[i % 10] + "_" + std::to_string(i);
        NpcType type = static_cast<NpcType>(type_dist(gen));
        int x = coord_dist(gen);
        int y = coord_dist(gen);
        
        npcs.insert(create_npc(type, name, x, y));
    }
    
    safe_print("== starting game threads ==\n");

    std::thread fight_thread([]() {
        FightManager::get().process_events();
    });
    
    std::thread move_thread([&npcs]() {
        movement_thread(npcs);
    });
    
    std::thread display_thread_obj([&npcs]() {
        display_thread(npcs);
    });

    std::this_thread::sleep_for(std::chrono::seconds(30));
    
    game_running = false;
    FightManager::get().stop();
    
    move_thread.join();
    fight_thread.join();
    display_thread_obj.join();
    
    safe_print("\n=== GAME OVER ===\n");
    safe_print("Survivors: \n");
    
    int survivor_count = 0;
    for (const auto& npc : npcs) {
        if (npc->is_alive()) {
            std::stringstream ss;
            ss << "  " << npc->get_name() << " (" << npc->get_type_name() 
               << ") at (" << npc->get_x() << "," << npc->get_y() << ")\n";
            safe_print(ss.str());
            survivor_count++;
        }
    }
    
    std::stringstream ss;
    ss << "\nTotal survivors: " << survivor_count << " out of 50\n";
    safe_print(ss.str());
    
    return 0;
}