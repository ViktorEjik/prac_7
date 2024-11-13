//
// Created by viktor on 11.11.2024.
//

#include "Annealing.hpp"

//template <class Schedule>
Schedule
Parallel_Simulation(Schedule start_schedule_, Temperature start_temp_, unsigned long iter_,
                    const unsigned long boredom_parallel_constraint_, unsigned int Nproc_) {
    unsigned long current_boredom_parallel = 0;

    Schedule mutex_best_schedule = start_schedule_;
    auto current_schedule = start_schedule_;
    Schedule best = start_schedule_;
    std::mutex sch_mutex;

    while (current_boredom_parallel < boredom_parallel_constraint_) {
        std::vector<Annealing_Simulation<Schedule>> simulations_vec;
        std::vector<std::thread> thread_vec;

        for (unsigned int i = 0; i < Nproc_; i++) {
            simulations_vec.push_back(Annealing_Simulation<Schedule>(current_schedule, start_temp_, iter_));
        }

        for (unsigned int i = 0; i < Nproc_; i++) {
            thread_vec.push_back(std::thread {
                [&mutex_best_schedule, &simulations_vec, &sch_mutex, i] {
                    simulations_vec[i].multy_algorithm(sch_mutex, mutex_best_schedule, i+1);
                }
            });

        }
        for (unsigned int i = 0; i < Nproc_; i++) {
            thread_vec[i].join();
        }
//        std::cout << std::boolalpha << (start_schedule.get_criterion() <= mutex_best_schedule.get_criterion()) << ' ' << start_schedule.get_criterion() << ' '<< mutex_best_schedule.get_criterion() << std::endl;
        if (best.get_criterion() > mutex_best_schedule.get_criterion()) {
            current_boredom_parallel = 0;
            best = mutex_best_schedule;
        } else {
            current_boredom_parallel++;
        }
        current_schedule = mutex_best_schedule;
    }

    return best;
}
