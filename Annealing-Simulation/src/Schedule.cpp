//
// Created by viktor on 11.11.2024.
//

#include "Schedule.hpp"
#include <cmath>

Schedule::Schedule(const json& tasks, bool random) {
    /* Read from csv file*/

    workN = tasks["Works"].size();
    procN = tasks["NProses"];
    proc_tasks = std::vector<std::list<Task>>(procN);

    /* Random engine */

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long> distrib(0, procN - 1);

    for (unsigned long i = 0; i < workN; i++) {
        Task tmp = {tasks["Works"][i][0], tasks["Works"][i][1]};
        allTasks.push_back(tmp);
        if (random) {
            proc_tasks[distrib(gen)].push_back(tmp);
        } else {
            proc_tasks[0].push_back(tmp);
        }
    }
}

unsigned long long Schedule::get_criterion() {
    size_t i = 0;
    for(;i < proc_tasks.size() && proc_tasks[i].empty() ; i++);
    unsigned long long minTimeEnd = proc_tasks[i].front().time;
    unsigned long long maxTimeEnd = 0;

    for(const auto& tasks: proc_tasks){
        if(minTimeEnd > tasks.front().time) minTimeEnd = tasks.front().time;
        uint64_t sum = std::accumulate(tasks.begin(), tasks.end(), (uint64_t)0,[](auto sum, auto elem){
            return elem.time + sum;
        });
        if (maxTimeEnd < sum) maxTimeEnd = sum;
    }

    return maxTimeEnd - minTimeEnd;
}

unsigned long long Schedule::get_procN() const {
    return procN;
}

unsigned long long Schedule::get_workN() const {
    return workN;
}

std::shared_ptr<Task_Solution> MyMutation::mutate(Task_Solution* schedule) {
    auto newSchedule = std::shared_ptr<Task_Solution>(new Schedule(schedule));
    if (newSchedule->get_procN() == 1) {
        auto moving_work = select_randomly(newSchedule->allTasks.begin(), newSchedule->allTasks.end());
        auto target_work = select_randomly(newSchedule->allTasks.begin(), newSchedule->allTasks.end());
        auto tmp =  *moving_work;
        *moving_work = *target_work;
        *target_work = tmp;
        return newSchedule;
    }

    /* Rearrange 1.% + 1 of works */

    for (unsigned long i = 1; i <= newSchedule->get_workN() / 100 + 1; i++) {
        auto moving_workIt = select_randomly(newSchedule->allTasks.begin(), newSchedule->allTasks.end());
        auto movingWorkValue = *moving_workIt;
        for (unsigned long j = 0; j < newSchedule->get_procN(); j++) {
            auto findIt = std::find(newSchedule->proc_tasks[j].begin(), newSchedule->proc_tasks[j].end(), movingWorkValue);
            if (findIt != newSchedule->proc_tasks[j].end()) {
                newSchedule->proc_tasks[j].erase(findIt);
                break;
            }
        }
        auto target_proc = select_randomly(newSchedule->proc_tasks.begin(), newSchedule->proc_tasks.end());
        if (target_proc->empty()) {
            target_proc->push_back(movingWorkValue);
        } else {
            auto target_position = select_randomly(target_proc->begin(), target_proc->end());
            target_proc->insert(target_position, movingWorkValue);
        }
    }

    return newSchedule;
}
