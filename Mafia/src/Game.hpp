#pragma once

#include <variant>
#include <coro/coro.hpp>
#include <concepts>
#include <format>
#include <string>

#include "Utils.hpp"
#include "Player.hpp"
#include "Logger.hpp"


void game(uint32_t n, bool haveHuman){
    // Раздача ролей и подготовка
    Logger logger;
    uint32_t humanId = 0;
    logger.logFinal(std::format("human play: {}", (haveHuman)? "true": "false"));
    if(haveHuman){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dist(1,n);
        humanId = dist(gen);
        logger.logFinal(std::format("human id: {}", humanId));
    }

    auto [plsMap, mafBand] = takeRoles(n, humanId, logger);
    bool isManLife = true;

    logger.logFinal(std::format("start play: players={}, mafias={}, man={}", n, mafBand->size(), (int)isManLife));
    std::cout << std::format("Добрый день, игра начинается.\nВ игре принимает участие {} игроков, из них {} мафий.\n", n, mafBand->size());

    uint32_t round = 1;
    while(true) {
        std::cout << std::format("Начинается раунд №{}.\n", round);
        std::cout << "Дневное голосование\n";
        // Дневное голосование
        logger.logRound("Day vote:");
        auto dayResult = dayVote(*plsMap, logger);
        std::cout << "По результатам голосования нас покидает игрок №" << dayResult.first << " R.I.P." << std::endl
                  << "Он " << (((*plsMap)[dayResult.first]->role() != Role::MAFIA)? "не ": "")
                  << "принадлежал к клану мафии\n";
        logger.logRound(std::format("\tResult: {} expelled({})", dayResult.first, dayResult.second));
        logger.logFinal(std::format("Round {}: day vote result:\n\t{} expelled({})", round, dayResult.first, dayResult.second));

        // Изгнание
        if ((*plsMap)[dayResult.first]->realRole() == Role::MAFIA) {
            mafBand->remove(dayResult.first);
        }
        if ((*plsMap)[dayResult.first]->realRole() == Role::MAN){
            isManLife = false;
        }
        plsMap->erase(dayResult.first);
        std::cout << std::format("В игре осталось {} участников, из них {} мафий.\n", plsMap->size(), mafBand->size());
        logger.logFinal(std::format("round {}(day): players={}, mafias={}, man={}", round, plsMap->size(), mafBand->size(), (int)isManLife));

        if(peacefulWin(mafBand->size(), isManLife)){
            logger.logFinal("Peaceful WIN");
            std::cout << "Мирные победили\n";
            break;
        } else if (mafiaWin(plsMap->size(), mafBand->size(), isManLife)){
            logger.logFinal("Mafia WIN");
            std::cout << "Мафия победила\n";
            break;
        } else if (manWin(mafBand->size(), isManLife)){
            logger.logFinal("Man WIN");
            std::cout << "Победил маньяк\n";
            break;
        }
        std::cout << "Город засыпает, просыпаются бесы...\n";
        // Ночные действия
        logger.logRound("Night act:");
        auto nightKilled = nightAct(*plsMap, logger);

        std::cout << ((!nightKilled.empty())? "Ночью были убиты:": "Ночью ничего не произошло") << std::endl;
        // Результаты ночных действий
        logger.logFinal(std::format("Round {}: night act result:", round));
        for (const auto& deadId: nightKilled){
            std::cout << "Игрок №" << deadId << std::endl;
            logger.logFinal(std::format("\t{} killed", deadId));
            logger.logRound(std::format("\tResult: {} killed", deadId));

            if ((*plsMap)[deadId]->realRole() == Role::MAFIA) {
                mafBand->remove(deadId);
            }
            if ((*plsMap)[deadId]->realRole() == Role::MAN){
                isManLife = false;
            }
            plsMap->erase(deadId);
        }
        std::cout << std::format("В игре осталось {} участников, из них {} мафий.\n", plsMap->size(), mafBand->size());
        logger.logFinal(std::format("round {}(night): players={}, mafias={}, man={}", round, plsMap->size(), mafBand->size(), (int)isManLife));
        if(peacefulWin(mafBand->size(), isManLife)){
            logger.logFinal("Peaceful WIN");
            std::cout << "Мирные победили\n";
            break;
        } else if (mafiaWin(plsMap->size(), mafBand->size(), isManLife)){
            logger.logFinal("Mafia WIN");
            std::cout << "Мафия победила\n";
            break;
        } else if (manWin(mafBand->size(), isManLife)){
            logger.logFinal("Man WIN");
            std::cout << "Победил маньяк\n";
            break;
        }
        round++, logger.nextRound();
    }
    logger.logFinal(std::format("end play: players={}, mafias={}, man={}", plsMap->size(), mafBand->size(), (int)isManLife));
}

