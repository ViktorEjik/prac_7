//
// Created by viktor on 10.11.2024.
//

#include "Temperature.hpp"

double Boltzmann::algorithm_step(double start_temp_, unsigned long iter_) {
    return start_temp_ / std::log(iter_ + 1);
}

double Cauchy::algorithm_step(double start_temp_, unsigned long iter_) {
    return start_temp_ / (iter_ + 1);
}

double NatLog::algorithm_step(double start_temp_, unsigned long iter_) {
    return start_temp_ * std::log(iter_ + 1) / (iter_ + 1);
}

void Temperature::temperature_step(unsigned long iter_) {
    current_temp = changeTemperature->algorithm_step(start_temp, iter_);
}

bool Temperature::is_better_enough(long long delta) const {
    return ((static_cast<double> (random()) / static_cast<double> (RAND_MAX)) > exp(-static_cast<double>(delta) / current_temp));
}

double Temperature::get_current_temp() const {
    return current_temp;
}

double Temperature::get_start_temp() {
    return start_temp;
}