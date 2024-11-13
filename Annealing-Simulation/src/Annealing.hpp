//
// Created by viktor on 11.11.2024.
//

#ifndef ANNEALING_SIMULATION_ANNEALING_HPP
#define ANNEALING_SIMULATION_ANNEALING_HPP


#include <unordered_map>
#include <vector>
#include <thread>
#include <future>
#include <mutex>

#include "Schedule.hpp"
#include "Temperature.hpp"

/* Algorithm implementation */

template <class TSchedule>
class Annealing_Simulation {
public:
    Annealing_Simulation(TSchedule start_schedule_, Temperature start_temp_, unsigned long iter_) :
            start_schedule(start_schedule_), best_schedule(start_schedule_), current_schedule(start_schedule_),
            temperature(start_temp_), iterations(iter_) {}

    int multy_algorithm(std::mutex& sch_mutex, TSchedule& mutex_best_schedule, unsigned int int_name) {
        algorithm(int_name);
        sch_mutex.lock();
        if (mutex_best_schedule.get_criterion() > best_schedule.get_criterion()) {
            mutex_best_schedule = best_schedule;
        }
        sch_mutex.unlock();
        return 0;
    }

    TSchedule algorithm(unsigned int int_name = 0){
        unsigned long boredom_current = 0;
        unsigned long iter_current = 0;

        unsigned long long current_criterion = current_schedule.get_criterion();
        unsigned long long best_criterion = best_schedule.get_criterion();
        unsigned long long tmp_criterion;

        MyMutation mutation;

        while (boredom_current < boredom_constraint) {
            // current_schedule = best_schedule; // Modification
            iter_current++;
            for (int i = 0; i < iterations; i++) {
                boredom_current++;
                auto tmp_schedule = *dynamic_cast<TSchedule*>(mutation.mutate(&current_schedule).get());
                tmp_criterion = tmp_schedule.get_criterion();

                if ((tmp_criterion <= current_criterion) ||
                    (temperature.is_better_enough((long long)tmp_criterion - (long long)current_criterion))) {
                    if (tmp_criterion < best_criterion) {
                        best_schedule = tmp_schedule;
                        best_criterion = best_schedule.get_criterion();
                        boredom_current = 0;
                    }
                    current_schedule = tmp_schedule;
                    current_criterion = current_schedule.get_criterion();
                }
            }
//        if (int_name != 0) std::cerr << "Process NUMBER: " << int_name << "\t";
//        std::cerr << "Temperature: " << temperature.get_current_temp() << "\tIter: " << iter_current << "  \tCriterion: " << best_criterion << "\n";
            temperature.temperature_step(iter_current);
        }

        return best_schedule;
    };
private:
    const unsigned long boredom_constraint = 100;
    unsigned long iterations;

    TSchedule start_schedule;
    TSchedule current_schedule;
    TSchedule best_schedule;

    Temperature temperature;

};

Schedule
Parallel_Simulation(Schedule start_schedule_, Temperature start_temp_, unsigned long iter_,
                    unsigned long boredom_parallel_constraint_, unsigned int Nproc_);
#endif //ANNEALING_SIMULATION_ANNEALING_HPP
