//
// Created by viktor on 10.11.2024.
//

#ifndef ANNEALING_SIMULATION_TEMPERATURE_HPP
#define ANNEALING_SIMULATION_TEMPERATURE_HPP
#include <random>
#include <cmath>
#include <iostream>

class ChangeTemperature {
public:
    virtual double algorithm_step(double start_temp_, unsigned long iter_) = 0;
};

class Boltzmann : public ChangeTemperature {
public:
    double algorithm_step(double start_temp_, unsigned long iter_) override;
};

class Cauchy : public ChangeTemperature {
public:
    double algorithm_step(double start_temp_, unsigned long iter_) override;
};

class NatLog : public ChangeTemperature {
public:
    double algorithm_step(double start_temp_, unsigned long iter_) override;
};

class Temperature {
public:
    Temperature(ChangeTemperature* changeTemperature_, double start_temp_) :
            changeTemperature(changeTemperature_), start_temp(start_temp_), current_temp(start_temp_) {};
    void temperature_step(unsigned long iter_);
    [[nodiscard]] bool is_better_enough(long long delta) const;
    [[nodiscard]] double get_current_temp() const;
    double get_start_temp();

private:
    double start_temp;
    double current_temp;
    ChangeTemperature* changeTemperature;
};

#endif //ANNEALING_SIMULATION_TEMPERATURE_HPP
