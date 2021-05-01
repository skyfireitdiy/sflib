
#pragma once
#include <bits/stdc++.h>
namespace skyfire
{
template <typename... _Types>
struct type_list
{
    using tuple_type          = std::tuple<_Types...>;
    static constexpr int size = sizeof...(_Types);
};
template <typename _Type>
struct type_list<_Type>
{
    using tuple_type          = std::tuple<_Type>;
    static constexpr int size = 1;
    using type                = _Type;
};
template <typename _Itype, typename _TypeList>
struct tl_push_front;
template <typename _Itype, typename _TypeList>
struct tl_push_back;
template <int _Index, typename _TypeList>
struct tl_get;
template <typename _TypeList>
struct tl_front;
template <typename _TypeList>
struct tl_back;
template <int _Index, typename _TypeList>
struct tl_left;
template <int _Index, typename _TypeList>
struct tl_right;
template <typename _TypeList>
struct tl_pop_front;
template <typename _TypeList>
struct tl_pop_back;
template <typename _TypeList1, typename _TypeList2>
struct tl_merge;
template <int _Index, int _Len, typename _TypeList>
struct tl_erase;
template <int _Index, int _Len, typename _TypeList>
struct tl_mid;
template <int _Pos, typename _Itype, typename _TypeList>
struct tl_insert;
template <typename _Old, typename _New, typename _TypeList>
struct tl_replace;
template <int _Index, typename _New, typename _TypeList>
struct tl_replace_index;
template <typename _TypeList>
struct tl_reverse;
template <typename _Type>
struct tl_from_tuple;
template <typename _Itype, typename _TypeList>
using tl_push_front_t = typename tl_push_front<_Itype, _TypeList>::type;
template <typename _Itype, typename _TypeList>
using tl_push_back_t = typename tl_push_back<_Itype, _TypeList>::type;
template <int _Index, typename _TypeList>
using tl_get_t = typename tl_get<_Index, _TypeList>::type;
template <typename _TypeList>
using tl_front_t = typename tl_front<_TypeList>::type;
template <typename _TypeList>
using tl_back_t = typename tl_back<_TypeList>::type;
template <int _Index, typename _TypeList>
using tl_left_t = typename tl_left<_Index, _TypeList>::type;
template <int _Index, typename _TypeList>
using tl_right_t = typename tl_right<_Index, _TypeList>::type;
template <typename _TypeList>
using tl_pop_front_t = typename tl_pop_front<_TypeList>::type;
template <typename _TypeList>
using tl_pop_back_t = typename tl_pop_back<_TypeList>::type;
template <typename _TypeList1, typename _TypeList2>
using tl_merge_t = typename tl_merge<_TypeList1, _TypeList2>::type;
template <int _Index, int _Len, typename _TypeList>
using tl_erase_t = typename tl_erase<_Index, _Len, _TypeList>::type;
template <int _Index, int _Len, typename _TypeList>
using tl_mid_t = typename tl_mid<_Index, _Len, _TypeList>::type;
template <int _Pos, typename _Itype, typename _TypeList>
using tl_insert_t = typename tl_insert<_Pos, _Itype, _TypeList>::type;
template <typename _Old, typename _New, typename _TypeList>
using tl_replace_t = typename tl_replace<_Old, _New, _TypeList>::type;
template <int _Index, typename _New, typename _TypeList>
using tl_replace_index_t =
    typename tl_replace_index<_Index, _New, _TypeList>::type;
template <typename _TypeList>
using tl_reverse_t = typename tl_reverse<_TypeList>::type;
template <typename _Type>
using tl_from_tuple_t = typename tl_from_tuple<_Type>::type;
//////////////////////////////////////////////
template <int _Index, typename... _TypeList>
struct tl_get_helper;
template <int _Index, typename... _TypeList>
struct tl_left_helper;
template <int _Index, typename... _TypeList>
struct tl_right_helper;
template <typename... _TypeList>
struct tl_replace_helper;
template <int _Index, typename... _TypeList>
struct tl_replace_index_helper;
template <typename _TypeList1, typename _TypeList2>
struct tl_reverse_helper;
///////////////////////////////////////////////
template <int _Index>
struct tl_get_helper<_Index>
{
    using type = void;
};
template <int _Index, typename _Type, typename... _TypeList>
struct tl_get_helper<_Index, _Type, _TypeList...>
{
    using type = std::conditional_t<
        _Index == 0, _Type,
        typename tl_get_helper<_Index - 1, _TypeList...>::type>;
};
///////
template <int _Index, typename _Type, typename... _TypeList,
          typename... _Result>
struct tl_left_helper<_Index, type_list<_Type, _TypeList...>,
                      type_list<_Result...>>
{
    using type = typename std::conditional<
        _Index == 0, type_list<_Result...>,
        typename tl_left_helper<_Index - 1, type_list<_TypeList...>,
                                type_list<_Result..., _Type>>::type>::
        type;
};
template <typename _Type, typename... _TypeList, typename... _Result>
struct tl_left_helper<-1, type_list<_Type, _TypeList...>,
                      type_list<_Result...>>
{
    using type = type_list<>;
};
template <typename... _Result>
struct tl_left_helper<-1, type_list<>, type_list<_Result...>>
{
    using type = type_list<>;
};
template <>
struct tl_left_helper<0, type_list<>, type_list<>>
{
    using type = type_list<>;
};
///////
template <typename _Type, typename... _TypeList>
struct tl_right_helper<-1, type_list<_Type, _TypeList...>>
{
    using type = type_list<>;
};
template <int _Index, typename _Type, typename... _TypeList>
struct tl_right_helper<_Index, type_list<_Type, _TypeList...>>
{
    using type = typename std::conditional<
        _Index == 0, type_list<_Type, _TypeList...>,
        typename tl_right_helper<_Index - 1,
                                 type_list<_TypeList...>>::type>::type;
};
template <typename... _TypeList>
struct tl_right_helper<-1, type_list<_TypeList...>>
{
    using type = type_list<>;
};
template <>
struct tl_right_helper<0, type_list<>>
{
    using type = type_list<>;
};
///////
template <typename _Old, typename _New, typename _Type, typename... _TypeList1,
          typename... _TypeList2>
struct tl_replace_helper<_Old, _New, type_list<_Type, _TypeList1...>,
                         type_list<_TypeList2...>>
{
    using type = typename tl_replace_helper<
        _Old, _New, type_list<_TypeList1...>,
        type_list<_TypeList2...,
                  typename std::conditional<std::is_same_v<_Type, _Old>,
                                            _New, _Type>::type>>::type;
};
template <typename _Old, typename _New, typename... _TypeList2>
struct tl_replace_helper<_Old, _New, type_list<>,
                         type_list<_TypeList2...>>
{
    using type = type_list<_TypeList2...>;
};
//////
template <int _Index, typename _New, typename _Type, typename... _TypeList1,
          typename... _TypeList2>
struct tl_replace_index_helper<_Index, _New,
                               type_list<_Type, _TypeList1...>,
                               type_list<_TypeList2...>>
{
    using type = typename tl_replace_index_helper<
        _Index, _New, type_list<_TypeList1...>,
        type_list<_TypeList2...,
                  typename std::conditional<sizeof...(_TypeList2) == _Index,
                                            _New, _Type>::type>>::type;
};
template <int _Index, typename _New, typename... _TypeList2>
struct tl_replace_index_helper<_Index, _New, type_list<>,
                               type_list<_TypeList2...>>
{
    using type = type_list<_TypeList2...>;
};
////////
template <typename _Type, typename... _TypeList1, typename... _TypeList2>
struct tl_reverse_helper<type_list<_Type, _TypeList1...>,
                         type_list<_TypeList2...>>
{
    using type =
        typename tl_reverse_helper<type_list<_TypeList1...>,
                                   type_list<_Type, _TypeList2...>>::type;
};
template <typename... _TypeList>
struct tl_reverse_helper<type_list<>, type_list<_TypeList...>>
{
    using type = type_list<_TypeList...>;
};
//////////////////////////////////////////////
template <typename _Itype, typename... _TypeList>
struct tl_push_front<_Itype, type_list<_TypeList...>>
{
    using type = type_list<_Itype, _TypeList...>;
};
template <typename _Itype, typename... _TypeList>
struct tl_push_back<_Itype, type_list<_TypeList...>>
{
    using type = type_list<_TypeList..., _Itype>;
};
template <int _Index, typename... _TypeList>
struct tl_get<_Index, type_list<_TypeList...>>
{
    using type = typename tl_get_helper<_Index, _TypeList...>::type;
};
template <typename _Type, typename... _TypeList>
struct tl_front<type_list<_Type, _TypeList...>>
{
    using type = _Type;
};
template <typename... _TypeList>
struct tl_back<type_list<_TypeList...>>
{
    using type = typename tl_get<sizeof...(_TypeList) - 1,
                                 type_list<_TypeList...>>::type;
};
template <int _Index, typename... _TypeList>
struct tl_left<_Index, type_list<_TypeList...>>
{
    using type = typename tl_left_helper<_Index, type_list<_TypeList...>,
                                         type_list<>>::type;
};
template <int _Index, typename... _TypeList>
struct tl_right<_Index, type_list<_TypeList...>>
{
    using type = typename tl_right_helper<sizeof...(_TypeList) - _Index,
                                          type_list<_TypeList...>>::type;
};
template <typename _Type, typename... _TypeList>
struct tl_pop_front<type_list<_Type, _TypeList...>>
{
    using type = type_list<_TypeList...>;
};
template <typename... _TypeList>
struct tl_pop_back<type_list<_TypeList...>>
{
    using type = typename tl_left_helper<sizeof...(_TypeList) - 1,
                                         type_list<_TypeList...>,
                                         type_list<>>::type;
};
template <typename... _TypeList1, typename... _TypeList2>
struct tl_merge<type_list<_TypeList1...>, type_list<_TypeList2...>>
{
    using type = type_list<_TypeList1..., _TypeList2...>;
};
template <int _Index, int _Len, typename... _TypeList>
struct tl_erase<_Index, _Len, type_list<_TypeList...>>
{
    using type = typename tl_merge<
        typename tl_left<_Index, type_list<_TypeList...>>::type,
        typename tl_right<sizeof...(_TypeList) - _Index - _Len,
                          type_list<_TypeList...>>::type>::type;
};
template <int _Index, int _Len, typename... _TypeList>
struct tl_mid<_Index, _Len, type_list<_TypeList...>>
{
    using type = typename tl_right<
        _Len, typename tl_left<_Index + _Len, type_list<_TypeList...>>::type>::type;
};
template <int _Pos, typename _Itype, typename... _TypeList>
struct tl_insert<_Pos, _Itype, type_list<_TypeList...>>
{
    using type = typename tl_merge<
        typename tl_push_back<
            _Itype, typename tl_left<_Pos, type_list<_TypeList...>>::type>::type,
        typename tl_right<sizeof...(_TypeList) - _Pos,
                          type_list<_TypeList...>>::type>::type;
};
template <typename _Old, typename _New, typename... _TypeList>
struct tl_replace<_Old, _New, type_list<_TypeList...>>
{
    using type =
        typename tl_replace_helper<_Old, _New, type_list<_TypeList...>,
                                   type_list<>>::type;
};
template <int _Index, typename _New, typename... _TypeList>
struct tl_replace_index<_Index, _New, type_list<_TypeList...>>
{
    using type = typename tl_replace_index_helper<
        _Index, _New, type_list<_TypeList...>, type_list<>>::type;
};
template <typename... _TypeList>
struct tl_reverse<type_list<_TypeList...>>
{
    using type = typename tl_reverse_helper<type_list<_TypeList...>,
                                            type_list<>>::type;
};
template <typename... _TypeList>
struct tl_from_tuple<std::tuple<_TypeList...>>
{
    using type = type_list<_TypeList...>;
};
} // namespace skyfire
