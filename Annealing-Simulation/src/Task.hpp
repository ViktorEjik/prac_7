//
// Created by viktor on 11.11.2024.
//

#ifndef ANNEALING_SIMULATION_TASK_HPP
#define ANNEALING_SIMULATION_TASK_HPP

struct Task {
    unsigned long number;
    unsigned long time;

    bool operator==(const Task& t1) const{
        return number == t1.number;
    }
};


#endif //ANNEALING_SIMULATION_TASK_HPP
