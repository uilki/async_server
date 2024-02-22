#ifndef __FXLJISTJFJZXMG2IXULUSN68AYR1G6RAGGB7S82LB110954XOAI7TK61RHWMIQB3N5HX8BXN3__H__
#define __FXLJISTJFJZXMG2IXULUSN68AYR1G6RAGGB7S82LB110954XOAI7TK61RHWMIQB3N5HX8BXN3__H__

#include <functional>
#include <list>
#include <sstream>
#include <string>

namespace func {
struct BadHolderCall : std::exception {
    std::string msg_;

public:
    explicit BadHolderCall(const char *msg) : msg_{msg} {}
    const char *what() const noexcept { return msg_.c_str(); }
};

template <typename T>
T deserialize(const std::string &str)
{
    std::istringstream is{str};
    T value;
    is >> value;
    if (is.fail() || is.rdbuf()->in_avail())
        { throw BadHolderCall{"Can't deserialize argument"}; }
    return value;
}

struct NullType {};

// type list
template <typename First, typename... Rest>
struct TypeList;

template <typename Last>
struct TypeList<Last>
{
    using Head = Last;
    using Tail = NullType;
    enum : uint { index = 0 };
};

template <typename First, typename... Rest>
struct TypeList
{
    using Head = First;
    using Tail = TypeList<Rest...>;
    enum : uint { index = 1 + Tail::index };
};

using ArgList = std::list<std::string>;

template <typename ReturnType, typename... Args>
class FunctionHolder
{
    // unwind argument list
    template <typename TList, typename... Values>
    struct Call;

    template <typename... Values>
    struct Call<NullType, Values...>
    {
        static ReturnType call(const FunctionHolder *fun, ArgList /*others*/, Values... values)
            { return fun->callable_(values...); }
    };

    template <typename List, typename... Values>
    struct Call
    {
        static ReturnType call(const FunctionHolder *fun, ArgList args, Values... values)
        {
            using argument_t = typename std::decay<typename List::Head>::type;

            auto next = deserialize<argument_t>(args.front());
            args.pop_front();

            return Call<typename List::Tail, Values..., argument_t>::call(fun, args, values..., next);
        }
    };

    using Fun      = std::function<ReturnType(Args...)>;
    using ArgTypes = TypeList<Args...>;

    Fun callable_;

public:
    explicit FunctionHolder(const Fun &f)
        : callable_{f}
        {}

    ReturnType operator()(const ArgList &args) const
    {
        if (args.size() != ArgTypes::index + 1)
            { throw BadHolderCall{"Wrong args number"}; }
        return Call<ArgTypes>::call(this, args);
    }
};
} // namespace func

#endif // __FXLJISTJFJZXMG2IXULUSN68AYR1G6RAGGB7S82LB110954XOAI7TK61RHWMIQB3N5HX8BXN3__H__
