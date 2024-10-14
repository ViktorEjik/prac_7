#pragma once

#include <algorithm>
#include <map>
#include <vector>
#include <coro/coro.hpp>
#include <set>

#include "Player.hpp"
#include "Logger.hpp"


template<class T>
concept DayPlayer = requires(T a) {
    a.day();
    a.id;
};

template<class T>
concept NightPlayer = requires(T a) {
    a.night();
    a.id;
};

template <class T>
concept TotalOrderedValue = std::totally_ordered<T>;

template<class Key, TotalOrderedValue Value>
std::pair<Key, Value> maxElemInMap(const std::map<Key, Value>& map){
    return *std::max_element(map.begin(), map.end(), [](auto& a, auto& b) { return a.second < b.second; });
}

template<DayPlayer T> coro::task<uint32_t> corDay(T& player, Logger& logger){
    auto res = player.day();
    logger.logRound(std::format("\t{} votes for {}", player.id, res));
    co_return res;
}

template<NightPlayer T>
coro::task<std::optional<std::pair<Do, uint32_t>>> corNight(T& player, Logger& logger){
    auto act = player.night();
    if(act){
        logger.logRound(std::format("\t{} do {} with {}", player.id, DoToStr[act->first], act->second));
    }
    co_return act;
}

bool peacefulWin(size_t mafiaBand, bool isManLife ){
    return (mafiaBand == 0) && !isManLife;
}

bool manWin(size_t mafiaBand, bool isManLife){
    return (mafiaBand == 0) && isManLife;
}

bool mafiaWin(size_t livePls, size_t mafiaBand, bool isManLife){
    return (mafiaBand > livePls - mafiaBand) || ((mafiaBand == (livePls - mafiaBand)) && !isManLife);
}

std::pair<uint32_t, uint32_t>
dayVote(std::map<uint32_t, SharedPtr<Player>>& live, Logger& logger){
    std::vector<coro::task<uint32_t>> tasks{};
    for(auto& pl: live){
        tasks.emplace_back(corDay(*pl.second, logger));
    }
    auto results = coro::sync_wait(coro::when_all(std::move(tasks)));

    std::map<uint32_t, uint32_t> count;
    std::for_each(results.begin(), results.end(), [&count](const auto& elem){
        count[elem.return_value()] += 1;
    });

    return maxElemInMap(count);
}

std::set<uint32_t> nightAct(std::map<uint32_t, SharedPtr<Player>>& live, Logger& logger){
    std::vector<coro::task<std::optional<std::pair<Do, uint32_t >>>> tasks{};
    for(auto& pl: live){
        tasks.emplace_back(corNight(*pl.second, logger));
    }
    auto results = coro::sync_wait(coro::when_all(std::move(tasks)));
    std::set<uint32_t> kill;
    std::vector<uint32_t> mafiasVote;
    uint32_t heallId = 0;
    for(auto& res: results){
        auto t = res.return_value();
        if(t){
            switch (t.value().first) {
                case Do::HEAL:
                    heallId = t.value().second;
                    break;
                case Do::MAFIA_VOTE:
                    mafiasVote.push_back(t.value().second);
                    break;
                case Do::CHECK:
                    break;
                case Do::KILL:
                    kill.insert(t.value().second);
                    break;
            }
        }
    }
    if(!mafiasVote.empty()) {
        std::map<uint32_t, uint32_t> count;
        std::for_each(mafiasVote.begin(), mafiasVote.end(), [&count](const auto &elem) {
            count[elem] += 1;
        });
        kill.insert(Maf::night(count).second);
    }
    kill.erase(heallId);
    return kill;
}

std::tuple<SharedPtr<std::map<uint32_t, SharedPtr<Player>>>, SharedPtr<std::list<uint32_t>>>
takeRoles(uint32_t n, uint32_t humanId, Logger& logger ){
    SharedPtr<std::map<uint32_t, SharedPtr<Player>>> playersMap(new std::map<uint32_t , SharedPtr<Player>>{});
    SharedPtr<std::list<uint32_t>> mafiaBandPtr(new std::list<uint32_t >);
    std::map<Role, uint32_t> roleCount = {
            {Role::MAN, 1},
            {Role::MAFIA, n/3},
            {Role::DOC, 1},
            {Role::COM, 1},
            {Role::PEACE, n - n/3 - 3}
    };
    logger.logFinal("Total role count:");
    for(auto[k, e]: roleCount){
        logger.logFinal(std::format("\t{}: {}", RoleToStr[k], e));
        std::cout << RoleToStr[k] << ' ' << e <<std::endl;
    }std::cout << "--------------------------\n" << std::endl;

    logger.logFinal("Player id role:");
    for(uint32_t i = 1; i <=n; i++){
        auto role = select_randomly(roleCount.begin(), roleCount.end());
        SharedPtr<Player> t;
        switch (role->first) {
            case Role::MAN:
                t = new Man(i, playersMap, i==humanId);
                role->second -= 1;
                if(role->second == 0){
                    roleCount.erase(Role::MAN);
                }
                break;
            case Role::MAFIA:
                t = new Maf(i, playersMap, mafiaBandPtr, i==humanId);
                mafiaBandPtr->push_back(i);
                role->second -= 1;
                if(role->second == 0){
                    roleCount.erase(Role::MAFIA);
                }
                break;
            case Role::DOC:
                t = new Doc(i, playersMap, i==humanId);
                role->second -= 1;
                if(role->second == 0){
                    roleCount.erase(Role::DOC);
                }
                break;
            case Role::COM:
                t = new Com(i, playersMap, i==humanId);
                role->second -= 1;
                if(role->second == 0){
                    roleCount.erase(Role::COM);
                }
                break;
            case Role::PEACE:
                t = new Player(i, playersMap, i==humanId);
                role->second -= 1;
                if(role->second == 0){
                    roleCount.erase(Role::PEACE);
                }
                break;
        }
        logger.logFinal(std::format("\t{}: {}", i, RoleToStr[role->first]));
        (*playersMap)[i] = t;
    }
    return {playersMap, mafiaBandPtr};
}
