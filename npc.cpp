#include "npc.h"
#include <algorithm>

NPC::NPC(NpcType t, const std::string& _name, int _x, int _y) 
    : type(t), name(_name), x(_x), y(_y), alive(true) {}

NPC::NPC(NpcType t, std::istream &is) : type(t) {
    is >> name;
    is >> x;
    is >> y;
    alive = true;
}

void NPC::move(int dx, int dy) {
    std::lock_guard<std::mutex> lock(mtx);
    if (!alive) return;
    int new_x = x + dx;
    int new_y = y + dy;

    if (new_x >= 0 && new_x < MAP_WIDTH) x = new_x;
    if (new_y >= 0 && new_y < MAP_HEIGHT) y = new_y;
}

bool NPC::is_alive() const {
    std::lock_guard<std::mutex> lock(mtx);
    return alive;
}

void NPC::must_die() {
    std::lock_guard<std::mutex> lock(mtx);
    alive = false;
}

int NPC::get_x() const {
    std::lock_guard<std::mutex> lock(mtx);
    return x;
}

int NPC::get_y() const {
    std::lock_guard<std::mutex> lock(mtx);
    return y;
}

NpcType NPC::get_type() const {
    return type;
}

std::string NPC::get_name() const {
    return name;
}

std::string NPC::get_type_name() const {
    switch(type) {
        case DruidType: return "Druid";
        case ElfType: return "Elf";
        case BlackKnightType: return "Knight";
        default: return "Unknown";
    }
}

char NPC::get_symbol() const {
    switch(type) {
        case DruidType: return 'D';
        case ElfType: return 'E';
        case BlackKnightType: return 'K';
        default: return '?';
    }
}

bool NPC::is_close(const std::shared_ptr<NPC> &other, size_t distance) const {
    std::lock_guard<std::mutex> lock(mtx);
    if (!alive || !other->is_alive()) return false;
    
    int dx = x - other->get_x();
    int dy = y - other->get_y();
    return (dx * dx + dy * dy) <= (distance * distance);
}

bool NPC::is_druid() const { return false; }
bool NPC::is_elf() const { return false; }
bool NPC::is_black_knight() const { return false; }

void NPC::subscribe(std::shared_ptr<IFightObserver> observer) {
    observers.push_back(observer);
}

void NPC::fight_notify(const std::shared_ptr<NPC> defender, bool win) {
    for (auto &o : observers)
        o->on_fight(shared_from_this(), defender, win);
}

void NPC::save(std::ostream &os) {
    os << name << std::endl;
    os << x << std::endl;
    os << y << std::endl;
}

std::ostream &operator<<(std::ostream &os, NPC &npc) {
    os << "{ name:" << npc.name << ", type:" << npc.get_type_name() 
       << ", x:" << npc.x << ", y:" << npc.y << ", alive:" << npc.alive << "} ";
    return os;
}