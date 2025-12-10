#include "black_knight.h"
#include "elf.h"
#include "druid.h"
#include <random>

BlackKnight::BlackKnight(const std::string& name, int x, int y) : NPC(BlackKnightType, name, x, y) {}
BlackKnight::BlackKnight(std::istream &is) : NPC(BlackKnightType, is) {}

bool BlackKnight::accept(const std::shared_ptr<IFightVisitor>& attacker) {
    return attacker->visit(std::static_pointer_cast<BlackKnight>(shared_from_this()));
}

void BlackKnight::print() {
    std::cout << *this;
}

void BlackKnight::save(std::ostream &os) {
    os << BlackKnightType << std::endl;
    NPC::save(os);
}

bool BlackKnight::is_black_knight() const {
    return true;
}

bool BlackKnight::fight(std::shared_ptr<Druid> other) {
    fight_notify(other, false);
    return false;
}

bool BlackKnight::fight(std::shared_ptr<Elf> other) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dice(1, 6);

    int attack_power = dice(gen);
    int defense_power = dice(gen);
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

bool BlackKnight::fight(std::shared_ptr<BlackKnight> other) {
    fight_notify(other, false);
    return false;
}

std::ostream &operator<<(std::ostream &os, BlackKnight &knight) {
    os << "Black Knight: " << *static_cast<NPC *>(&knight) << std::endl;
    return os;
}