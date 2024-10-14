#pragma once

#include "ShtaredPtr.hpp"
#include "Random.hpp"
#include "Utils.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <ranges>
#include <list>
#include <optional>
#include <string>
#include <format>

namespace rv = std::ranges::views;

enum class Role 
{
    MAFIA = 0,
    DOC,
    COM,
    MAN,
    PEACE
};

static std::map<Role, std::string> RoleToStr = {
        {Role::MAFIA, "MAFIA"},
        {Role::DOC, "DOC"},
        {Role::COM, "COM"},
        {Role::MAN, "MAN"},
        {Role::PEACE, "PEACE"}
};

enum class Do
{
    HEAL,
    CHECK,
    KILL,
    MAFIA_VOTE
};

static std::map<Do, std::string> DoToStr = {
        {Do::HEAL, "HEAL"},
        {Do::CHECK, "CHECK"},
        {Do::MAFIA_VOTE, "MAFIA_VOTE"},
        {Do::KILL, "KILL"},
};

class Player{
    public:
        uint32_t id;
        Player(
            uint32_t id, SharedPtr<std::map<uint32_t, SharedPtr<Player>>>& players,
            bool isHuman = false
        ): id(id), players(players), isHuman(isHuman), roleView(Role::PEACE){
            roleReal = Role::PEACE;
        };

        virtual std::string invite(){
            return std::format("<id={}, Role={}>: ", id, RoleToStr[roleReal]);
        }

        virtual uint32_t readHumanDayAnswer(){

            while (true){
                std::string ansS;
                std::cout << invite();
                std::cin >> ansS;
                uint32_t ans;
                try {
                    ans = std::stoi(ansS);
                }
                catch (std::invalid_argument& e) {
                    std::cout << "Введите число\n";
                    continue;
                }
                if(players->contains(ans)){
                    std::cout << std::format("\t<{}>: Я предполагаю, что {} мафия\n", id, ans);
                    return ans;
                } else {
                    std::cout << "Игрока с таким id не существует или он мертв\n";
                }
            }
        }

        virtual std::optional<std::pair<Do, uint32_t >> readHumanNightAnswer(){return{};};

        virtual uint32_t day(){
            
            if( !players->contains(id) ){
                throw std::runtime_error(std::format("Player {} was died but takes part in the vote", id));
            }

            if(isHuman){
                std::cout << "Введите id игрока против которого хотите проголосовать:\n";
                return readHumanDayAnswer();
            }

            std::vector<uint32_t> victims;
            std::for_each(players->begin(), players->end(),
                          [&victims, this](auto& pair) {
                              if (pair.first != id) { victims.push_back(pair.first); }});
            auto choice = victims | std::ranges::views::filter([&](const auto& obj){return obj != id;});
            auto victim = *select_randomly(choice.begin(), choice.end());
            std::cout << std::format("\t<{}>: Я предполагаю, что {} мафия\n", id, victim);
            return victim;
        }

        virtual std::optional<std::pair<Do, uint32_t>> night(){
            return {};
        }

        virtual Role role(){
            return roleView;
        }

        virtual Role realRole(){
            return roleReal;
        }

        virtual ~Player() = default;
    protected:
    SharedPtr<std::map<uint32_t, SharedPtr<Player>>> players;
        bool isHuman = false;
        Role roleView;
        Role roleReal;
};

class Doc: public Player{
    public:
        Doc( uint32_t id,
            SharedPtr<std::map<uint32_t, SharedPtr<Player>>>& players, bool isHuman = false):Player(id, players, isHuman){
                roleView = Role::PEACE;
                roleReal = Role::DOC;
            };

        std::optional<std::pair<Do, uint32_t>> readHumanNightAnswer() override {
            while (true){
                std::string ansS;
                std::cout << invite();
                std::cin >> ansS;
                uint32_t ans;
                try {
                    ans = std::stoi(ansS);
                }
                catch (std::invalid_argument& e) {
                    std::cout << "Введите число\n";
                    continue;
                }

                if(players->contains(ans)){
                    if(ans != prevTarget) return std::pair{Do::HEAL, ans};
                    else std::cout << "Нельзя спасать одно и того же игрока два раза подряд\n";
                } else {
                    std::cout << "Игрока с таким id не существует или он мертв\n";
                }
            }
        }

        std::optional<std::pair<Do, uint32_t>> night() override {
            if(isHuman){
                std::cout << "Введите id игрока которого хотите спасти:\n";
                return readHumanNightAnswer();
            }
            std::vector<uint32_t> live;

            std::transform(players->begin(), players->end(), back_inserter(live),
              [](const std::pair<uint32_t, SharedPtr<Player>> &pair) { return pair.first; });

            auto canSave = live | std::ranges::views::filter([&](const auto& obj){return obj != prevTarget;});
            auto saved = select_randomly(canSave.begin(), canSave.end());
            prevTarget = *saved;
            return std::pair{Do::HEAL, (*saved)};
        }
    ~Doc() override = default;
    protected:
        uint32_t prevTarget = 0;
};

class Com: public Player{
    public:
        Com( uint32_t id, SharedPtr<std::map<uint32_t, SharedPtr<Player>>>& players,
                bool isHuman = false
            ):Player(id, players, isHuman){
                roleView = Role::PEACE;
                roleReal = Role::COM;
            }

        uint32_t day() override {
            if( !players->contains(id)){
                throw std::runtime_error(std::format("Player {} was died but takes part in the vote", id));
            }

            if(isHuman){
                std::cout << "Введите id игрока против которого хотите проголосовать" <<
                ((players->contains(killTarget))? std::format("(Вы знаете что id={} мафия)", killTarget): "")
                << ":\n";
                return readHumanDayAnswer();
            }

            if (killTarget != 0 && players->contains(killTarget)){
                std::cout << std::format("\t<{}>: Я предполагаю, что {} мафия\n", id, killTarget);
                return killTarget;
            }
            return Player::day();
        }

        std::optional<std::pair<Do, uint32_t >> readHumanNightAnswer() override{
            while(true){
                std::string act;
                std::string ansS;
                std::cout << invite();
                std::cin >> act;
                if (act == "skip"){
                    return{};
                }
                std::cin >> ansS;

                uint32_t pId;
                try {
                    pId = std::stoi(ansS);
                }
                catch (std::invalid_argument& e) {
                    std::cout << "Введите число\n";
                    continue;
                }

                if (act == "kill"){
                    if (players->contains(pId)){
                        return std::pair{Do::KILL, pId};
                    } else std::cout << "Игрока с таким id не существует или он мертв\n";
                } else if (act == "check"){
                    if (players->contains(pId) && !playerRole.contains(pId)){
                        auto tRole = (*players)[pId]->role();
                        playerRole[pId] = tRole;
                        std::cout << std::format("Игрок под {} является {}\n", pId, RoleToStr[tRole]);
                        if (tRole == Role::MAFIA) {
                            killTarget = pId;
                        }
                        return std::pair{Do::CHECK, pId};
                    } else if(players->contains(pId) && playerRole.contains(pId)) {
                        std::cout << "Вы уже проверяли этого игрока, его роль " << RoleToStr[playerRole[pId]] << std::endl;
                    }
                    else std::cout << "Игрока с таким id не существует или он мертв\n";
                } else std::cout << "Вы не можете применять действие '" << act <<"'\n";
            }
        }

        std::optional<std::pair<Do, uint32_t>> night() override {
            if( !players->contains(id)){
                throw std::runtime_error(std::format("Player {} was died but takes part in the vote", id));
            }

            if(isHuman){
                std::cout << "Выберите действие ('check'/'kill'/'skip') и по отношению к кому Вы хотите его применить:\n";
                return readHumanNightAnswer();
            }

            if(killTarget != 0){
                if((*players).contains(killTarget)){
                    return std::pair{Do::KILL, killTarget};
                } else {
                    playerRole.erase(killTarget);
                    killTarget = 0;
                }
            }
            std::vector<uint32_t> live;
            std::transform(players->begin(), players->end(), back_inserter(live),
              [](const std::pair<uint32_t, SharedPtr<Player>> &pair) { return pair.first; });


            auto checks = live | rv::filter([&](const auto& obj){return obj != id && !playerRole.contains(obj);});
            if (!checks.empty()) {
                auto check = select_randomly(checks.begin(), checks.end());
                auto tRole = (*players)[*check]->role();
                playerRole[*check] = tRole;
                if (tRole == Role::MAFIA) {
                    killTarget = *check;
                }
                return std::pair{Do::CHECK, *check};
            }
            return{};

        }

        ~Com() override = default;
    protected:
        uint32_t killTarget = 0;
        std::map<uint32_t, Role> playerRole;
};

class Maf: public Player {
    public:
        bool boss;
        Maf(uint32_t id, SharedPtr<std::map<uint32_t, SharedPtr<Player>>>& players, SharedPtr<std::list<uint32_t>>& mafiasBand,
            bool isHuman = false, bool boss = false):Player(id, players, isHuman), boss(boss), mafiasBand(mafiasBand)
            {
                roleView = Role::MAFIA;
                roleReal = Role::MAFIA;
            };

        uint32_t readHumanDayAnswer() override{
            while (true){
                std::string ansS;
                std::cout << invite();
                std::cin >> ansS;
                uint32_t ans;
                try {
                    ans = std::stoi(ansS);
                }
                catch (std::invalid_argument& e) {
                    std::cout << "Введите число\n";
                    continue;
                }
                if(players->contains(ans) && std::all_of(mafiasBand->begin(), mafiasBand->end(), [&ans](const auto& a){return a != ans;})){
                    std::cout << std::format("\t<{}>: Я предполагаю, что {} мафия\n", id, ans);
                    return ans;
                } else if (players->contains(ans) && std::any_of(mafiasBand->begin(), mafiasBand->end(), [&ans](const auto& a){return a == ans;})){
                    std::cout << "Нельзя убивать товарища по команде\n";
                }
                else {
                    std::cout << "Игрока с таким id не существует или он мертв\n";
                }
            }
        }

        uint32_t day() override {
            if( !players->contains(id)){
                throw std::runtime_error(std::format("Player {} was died but takes part in the vote", id));
            }
            if(isHuman){
                std::cout << "Введите id игрока против которого хотите проголосовать:\n";
                return readHumanDayAnswer();
            }

            std::vector<uint32_t> victims;
            std::transform(players->begin(), players->end(), back_inserter(victims),
              [](const std::pair<uint32_t, SharedPtr<Player>> &pair) { return pair.first; });

            auto choice = victims | std::ranges::views::filter(
                [this](const auto& obj){
                return std::all_of(mafiasBand->begin(), mafiasBand->end(), [&obj](auto& x){return x != obj;});
            });

            auto victim = *select_randomly(choice.begin(), choice.end());
            std::cout << std::format("\t<{}>: Я предполагаю, что {} мафия\n", id, victim);
            return victim;
        }

        static std::pair<Do, uint32_t> night(std::map<uint32_t, uint32_t>& idVote) {
            auto resVote = *std::max_element(idVote.begin(), idVote.end(), [](auto& a, auto& b){return a.second < b.second;});
            return {Do::KILL, resVote.first};
        }

        std::optional<std::pair<Do, uint32_t>> readHumanNightAnswer() override{
            while(true) {
                std::string ansS;
                std::cout << invite();
                std::cin >> ansS;
                uint32_t ans;
                try {
                    ans = std::stoi(ansS);
                }
                catch (std::invalid_argument& e) {
                    std::cout << "Введите число\n";
                    continue;
                }
                if (players->contains(ans) &&
                    std::all_of(mafiasBand->begin(), mafiasBand->end(), [&ans](const auto &a) { return a != ans; })) {
                    return std::pair{Do::MAFIA_VOTE, ans};
                } else if (players->contains(ans) && std::any_of(mafiasBand->begin(), mafiasBand->end(),
                                                                 [&ans](const auto &a) { return a == ans; })) {
                    std::cout << "Нельзя убивать товарища по команде\n";
                } else {
                    std::cout << "Игрока с таким id не существует или он мертв\n";
                }
            }
        }

        std::optional<std::pair<Do, uint32_t>> night() override{
            if( !players->contains(id)){
                throw std::runtime_error(std::format("Player {} was died but takes part in the vote", id));
            }

            if(isHuman){
                std::cout << "id членов мафии: ";
                for (const auto& maf: *mafiasBand){
                    std::cout << maf << ' ';
                }
                std::cout << std::endl;
                std::cout << "Выберите кого хотите убить этой ночью\n";
                return readHumanNightAnswer();
            }

            std::vector<uint32_t> victims;
            std::transform(players->begin(), players->end(), back_inserter(victims),
              [](const std::pair<uint32_t, SharedPtr<Player>> &pair) { return pair.first; });

            auto canKill = victims | std::ranges::views::filter(
                [this](const auto& obj){
                return std::all_of(mafiasBand->begin(), mafiasBand->end(), [&obj](auto& x){return x != obj;});
            });
            if(!canKill.empty()) {
                return std::pair{Do::MAFIA_VOTE, *select_randomly(canKill.begin(), canKill.end())};
            }
            return {};
        }

        void setBoss(){
            boss=true;
        }
        ~Maf() override = default;
    protected:
        SharedPtr<std::list<uint32_t>> mafiasBand;
};

class Man: public Player{
    public:
        Man(uint32_t id, SharedPtr<std::map<uint32_t, SharedPtr<Player>>>& players,
            bool isHuman = false): Player(id, players, isHuman){
            roleView = Role::PEACE;
            roleReal = Role::MAN;
            }
        std::optional<std::pair<Do, uint32_t>> readHumanNightAnswer() override {
            while (true){
                std::string ansS;
                std::cout << invite();
                std::cin >> ansS;
                uint32_t ans;
                try {
                    ans = std::stoi(ansS);
                }
                catch (std::invalid_argument& e) {
                    std::cout << "Введите число\n";
                    continue;
                }
                if(players->contains(ans)){
                    return std::pair{Do::KILL, ans};
                } else {
                    std::cout << "Игрока с таким id не существует или он мертв\n";
                }
            }
        }
        std::optional<std::pair<Do, uint32_t>> night() override{
            if( !players->contains(id)){
                throw std::runtime_error(std::format("Player {} was died but takes part in the vote", id));
            }
            if(isHuman){
                return readHumanNightAnswer();
            }
            std::vector<uint32_t> victims;
            std::transform(players->begin(), players->end(), back_inserter(victims),
              [](const std::pair<uint32_t, SharedPtr<Player>> &pair) { return pair.first; });

            auto canKill = victims | std::ranges::views::filter([&](const auto& obj){return obj != id;});
            if(!canKill.empty()){
                return std::pair{Do::KILL, (*select_randomly(canKill.begin(), canKill.end()))};
            }
            return {};
        }
        ~Man() override = default;
};
