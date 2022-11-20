//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_graph.hpp>
#include <aoc_range.hpp>

#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <typeindex>
#include <utility>

namespace aoc::year2015 {

namespace {

class effect;
using effect_map = std::map<std::type_index, std::unique_ptr<effect>>;

std::weak_ordering compare_effects(const effect_map::value_type& l,
                                   const effect_map::value_type& r);

struct stats {
    std::string name;
    int hit_points;
    int mana;
    int damage;
    int armor;
    effect_map active_effects;

    stats(std::string name_,
          int hit_points_,
          int mana_,
          int damage_,
          int armor_)
        : name{name_},
          hit_points{hit_points_},
          mana{mana_},
          damage{damage_},
          armor{armor_},
          active_effects{}
    {
    }
    stats(const stats& s);
    stats& operator=(const stats& s);
    void add_effect(std::unique_ptr<effect>&& e);
    void apply_effects();
    bool dead() const noexcept { return hit_points <= 0; }

   private:
    auto tie_() const noexcept
    {
        return std::tie(name, hit_points, mana, damage, armor);
    }

   public:
    friend std::weak_ordering operator<=>(const stats& l,
                                          const stats& r) noexcept
    {
        const auto first_result{l.tie_() <=> r.tie_()};
        if (first_result != 0) {
            return first_result;
        }
        return std::lexicographical_compare_three_way(
            l.active_effects.begin(), l.active_effects.end(),
            r.active_effects.begin(), r.active_effects.end(), compare_effects);
    };
    [[maybe_unused]] friend bool operator==(const stats& l,
                                            const stats& r) noexcept
    {
        return (l <=> r) == 0;
    };
};

class effect {
   public:
    virtual ~effect() noexcept = default;

    void cast() { cast_(); }
    void apply()
    {
        if (timer_ > 0) {
            apply_();
        }
        timer_--;
    }
    void finish() { finish_(); }
    virtual std::unique_ptr<effect> clone(stats* new_target) = 0;
    int timer() const noexcept { return timer_; };
    friend auto operator<=>(const effect& l, const effect& r) noexcept
    {
        const std::type_index l_id{typeid(l)};
        const std::type_index r_id{typeid(r)};
        return std::tie(l_id, l.timer_) <=> std::tie(r_id, l.timer_);
    }
    [[maybe_unused]] friend bool operator==(const effect& l,
                                            const effect& r) noexcept
    {
        const std::type_index l_id{typeid(l)};
        const std::type_index r_id{typeid(r)};
        return std::tie(l_id, l.timer_) == std::tie(r_id, l.timer_);
    }

   protected:
    explicit effect(stats* target, int turns) : target_{target}, timer_(turns)
    {
    }
    virtual void cast_() {}
    virtual void apply_() {}
    virtual void finish_() {}
    stats* target_;
    int timer_;
};

class shield final : public effect {
   public:
    explicit shield(stats* target, int turns = 6) : effect{target, turns} {}
    virtual std::unique_ptr<effect> clone(stats* new_target) override
    {
        return std::make_unique<shield>(new_target, timer_);
    }

   protected:
    virtual void cast_() override { target_->armor += 7; }
    virtual void finish_() override { target_->armor -= 7; }
};

class poison final : public effect {
   public:
    explicit poison(stats* target, int turns = 6) : effect{target, turns} {}
    virtual std::unique_ptr<effect> clone(stats* new_target) override
    {
        return std::make_unique<poison>(new_target, timer_);
    }

   protected:
    virtual void apply_() override { target_->hit_points -= 3; }
};

class recharge final : public effect {
   public:
    explicit recharge(stats* target, int turns = 5) : effect{target, turns} {}
    virtual std::unique_ptr<effect> clone(stats* new_target) override
    {
        return std::make_unique<recharge>(new_target, timer_);
    }

   protected:
    virtual void apply_() override { target_->mana += 101; }
};

std::weak_ordering compare_effects(const effect_map::value_type& l,
                                   const effect_map::value_type& r)
{
    const auto& [l_id, l_ptr]{l};
    const auto& [r_id, r_ptr]{r};
    return std::tie(l_id, *l_ptr) <=> std::tie(r_id, *r_ptr);
}

stats::stats(const stats& s)
    : stats{s.name, s.hit_points, s.mana, s.damage, s.armor}
{
    for (const auto& [id, e] : s.active_effects) {
        active_effects.emplace(id, e->clone(this));
    }
    assert(s == *this);
}

stats& stats::operator=(const stats& s)
{
    name = s.name;
    hit_points = s.hit_points;
    mana = s.mana;
    damage = s.damage;
    armor = s.armor;
    active_effects.clear();
    for (const auto& [id, e] : s.active_effects) {
        active_effects.emplace(id, e->clone(this));
    }
    assert(s == *this);
    return *this;
}

void stats::add_effect(std::unique_ptr<effect>&& e)
{
    e->cast();
    auto& e_val{*e};
    const auto id{std::type_index(typeid(e_val))};
    if (!active_effects.contains(id)) {
        active_effects.emplace(id, std::move(e));
    }
}

void stats::apply_effects()
{
    std::vector<std::type_index> effects_to_erase;
    for (const auto& [id, e] : active_effects) {
        e->apply();
        if (e->timer() == 0) {
            e->finish();
            effects_to_erase.push_back(id);
        }
    }
    for (const auto& id : effects_to_erase) {
        active_effects.erase(id);
    }
}

struct game_state {
    stats player;
    stats boss;
    friend auto operator<=>(const game_state&, const game_state&) = default;
};

void cast_magic_missile(stats& /*caster*/, stats& enemy)
{
    enemy.hit_points -= 4;
}

void cast_drain(stats& caster, stats& enemy)
{
    // fmt::print(
    caster.hit_points += 2;
    enemy.hit_points -= 2;
}

void cast_shield(stats& caster, stats& /*enemy*/)
{
    caster.add_effect(std::make_unique<shield>(&caster));
}

void cast_poison(stats& /*caster*/, stats& enemy)
{
    enemy.add_effect(std::make_unique<poison>(&enemy));
}

void cast_recharge(stats& caster, stats& /*enemy*/)
{
    caster.add_effect(std::make_unique<recharge>(&caster));
}

void attack(const stats& attacker, stats& target)
{
    const int damage{std::max(1, attacker.damage - target.armor)};
    target.hit_points -= damage;
}

using cost_t = int;
using spell_signature = decltype(&cast_magic_missile);

enum class effect_target { self, enemy };

struct effect_info {
    std::type_index id;
    effect_target target;
};

struct spell {
    cost_t cost;
    spell_signature func;
    std::optional<effect_info> effect_info_;
};

const spell def_magic_missile{53, cast_magic_missile, {}};
const spell def_drain{73, cast_drain, {}};
const spell def_shield{113,
                       cast_shield,
                       {{typeid(shield), effect_target::self}}};
const spell def_poison{173,
                       cast_poison,
                       {{typeid(poison), effect_target::enemy}}};
const spell def_recharge{229,
                         cast_recharge,
                         {{typeid(recharge), effect_target::self}}};

const std::array<spell, 5> spell_list{
    {def_magic_missile, def_drain, def_shield, def_poison, def_recharge}};

auto allowed_spells(const game_state& state)
{
    return spell_list | rv::filter([&](const spell& s) {
               const auto& [player, boss]{state};
               if (s.cost > player.mana) {
                   // Not enough mana to cast this spell.
                   return false;
               }
               if (!s.effect_info_) {
                   // This spell causes no effect.
                   return true;
               }
               const auto& [cost, func, info]{s};
               const auto& [id, target]{*info};
               const auto& active_effects{target == effect_target::self
                                              ? player.active_effects
                                              : boss.active_effects};
               const auto found_effect{active_effects.find(id)};
               if (found_effect == active_effects.end()) {
                   // Effect is not active
                   return true;
               }
               // Whether effect wore off or will this turn
               return found_effect->second->timer() <= 0;
           });
}

stats parse_input(std::string_view input)
{
    const auto lines{sv_lines(input) | r::to<std::vector>};
    return {"Boss", to_int(lines[0].substr(lines[0].find_last_of(' ') + 1)), 0,
            to_int(lines[1].substr(lines[1].find_last_of(' ') + 1)), 0};
}

enum class game_difficulty { normal, hard };

game_state run_turn(const game_state& state,
                    const spell& player_spell,
                    const game_difficulty difficulty)
{
    game_state out{state};
    auto& [player, boss]{out};
    const auto& [cost, spell_func, id]{player_spell};

    // We start in the middle of the player's turn at the point where they make
    // a decision.
    spell_func(player, boss);
    player.mana -= cost;
    if (player.dead() || boss.dead()) {
        return out;
    }

    // Boss turn
    player.apply_effects();
    if (player.dead() || boss.dead()) {
        return out;
    }
    boss.apply_effects();
    if (player.dead() || boss.dead()) {
        return out;
    }
    attack(boss, player);
    if (player.dead() || boss.dead()) {
        return out;
    }

    // Beginning of player's next turn
    if (difficulty == game_difficulty::hard) {
        player.hit_points--;
    }
    if (player.dead() || boss.dead()) {
        return out;
    }

    player.apply_effects();
    if (player.dead() || boss.dead()) {
        return out;
    }
    boss.apply_effects();

    // At this point the player makes their next decision.
    return out;
}

struct game_queue_entry {
    game_state vert;
    int dist;
    friend auto operator<=>(const game_queue_entry& lhs,
                            const game_queue_entry& rhs) noexcept
    {
        return lhs.dist <=> rhs.dist;
    }
};

struct game_state_graph {
    using vertex_type = game_state;
    using cost_type = int;
    using queue_entry = game_queue_entry;

    const game_state root() const
    {
        auto out{start_};
        if (difficulty_ == game_difficulty::hard) {
            // XXX HACK: Because I defined "run_turn" to start in the middle
            // of the player's turn instead of the "start" as the
            // instructions defined it (because the player's decision is
            // based on information in the middle), the player losing 1 HP
            // at the beginning of a turn is missed for the first turn, so
            // do it here.
            out.player.hit_points--;
        }
        return out;
    }
    static bool reject(const vertex_type& v) noexcept
    {
        // XXX should reject be in this interface, or just part of
        // adjacencies?
        return v.player.dead();
    }
    bool accept(const vertex_type& v) const noexcept
    {
        return v.boss.dead() && !v.player.dead();
    }
    auto adjacencies(const vertex_type& v) const
    {
        return allowed_spells(v) | rv::transform([&](const spell& s) {
                   return queue_entry{run_turn(v, s, difficulty_), s.cost};
               }) |
               rv::filter([](const queue_entry& entry) {
                   return !reject(entry.vert);
               }) |
               r::to<std::vector>;
    }

    const game_state& start_;
    const game_difficulty difficulty_;
};

}  // namespace

aoc::solution_result day22(std::string_view input)
{
    const stats boss_stats{parse_input(trim(input))};
    const stats player_stats{"Player", 50, 500, 0, 0};

    const game_state start{player_stats, boss_stats};

    game_state_graph g1{start, game_difficulty::normal};
    const auto result1{dijkstra(g1)};
    const auto part1_result{result1.dist.at(*(result1.end))};

    game_state_graph g2{start, game_difficulty::hard};
    const auto result2{dijkstra(g2)};
    const auto part2_result{result2.dist.at(*(result2.end))};

    return {part1_result, part2_result};
}
}  // namespace aoc::year2015
