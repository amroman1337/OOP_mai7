#pragma once
#include "npc.h"

class BlackKnight : public NPC {
public:
    BlackKnight(const std::string& name, int x, int y);
    BlackKnight(std::istream &is);

    virtual bool accept(const std::shared_ptr<IFightVisitor>& attacker) override;
    virtual int get_move_distance() const override { return KNIGHT_MOVE_DISTANCE; }
    virtual int get_fight_distance() const override { return KNIGHT_FIGHT_DISTANCE; }

    void print() override;
    void save(std::ostream &os) override;
    bool is_black_knight() const override;
    bool fight(std::shared_ptr<Druid> other) override;
    bool fight(std::shared_ptr<Elf> other) override;
    bool fight(std::shared_ptr<BlackKnight> other) override;

    friend std::ostream &operator<<(std::ostream &os, BlackKnight &knight);
};