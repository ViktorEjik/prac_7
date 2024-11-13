//
// Created by viktor on 11.11.2024.
//

#ifndef ANNEALING_SIMULATION_SCHEDULE_HPP
#define ANNEALING_SIMULATION_SCHEDULE_HPP


#include <vector>
#include <random>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <concepts>
#include <list>

#include "../thirdparty/json/single_include/nlohmann/json.hpp"

#include "Task.hpp"
#include "Random.hpp"

using json = nlohmann::json;

class Task_Solution {
public:
    virtual unsigned long long get_criterion() = 0;
    [[nodiscard]] virtual unsigned long long get_procN() const = 0;
    [[nodiscard]] virtual unsigned long long get_workN() const = 0;
    std::vector<std::list<Task>> proc_tasks;
    std::vector<Task> allTasks;
    virtual ~Task_Solution() = default;
protected:
    unsigned long procN{};
    unsigned long workN{};
};

class Schedule : public Task_Solution {
public:
    Schedule(const json& tasks, bool random);
    explicit Schedule(Task_Solution* shed){
        procN = shed->get_procN();
        workN = shed->get_workN();
        proc_tasks = shed->proc_tasks;
        allTasks = shed->allTasks;
    }
    Schedule& operator=(const Schedule& other) {
        if (this != &other) {
            // Выполнить пользовательское присваивание
            procN = other.procN;
            workN = other.workN;
            proc_tasks = other.proc_tasks;
            allTasks = other.allTasks;
        }
        return *this;
    }
    unsigned long long get_criterion() override; // Less => better
    [[nodiscard]] unsigned long long get_procN() const override;
    [[nodiscard]] unsigned long long get_workN() const override;
    ~Schedule() override = default;
protected:
    unsigned long procN;
    unsigned long workN;
};


/* Mutations */

class Mutation {
public:
    virtual std::shared_ptr<Task_Solution> mutate(Task_Solution* schedule) = 0;
};

class MyMutation : Mutation {
public:
    std::shared_ptr<Task_Solution> mutate(Task_Solution* schedule) override;
};

#endif //ANNEALING_SIMULATION_SCHEDULE_HPP
