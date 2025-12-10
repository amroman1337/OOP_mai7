#include "druid.h"
#include "elf.h"
#include "black_knight.h"
#include <random>

Druid::Druid(const std::string& name, int x, int y) : NPC(DruidType, name, x, y) {}
Druid::Druid(std::istream &is) : NPC(DruidType, is) {}

bool Druid::accept(const std::shared_ptr<IFightVisitor>& attacker) {
    return attacker->visit(std::static_pointer_cast<Druid>(shared_from_this()));
}

void Druid::print() {
    std::cout << *this;
}

void Druid::save(std::ostream &os) {
    os << DruidType << std::endl;
    NPC::save(os);
}

bool Druid::is_druid() const {
    return true;
}

bool Druid::fight(std::shared_ptr<Druid> other) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dice(1, 6);

    int attack_power = dice(gen);
    int defense_power = dice(gen);
    bool win = attack_power > defense_power;
    
    fight_notify(other, win);
    return win;
}

bool Druid::fight(std::shared_ptr<Elf> other) {
    fight_notify(other, false);
    return false;
}

bool Druid::fight(std::shared_ptr<BlackKnight> other) {
    fight_notify(other, false);
    return false;
}

std::ostream &operator<<(std::ostream &os, Druid &druid) {
    os << "Druid: " << *static_cast<NPC *>(&druid) << std::endl;
    return os;
}