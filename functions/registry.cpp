#include "registry.h"
#include "functionholder.h"

#include "somefunctions.h"

#include <map>
#include <tuple>

namespace holder {

struct Registry::Impl
{
    template <typename... Args>
    using Callable = func::FunctionHolder<Args...>;

    const std::tuple
        <
            Callable<std::string, const std::string&, const std::string&, std::string>,
            Callable<double, int, double>
        >
        FUNCTIONS_REGISTRY =
        std::make_tuple(
            Callable<std::string, const std::string&, const std::string&, std::string>{findFile},
            Callable<double, int, double>{divide}
        );

#define STRINGIFY(fun) #fun
    const std::map<std::string, int> FUNCTIONS_MAP {
        {STRINGIFY(findFile), 0},
        {STRINGIFY(divide)  , 1}
    };
#undef STRINGIFY

    int getFunNum(const std::string &name) {
    if (FUNCTIONS_MAP.find(name) == FUNCTIONS_MAP.end())
        { return -1; }
    return FUNCTIONS_MAP.at(name);
    }
};

#define GET_FUN_RESULT_TO_STREAM(stream, funNum, args) \
    case funNum: stream << std::get<funNum>(impl_->FUNCTIONS_REGISTRY)(args); break

std::string Registry::call(const std::string &funName, std::list<std::string> &args)
{
    std::ostringstream os;
    switch (impl_->getFunNum(funName)) {
        GET_FUN_RESULT_TO_STREAM(os, 0, args);
        GET_FUN_RESULT_TO_STREAM(os, 1, args);
        default: os << "Unregistered function";
    }
    return os.str();
}
#undef GET_FUN_RESULT_TO_STREAM

Registry::Registry()
    : impl_{std::make_unique<Impl>()}
    {}

Registry &Registry::registry()
{
    static Registry reg{};
    return reg;
}

} // namespace holder
