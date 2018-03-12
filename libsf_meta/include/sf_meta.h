#pragma once

#include <functional>
#include <tuple>

template<class T>struct sf_function_type_helper{};

template<class T, class ... U>
struct sf_function_type_helper<std::function<T(U...)>>
{
    using return_type=T;
    using param_type= std::tuple<U...>;
};

template<typename T> struct sf_bind_type_helper{};

template<template<typename> class T,typename U,typename...V>
struct sf_bind_type_helper<T<U(V...)>>
{
    using return_type = U;
    using param_type = std::tuple<V...>;
};
