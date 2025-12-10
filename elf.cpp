#include "elf.h"
#include "druid.h"
#include "black_knight.h"
#include <random>

Elf::Elf(const std::string& name, int x, int y) : NPC(ElfType, name, x, y) {}
Elf::Elf(std::istream &is) : NPC(ElfType, is) {}

bool Elf::accept(const std::shared_ptr<IFightVisitor>& attacker) {
    return attacker->visit(std::static_pointer_cast<Elf>(shared_from_this()));
}

void Elf::print() {
    std::cout << *this;
}

void Elf::save(std::ostream &os) {
    os << ElfType << std::endl;
    NPC::save(os);
}

bool Elf::is_elf() const {
    return true;
}

bool Elf::fight(std::shared_ptr<Druid> other) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dice(1, 6);
    
    int attack_power = dice(gen);
    int defense_power = dice(gen);
    
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

bool Elf::fight(std::shared_ptr<Elf> other) {
    fight_notify(other, false);
    return false;
}

bool Elf::fight(std::shared_ptr<BlackKnight> other) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dice(1, 6);
    
    int attack_power = dice(gen);
    int defense_power = dice(gen);
    
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

std::ostream &operator<<(std::ostream &os, Elf &elf) {
    os << "Elf: " << *static_cast<NPC *>(&elf) << std::endl;
    return os;
}