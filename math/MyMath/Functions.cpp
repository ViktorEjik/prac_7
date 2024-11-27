//
// Created by viktor on 24.11.2024.
//

#include "Functions.hpp"
#include <cmath>
#include <utility>
#include <functional>
#include <numeric>

namespace functions {

    Compose::Compose(TFunction &lop_, TFunction &rop_, std::string type_) :
            lop(lop_.Copy()), rop(rop_.Copy()), type(std::move(type_)) {}

    double Compose::Derive(double x) {
        if (type == "+") {
            return lop->Derive(x) + rop->Derive(x);
        } else if (type == "-") {
            return lop->Derive(x) - rop->Derive(x);
        } else if (type == "*") {
            return (lop->Derive(x) * (*rop)(x) + rop->Derive(x) * (*lop)(x));
        } else if (type == "/") {
            return (lop->Derive(x) * (*rop)(x) - rop->Derive(x) * (*lop)(x)) / ((*rop)(x) * (*rop)(x));
        } else {
            throw std::logic_error("No such type!");
        }
    }

    double Compose::operator()(double x) {
        if (type == "+") {
            return (*lop)(x) + (*rop)(x);
        } else if (type == "-") {
            return (*lop)(x) - (*rop)(x);
        } else if (type == "*") {
            return (*lop)(x) * (*rop)(x);
        } else if (type == "/") {
            return (*lop)(x) / (*rop)(x);
        } else {
            throw std::logic_error("No such type!");
        }
    }

    std::string Compose::ToString() const {
        if (type == "+") {
            return lop->ToString() + " + " + rop->ToString();
        } else if (type == "-") {
            return lop->ToString() + " - " + rop->ToString();
        } else if (type == "*") {
            return "(" + lop->ToString() + ") * (" + rop->ToString() + ")";
        } else if (type == "/") {
            return "(" + lop->ToString() + ") / (" + rop->ToString() + ")";
        } else {
            throw std::logic_error("No such type!");
        }
    }

    std::shared_ptr<TFunction> Compose::Copy() {
        return std::make_shared<Compose>(*lop, *rop, type);
    }

    Exponential::Exponential(std::vector<double> coef_) : coef(coef_[0]) {
        if (!coef_.empty())
            coef = coef_[0];
        else
            coef = 0;
    }

    double Exponential::Derive(double x) {
        return (coef * exp(coef * x));
    }

    double Exponential::operator()(double x) {
        return (exp(coef * x));
    }

    std::string Exponential::ToString() const {
        if (coef == 0)
            return "1";
        else
            return "exp(" + std::to_string(coef) + "*x)";
    }

    std::shared_ptr<TFunction> Exponential::Copy() {
        return std::make_shared<Exponential>(*this);
    }


    Constant::Constant(std::vector<double> constants){
        constant = std::accumulate(constants.begin(), constants.end(), (double)0);
    }

    std::string Constant::ToString() const {
        return std::to_string(constant);
    }

    std::shared_ptr<TFunction> Constant::Copy() {
        return std::make_shared<Constant>(*this);
    }

    Power::Power() {
        power = 0;
    }

    Power::Power(double coef_) {
        power = coef_;
    }

    double Power::Derive(double x) {
        return power * std::pow(x, power - 1);
    }

    std::string Power::ToString() const {
        return "x^" + std::to_string(power);
    }

    double Power::operator()(double x) {
        return std::pow(x, power);
    }

    std::shared_ptr<TFunction> Power::Copy() {
        return std::make_shared<Power>(*this);
    }

    Power::Power(std::vector<double> coef_) {
        if(coef_.empty()) throw std::invalid_argument("Coef must be not empty");
        power = coef_.front();
    }

    Polynomial::Polynomial(std::vector<double> coeff) {
        if(coeff.empty()) throw std::invalid_argument("Coef must be not empty");
        for(size_t i = 0; i < coeff.size(); ++i){
            powerCoeffMap.insert({i, coeff[i]});
        }
    }

    std::string Polynomial::ToString() const {
        std::vector<std::string> vector;
        for(auto&[power, coeff]: powerCoeffMap){
            if (std::fabs( coeff - 0) > std::numeric_limits<double>::epsilon()) {
                if (power == 1)
                    vector.push_back(std::to_string(coeff) + '*' + "x");
                else if (power != 0)
                    vector.push_back(std::to_string(coeff) + '*' + "x^" + std::to_string(power));
                else
                    vector.push_back(std::to_string(coeff));
            }
        }
        std::string str;
        for(auto iter = vector.begin(); iter != std::prev(vector.end()); ++iter){
            str += *iter;
            str += " + ";
        }
        str += vector.back();

        return str;
    }

    double Polynomial::Derive(double x) {
        double result = 0;
        for(auto&[power, coeff]: powerCoeffMap){
            if(power != 0)
                result += coeff * power * std::pow(x, power - 1);
        }
        return result;
    }

    double Polynomial::operator()(double x) {
        double result = 0;
        for(auto&[power, coeff]: powerCoeffMap){
            result += coeff * std::pow(x, power);
        }
        return result;
    }

    std::shared_ptr<TFunction> Polynomial::Copy() {
        return std::make_shared<Polynomial>(*this);
    }

    std::shared_ptr<TFunction> Identical::Copy() {
        return std::make_shared<Identical>(*this);
    }
} // functions