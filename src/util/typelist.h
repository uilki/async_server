#ifndef __IO1HQI2YQGBJM0S64QB4NUUO4ZYO1E6BN96F8XGHA7A7CQTQWO4CAA342AAMWP10VDQQ8KNVY__H__
#define __IO1HQI2YQGBJM0S64QB4NUUO4ZYO1E6BN96F8XGHA7A7CQTQWO4CAA342AAMWP10VDQQ8KNVY__H__

namespace util::details {
struct NullType {};

// type list
template <typename First, typename... Rest>
struct TypeList;

template <typename Last>
struct TypeList<Last>
{
    using Head = Last;
    using Tail = NullType;
    enum : unsigned { index = 0 };
};

template <typename First, typename... Rest>
struct TypeList
{
    using Head = First;
    using Tail = TypeList<Rest...>;
    enum : unsigned { index = 1 + Tail::index };
};
}

#endif // __IO1HQI2YQGBJM0S64QB4NUUO4ZYO1E6BN96F8XGHA7A7CQTQWO4CAA342AAMWP10VDQQ8KNVY__H__
