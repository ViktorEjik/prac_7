#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <iostream>

namespace functions {

    /* Virtual Function */

    class TFunction {
    public:
        [[nodiscard]] virtual std::string ToString() const = 0;
        virtual double Derive(double x) = 0;
        virtual double operator()(double x) = 0;
        virtual std::shared_ptr<TFunction> Copy() = 0;
    };

    /* Operation handlers */

    class Compose : public TFunction {
    public:
        Compose(TFunction& lop_, TFunction& rop_, std::string type_);
        [[nodiscard]] std::string ToString() const override;
        double Derive(double x) override;
        double operator()(double x) override;
        std::shared_ptr<TFunction> Copy() override;

    private:
        std::shared_ptr<TFunction> lop, rop;
        std::string type;
    };

    /* Others.. */

    class Exponential : public TFunction {
    public:
        Exponential() : coef(0) {};
        explicit Exponential(double coef_) : coef(coef_) {};
        explicit Exponential(std::vector<double> coef_);
        [[nodiscard]] std::string ToString() const override;
        double Derive(double x) override;
        double operator()(double x) override;
        std::shared_ptr<TFunction> Copy() override;

    protected:
        double coef;
    };

    class Constant : public TFunction{
    public:
        Constant():constant(0){};
        explicit Constant(double constant):constant(constant){};
        explicit Constant(std::vector<double> constants);
        [[nodiscard]] std::string ToString() const override;
        double Derive(double x) override{
            return 0;
        };
        double operator()(double x) override{
            return constant;
        };
        std::shared_ptr<TFunction> Copy() override;
    private:
        double constant;
    };

    class Power : public TFunction {
    public:
        Power();
        explicit Power(double coef_);
        explicit Power(std::vector<double> coef_);
        [[nodiscard]] std::string ToString() const override;
        double Derive(double x) override;
        double operator()(double x) override;
        std::shared_ptr<TFunction> Copy() override;
    private:
        double power;
    };

    class Identical : public TFunction{
    public:
        Identical() = default;
        explicit Identical(double){};
        explicit Identical(const std::vector<double>&){};
        [[nodiscard]] std::string ToString() const override{
            return "x";
        };
        double Derive(double x) override{
            return 1;
        };
        double operator()(double x) override{
            return x;
        };
        std::shared_ptr<TFunction> Copy() override;
    };

    class Polynomial : public TFunction{
    public:
        Polynomial():powerCoeffMap({{0, 0}}){};
        explicit Polynomial(double coef):powerCoeffMap({{0, coef}}){};
        explicit Polynomial(std::vector<double> coeff);
        [[nodiscard]] std::string ToString() const override;
        double Derive(double x) override;
        double operator()(double x) override;
        std::shared_ptr<TFunction> Copy() override;
    private:
        std::map<size_t, double> powerCoeffMap;
    };

    template <typename TFunction1, typename TFunction2>
    Compose operator+(TFunction1& lop, TFunction2& rop) {
        if constexpr (std::is_base_of_v<TFunction, TFunction1> && std::is_base_of_v<TFunction, TFunction2>) {
            return Compose(lop, rop, "+");
        } else {
            throw std::logic_error("One of the types is wrong!");
        }
    }

    template <typename TFunction1, typename TFunction2>
    Compose operator-(TFunction1& lop, TFunction2& rop) {
        if constexpr (std::is_base_of_v<TFunction, TFunction1> && std::is_base_of_v<TFunction, TFunction2>) {
            return Compose(lop, rop, "-");
        } else {
            std::cout << "-\n";
            throw std::logic_error("One of the types is wrong!");
        }
    }

    template <typename TFunction1, typename TFunction2>
    Compose operator*(TFunction1& lop, TFunction2& rop) {
        if constexpr (std::is_base_of_v<TFunction, TFunction1> && std::is_base_of_v<TFunction, TFunction2>) {
            return Compose(lop, rop, "*");
        } else {
            throw std::logic_error("One of the types is wrong!");
        }
    }

    template <typename TFunction1, typename TFunction2>
    Compose operator/(TFunction1& lop, TFunction2& rop) {
        if constexpr (std::is_base_of_v<TFunction, TFunction1> && std::is_base_of_v<TFunction, TFunction2>) {
            return Compose(lop, rop, "/");
        } else {
            throw std::logic_error("One of the types is wrong!");
        }
    }

/* Gradient method */

    double GoldenSelection(Compose& f2, double grad, double x, double eps, double a = 0, double b = 0.05);

    template <typename TFunction>
    double FindRoot(TFunction& f, unsigned long it, double init_x = 0, double eps = 0.00000001) {
        /* Source: https://vlab.fandom.com/ru/wiki/%D0%93%D1%80%D0%B0%D0%B4%D0%B8%D0%B5%D0%BD%D1%82%D0%BD%D1%8B%D0%B9_%D1%81%D0%BF%D1%83%D1%81%D0%BA */

        double cur_x = init_x;
        double last_x;
        auto f2 = f * f;

        for (unsigned long i = 0; i < it; i++) {
            last_x = cur_x;
            double grad = f2.Derive(cur_x);
            double lambda = GoldenSelection(f2, grad, cur_x, eps);
            cur_x = cur_x - lambda * grad;
            if (std::abs(f2(cur_x) - f2(last_x)) < eps)
                break;
        }
        return cur_x;
    }

} // functions
