#pragma once

#include <map>

#include "Functions.hpp"

#include <memory>
#include <utility>
#include <vector>

/* Factory */
namespace functions {
    class FunctionFactory {
    public:
        FunctionFactory() {
            RegisterAll();
        }

        class ICreator {
        public:
            virtual ~ICreator() = default;

            virtual auto Create() -> std::shared_ptr<TFunction> const = 0;

            virtual auto Create(double param) -> std::shared_ptr<TFunction> const = 0;

            virtual auto Create(std::vector<double> params) -> std::shared_ptr<TFunction> const = 0;
        };

        template<class Function>
        class FunctionCreator : public ICreator {
        public:
            auto Create() -> std::shared_ptr<TFunction> const override {
                return std::make_shared<Function>(Function());
            }

            auto Create(double param) -> std::shared_ptr<TFunction> const override {
                return std::make_shared<Function>(Function(param));
            }

            auto Create(std::vector<double> params) -> std::shared_ptr<TFunction> const override {
                return std::make_shared<Function>(Function(params));
            }
        };

        [[nodiscard]] std::vector<std::string> GetAvailableObjects() const {
            std::vector<std::string> RegisteredV;
            for (auto &[key, val]: RegisteredCreators) {
                RegisteredV.push_back(key);
            }
            return RegisteredV;
        }

        std::shared_ptr<TFunction> Create(const std::string &type) {
            return RegisteredCreators[type]->Create();
        }

        std::shared_ptr<TFunction> Create(const std::string &type, double param) {
            return RegisteredCreators[type]->Create(param);
        }

        std::shared_ptr<TFunction> Create(const std::string &type, std::vector<double> params) {
            return RegisteredCreators[type]->Create(std::move(params));
        }

    private:
        using TCreatorPtr = std::shared_ptr<ICreator>;
        using TRegisteredCreators = std::map<std::string, TCreatorPtr>;
        TRegisteredCreators RegisteredCreators;

        void RegisterAll() {
            RegisteredCreators["ident"] = std::make_shared<FunctionCreator<Identical>>();
            RegisteredCreators["const"] = std::make_shared<FunctionCreator<Constant>>();
            RegisteredCreators["power"] = std::make_shared<FunctionCreator<Power>>();
            RegisteredCreators["exp"] = std::make_shared<FunctionCreator<Exponential>>();
            RegisteredCreators["polynomial"] = std::make_shared<FunctionCreator<Polynomial>>();
        }

    };
}