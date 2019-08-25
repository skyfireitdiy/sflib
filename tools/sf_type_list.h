
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_type_list.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_type_list
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"

#include "core/sf_stdc++.h"

namespace skyfire {
template <typename... _Types>
struct sf_type_list {
    using tuple_type = std::tuple<_Types...>;
    static constexpr int size = sizeof...(_Types);
};

template <typename _Type>
struct sf_type_list<_Type> {
    using tuple_type = std::tuple<_Type>;
    static constexpr int size = 1;
    using type = _Type;
};

template <typename _Itype, typename _TypeList>
struct sf_tl_push_front;
template <typename _Itype, typename _TypeList>
struct sf_tl_push_back;
template <int _Index, typename _TypeList>
struct sf_tl_get;
template <typename _TypeList>
struct sf_tl_front;
template <typename _TypeList>
struct sf_tl_back;
template <int _Index, typename _TypeList>
struct sf_tl_left;
template <int _Index, typename _TypeList>
struct sf_tl_right;
template <typename _TypeList>
struct sf_tl_pop_front;
template <typename _TypeList>
struct sf_tl_pop_back;
template <typename _TypeList1, typename _TypeList2>
struct sf_tl_merge;
template <int _Index, int _Len, typename _TypeList>
struct sf_tl_erase;
template <int _Index, int _Len, typename _TypeList>
struct sf_tl_mid;
template <int _Pos, typename _Itype, typename _TypeList>
struct sf_tl_insert;
template <typename _Old, typename _New, typename _TypeList>
struct sf_tl_replace;
template <int _Index, typename _New, typename _TypeList>
struct sf_tl_replace_index;
template <typename _TypeList>
struct sf_tl_reverse;
template <typename _Type>
struct sf_tl_from_tuple;

template <typename _Itype, typename _TypeList>
using sf_tl_push_front_t = typename sf_tl_push_front<_Itype, _TypeList>::type;
template <typename _Itype, typename _TypeList>
using sf_tl_push_back_t = typename sf_tl_push_back<_Itype, _TypeList>::type;
template <int _Index, typename _TypeList>
using sf_tl_get_t = typename sf_tl_get<_Index, _TypeList>::type;
template <typename _TypeList>
using sf_tl_front_t = typename sf_tl_front<_TypeList>::type;
template <typename _TypeList>
using sf_tl_back_t = typename sf_tl_back<_TypeList>::type;
template <int _Index, typename _TypeList>
using sf_tl_left_t = typename sf_tl_left<_Index, _TypeList>::type;
template <int _Index, typename _TypeList>
using sf_tl_right_t = typename sf_tl_right<_Index, _TypeList>::type;
template <typename _TypeList>
using sf_tl_pop_front_t = typename sf_tl_pop_front<_TypeList>::type;
template <typename _TypeList>
using sf_tl_pop_back_t = typename sf_tl_pop_back<_TypeList>::type;
template <typename _TypeList1, typename _TypeList2>
using sf_tl_merge_t = typename sf_tl_merge<_TypeList1, _TypeList2>::type;
template <int _Index, int _Len, typename _TypeList>
using sf_tl_erase_t = typename sf_tl_erase<_Index, _Len, _TypeList>::type;
template <int _Index, int _Len, typename _TypeList>
using sf_tl_mid_t = typename sf_tl_mid<_Index, _Len, _TypeList>::type;
template <int _Pos, typename _Itype, typename _TypeList>
using sf_tl_insert_t = typename sf_tl_insert<_Pos, _Itype, _TypeList>::type;
template <typename _Old, typename _New, typename _TypeList>
using sf_tl_replace_t = typename sf_tl_replace<_Old, _New, _TypeList>::type;
template <int _Index, typename _New, typename _TypeList>
using sf_tl_replace_index_t =
    typename sf_tl_replace_index<_Index, _New, _TypeList>::type;
template <typename _TypeList>
using sf_tl_reverse_t = typename sf_tl_reverse<_TypeList>::type;
template <typename _Type>
using sf_tl_from_tuple_t = typename sf_tl_from_tuple<_Type>::type;

//////////////////////////////////////////////

template <int _Index, typename... _TypeList>
struct sf_tl_get_helper;

template <int _Index, typename... _TypeList>
struct sf_tl_left_helper;

template <int _Index, typename... _TypeList>
struct sf_tl_right_helper;

template <typename... _TypeList>
struct sf_tl_replace_helper;

template <int _Index, typename... _TypeList>
struct sf_tl_replace_index_helper;

template <typename _TypeList1, typename _TypeList2>
struct sf_tl_reverse_helper;

///////////////////////////////////////////////

template <int _Index>
struct sf_tl_get_helper<_Index> {
    using type = void;
};

template <int _Index, typename _Type, typename... _TypeList>
struct sf_tl_get_helper<_Index, _Type, _TypeList...> {
    using type = std::conditional_t<
        _Index == 0, _Type,
        typename sf_tl_get_helper<_Index - 1, _TypeList...>::type>;
};

///////

template <int _Index, typename _Type, typename... _TypeList,
          typename... _Result>
struct sf_tl_left_helper<_Index, sf_type_list<_Type, _TypeList...>,
                         sf_type_list<_Result...>> {
    using type = typename std::conditional<
        _Index == 0, sf_type_list<_Result...>,
        typename sf_tl_left_helper<_Index - 1, sf_type_list<_TypeList...>,
                                   sf_type_list<_Result..., _Type>>::type>::
        type;
};

template <typename _Type, typename... _TypeList, typename... _Result>
struct sf_tl_left_helper<-1, sf_type_list<_Type, _TypeList...>,
                         sf_type_list<_Result...>> {
    using type = sf_type_list<>;
};

template <typename... _Result>
struct sf_tl_left_helper<-1, sf_type_list<>, sf_type_list<_Result...>> {
    using type = sf_type_list<>;
};

template <>
struct sf_tl_left_helper<0, sf_type_list<>, sf_type_list<>> {
    using type = sf_type_list<>;
};

///////

template <typename _Type, typename... _TypeList>
struct sf_tl_right_helper<-1, sf_type_list<_Type, _TypeList...>> {
    using type = sf_type_list<>;
};

template <int _Index, typename _Type, typename... _TypeList>
struct sf_tl_right_helper<_Index, sf_type_list<_Type, _TypeList...>> {
    using type = typename std::conditional<
        _Index == 0, sf_type_list<_Type, _TypeList...>,
        typename sf_tl_right_helper<_Index - 1,
                                    sf_type_list<_TypeList...>>::type>::type;
};

template <typename... _TypeList>
struct sf_tl_right_helper<-1, sf_type_list<_TypeList...>> {
    using type = sf_type_list<>;
};

template <>
struct sf_tl_right_helper<0, sf_type_list<>> {
    using type = sf_type_list<>;
};

///////

template <typename _Old, typename _New, typename _Type, typename... _TypeList1,
          typename... _TypeList2>
struct sf_tl_replace_helper<_Old, _New, sf_type_list<_Type, _TypeList1...>,
                            sf_type_list<_TypeList2...>> {
    using type = typename sf_tl_replace_helper<
        _Old, _New, sf_type_list<_TypeList1...>,
        sf_type_list<_TypeList2...,
                     typename std::conditional<std::is_same_v<_Type, _Old>,
                                               _New, _Type>::type>>::type;
};

template <typename _Old, typename _New, typename... _TypeList2>
struct sf_tl_replace_helper<_Old, _New, sf_type_list<>,
                            sf_type_list<_TypeList2...>> {
    using type = sf_type_list<_TypeList2...>;
};

//////

template <int _Index, typename _New, typename _Type, typename... _TypeList1,
          typename... _TypeList2>
struct sf_tl_replace_index_helper<_Index, _New,
                                  sf_type_list<_Type, _TypeList1...>,
                                  sf_type_list<_TypeList2...>> {
    using type = typename sf_tl_replace_index_helper<
        _Index, _New, sf_type_list<_TypeList1...>,
        sf_type_list<_TypeList2...,
                     typename std::conditional<sizeof...(_TypeList2) == _Index,
                                               _New, _Type>::type>>::type;
};

template <int _Index, typename _New, typename... _TypeList2>
struct sf_tl_replace_index_helper<_Index, _New, sf_type_list<>,
                                  sf_type_list<_TypeList2...>> {
    using type = sf_type_list<_TypeList2...>;
};

////////

template <typename _Type, typename... _TypeList1, typename... _TypeList2>
struct sf_tl_reverse_helper<sf_type_list<_Type, _TypeList1...>,
                            sf_type_list<_TypeList2...>> {
    using type =
        typename sf_tl_reverse_helper<sf_type_list<_TypeList1...>,
                                      sf_type_list<_Type, _TypeList2...>>::type;
};

template <typename... _TypeList>
struct sf_tl_reverse_helper<sf_type_list<>, sf_type_list<_TypeList...>> {
    using type = sf_type_list<_TypeList...>;
};

//////////////////////////////////////////////

template <typename _Itype, typename... _TypeList>
struct sf_tl_push_front<_Itype, sf_type_list<_TypeList...>> {
    using type = sf_type_list<_Itype, _TypeList...>;
};

template <typename _Itype, typename... _TypeList>
struct sf_tl_push_back<_Itype, sf_type_list<_TypeList...>> {
    using type = sf_type_list<_TypeList..., _Itype>;
};

template <int _Index, typename... _TypeList>
struct sf_tl_get<_Index, sf_type_list<_TypeList...>> {
    using type = typename sf_tl_get_helper<_Index, _TypeList...>::type;
};

template <typename _Type, typename... _TypeList>
struct sf_tl_front<sf_type_list<_Type, _TypeList...>> {
    using type = _Type;
};

template <typename... _TypeList>
struct sf_tl_back<sf_type_list<_TypeList...>> {
    using type = typename sf_tl_get<sizeof...(_TypeList) - 1,
                                    sf_type_list<_TypeList...>>::type;
};

template <int _Index, typename... _TypeList>
struct sf_tl_left<_Index, sf_type_list<_TypeList...>> {
    using type = typename sf_tl_left_helper<_Index, sf_type_list<_TypeList...>,
                                            sf_type_list<>>::type;
};

template <int _Index, typename... _TypeList>
struct sf_tl_right<_Index, sf_type_list<_TypeList...>> {
    using type = typename sf_tl_right_helper<sizeof...(_TypeList) - _Index,
                                             sf_type_list<_TypeList...>>::type;
};

template <typename _Type, typename... _TypeList>
struct sf_tl_pop_front<sf_type_list<_Type, _TypeList...>> {
    using type = sf_type_list<_TypeList...>;
};

template <typename... _TypeList>
struct sf_tl_pop_back<sf_type_list<_TypeList...>> {
    using type = typename sf_tl_left_helper<sizeof...(_TypeList) - 1,
                                            sf_type_list<_TypeList...>,
                                            sf_type_list<>>::type;
};

template <typename... _TypeList1, typename... _TypeList2>
struct sf_tl_merge<sf_type_list<_TypeList1...>, sf_type_list<_TypeList2...>> {
    using type = sf_type_list<_TypeList1..., _TypeList2...>;
};

template <int _Index, int _Len, typename... _TypeList>
struct sf_tl_erase<_Index, _Len, sf_type_list<_TypeList...>> {
    using type = typename sf_tl_merge<
        typename sf_tl_left<_Index, sf_type_list<_TypeList...>>::type,
        typename sf_tl_right<sizeof...(_TypeList) - _Index - _Len,
                             sf_type_list<_TypeList...>>::type>::type;
};

template <int _Index, int _Len, typename... _TypeList>
struct sf_tl_mid<_Index, _Len, sf_type_list<_TypeList...>> {
    using type = typename sf_tl_right<
        _Len, typename sf_tl_left<_Index + _Len,
                                  sf_type_list<_TypeList...>>::type>::type;
};

template <int _Pos, typename _Itype, typename... _TypeList>
struct sf_tl_insert<_Pos, _Itype, sf_type_list<_TypeList...>> {
    using type = typename sf_tl_merge<
        typename sf_tl_push_back<
            _Itype, typename sf_tl_left<_Pos, sf_type_list<_TypeList...>

                                        >::type>::type,
        typename sf_tl_right<sizeof...(_TypeList) - _Pos,
                             sf_type_list<_TypeList...>>::type>::type;
};

template <typename _Old, typename _New, typename... _TypeList>
struct sf_tl_replace<_Old, _New, sf_type_list<_TypeList...>> {
    using type =
        typename sf_tl_replace_helper<_Old, _New, sf_type_list<_TypeList...>,
                                      sf_type_list<>>::type;
};

template <int _Index, typename _New, typename... _TypeList>
struct sf_tl_replace_index<_Index, _New, sf_type_list<_TypeList...>> {
    using type = typename sf_tl_replace_index_helper<
        _Index, _New, sf_type_list<_TypeList...>, sf_type_list<>>::type;
};

template <typename... _TypeList>
struct sf_tl_reverse<sf_type_list<_TypeList...>> {
    using type = typename sf_tl_reverse_helper<sf_type_list<_TypeList...>,
                                               sf_type_list<>>::type;
};

template <typename... _TypeList>
struct sf_tl_from_tuple<std::tuple<_TypeList...>> {
    using type = sf_type_list<_TypeList...>;
};

}    // namespace skyfire

#pragma clang diagnostic pop