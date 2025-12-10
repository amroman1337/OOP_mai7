#pragma once
#include "npc.h"

class Druid : public NPC {
public:
    Druid(const std::string& name, int x, int y);
    Druid(std::istream &is);

    virtual bool accept(const std::shared_ptr<IFightVisitor>& attacker) override;
    virtual int get_move_distance() const override { return DRUID_MOVE_DISTANCE; }
    virtual int get_fight_distance() const override { return DRUID_FIGHT_DISTANCE; }

    void print() override;
    void save(std::ostream &os) override;

    bool is_druid() const override;
    bool fight(std::shared_ptr<Druid> other) override;
    bool fight(std::shared_ptr<Elf> other) override;
    bool fight(std::shared_ptr<BlackKnight> other) override;

    friend std::ostream &operator<<(std::ostream &os, Druid &druid);
};