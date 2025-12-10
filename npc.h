#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <set>
#include <string>
#include <mutex>
#include <random>

constexpr int MAP_WIDTH = 100;
constexpr int MAP_HEIGHT = 100;

constexpr int DRUID_MOVE_DISTANCE = 10;
constexpr int DRUID_FIGHT_DISTANCE = 10;
constexpr int ELF_MOVE_DISTANCE = 10;
constexpr int ELF_FIGHT_DISTANCE = 50;
constexpr int KNIGHT_MOVE_DISTANCE = 30;
constexpr int KNIGHT_FIGHT_DISTANCE = 10;

class Druid;
class Elf;
class BlackKnight;

class IFightVisitor {
public:
    virtual bool visit(const std::shared_ptr<Druid>&) = 0;
    virtual bool visit(const std::shared_ptr<Elf>&) = 0;
    virtual bool visit(const std::shared_ptr<BlackKnight>&) = 0;
    virtual ~IFightVisitor() = default;
};

class NPC;
using set_t = std::set<std::shared_ptr<NPC>>;

enum NpcType {
    DruidType = 1,
    ElfType = 2,
    BlackKnightType = 3
};

class IFightObserver {
public:
    virtual void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender,bool win) = 0;
};

class NPC : public std::enable_shared_from_this<NPC> {
protected:
    mutable std::mutex mtx;

    NpcType type;
    std::string name;
    int x{0};
    int y{0};
    bool alive{true};
    std::vector<std::shared_ptr<IFightObserver>> observers;
public:
    NPC(NpcType t, const std::string& _name, int _x, int _y);
    NPC(NpcType t, std::istream &is);

    virtual bool accept(const std::shared_ptr<IFightVisitor>& attacker) = 0;
    virtual void print() = 0;
    virtual void save(std::ostream &os);

    bool is_close(const std::shared_ptr<NPC> &other, size_t distance) const;
    virtual int get_move_distance() const = 0;
    virtual int get_fight_distance() const = 0;

    void move(int dx, int dy);
    bool is_alive() const;
    void must_die();
    
    int get_x() const;
    int get_y() const;
    NpcType get_type() const;
    std::string get_name() const;
    std::string get_type_name() const;
    char get_symbol() const;

    virtual bool is_druid() const;
    virtual bool is_elf() const;
    virtual bool is_black_knight() const;

    virtual bool fight(std::shared_ptr<Druid> other) = 0;
    virtual bool fight(std::shared_ptr<Elf> other) = 0;
    virtual bool fight(std::shared_ptr<BlackKnight> other) = 0;
    void subscribe(std::shared_ptr<IFightObserver> observer);
    void fight_notify(const std::shared_ptr<NPC> defender, bool win);

    friend std::ostream &operator<<(std::ostream &os, NPC &npc);
};