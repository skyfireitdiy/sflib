
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_define.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

#include <cstddef>

#ifdef __GNUC__
#if __GNUC__ < 7
#error Please update your complier!
#endif
#endif

#define SF_PRIVATE_ARGS_GLUE(x, y) x y
#define SF_PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...)       \
    N
#define SF_PRIVATE_MACRO_VAR_ARGS_IMPL(args) \
    SF_PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT args
#define SF_COUNT_MACRO_VAR_ARGS(...) \
    SF_PRIVATE_MACRO_VAR_ARGS_IMPL(  \
        (__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define SF_PRIVATE_MACRO_CHOOSE_HELPER2(M, count) M##count
#define SF_PRIVATE_MACRO_CHOOSE_HELPER1(M, count) \
    SF_PRIVATE_MACRO_CHOOSE_HELPER2(M, count)
#define SF_PRIVATE_MACRO_CHOOSE_HELPER(M, count) \
    SF_PRIVATE_MACRO_CHOOSE_HELPER1(M, count)
#define SF_INVOKE_VAR_MACRO(M, ...)                                    \
    SF_PRIVATE_ARGS_GLUE(SF_PRIVATE_MACRO_CHOOSE_HELPER(               \
                             M, SF_COUNT_MACRO_VAR_ARGS(__VA_ARGS__)), \
        (__VA_ARGS__))

#define SF_EXPAND_ARGS_2(prefix, _1) prefix._1
#define SF_EXPAND_ARGS_3(prefix, _1, _2) prefix._1, prefix._2
#define SF_EXPAND_ARGS_4(prefix, _1, _2, _3) prefix._1, prefix._2, prefix._3
#define SF_EXPAND_ARGS_5(prefix, _1, _2, _3, _4) \
    prefix._1, prefix._2, prefix._3, prefix._4
#define SF_EXPAND_ARGS_6(prefix, _1, _2, _3, _4, _5) \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5
#define SF_EXPAND_ARGS_7(prefix, _1, _2, _3, _4, _5, _6) \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6
#define SF_EXPAND_ARGS_8(prefix, _1, _2, _3, _4, _5, _6, _7) \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, prefix._7
#define SF_EXPAND_ARGS_9(prefix, _1, _2, _3, _4, _5, _6, _7, _8)      \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, \
        prefix._7, prefix._8
#define SF_EXPAND_ARGS_10(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9) \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6, \
        prefix._7, prefix._8, prefix._9
#define SF_EXPAND_ARGS_11(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,      \
        prefix._7, prefix._8, prefix._9, prefix._10
#define SF_EXPAND_ARGS_12(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
    _11)                                                                   \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,      \
        prefix._7, prefix._8, prefix._9, prefix._10, prefix._11
#define SF_EXPAND_ARGS_13(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
    _11, _12)                                                              \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,      \
        prefix._7, prefix._8, prefix._9, prefix._10, prefix._11, prefix._12
#define SF_EXPAND_ARGS_14(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,   \
    _11, _12, _13)                                                           \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,        \
        prefix._7, prefix._8, prefix._9, prefix._10, prefix._11, prefix._12, \
        prefix._13
#define SF_EXPAND_ARGS_15(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,   \
    _11, _12, _13, _14)                                                      \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,        \
        prefix._7, prefix._8, prefix._9, prefix._10, prefix._11, prefix._12, \
        prefix._13, prefix._14
#define SF_EXPAND_ARGS_16(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,   \
    _11, _12, _13, _14, _15)                                                 \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,        \
        prefix._7, prefix._8, prefix._9, prefix._10, prefix._11, prefix._12, \
        prefix._13, prefix._14, prefix._15
#define SF_EXPAND_ARGS_17(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,   \
    _11, _12, _13, _14, _15, _16)                                            \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,        \
        prefix._7, prefix._8, prefix._9, prefix._10, prefix._11, prefix._12, \
        prefix._13, prefix._14, prefix._15, prefix._16
#define SF_EXPAND_ARGS_18(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,   \
    _11, _12, _13, _14, _15, _16, _17)                                       \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,        \
        prefix._7, prefix._8, prefix._9, prefix._10, prefix._11, prefix._12, \
        prefix._13, prefix._14, prefix._15, prefix._16, prefix._17
#define SF_EXPAND_ARGS_19(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,   \
    _11, _12, _13, _14, _15, _16, _17, _18)                                  \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,        \
        prefix._7, prefix._8, prefix._9, prefix._10, prefix._11, prefix._12, \
        prefix._13, prefix._14, prefix._15, prefix._16, prefix._17, prefix._18
#define SF_EXPAND_ARGS_20(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,   \
    _11, _12, _13, _14, _15, _16, _17, _18, _19)                             \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,        \
        prefix._7, prefix._8, prefix._9, prefix._10, prefix._11, prefix._12, \
        prefix._13, prefix._14, prefix._15, prefix._16, prefix._17,          \
        prefix._18, prefix._19
#define SF_EXPAND_ARGS_21(prefix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,   \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20)                        \
    prefix._1, prefix._2, prefix._3, prefix._4, prefix._5, prefix._6,        \
        prefix._7, prefix._8, prefix._9, prefix._10, prefix._11, prefix._12, \
        prefix._13, prefix._14, prefix._15, prefix._16, prefix._17,          \
        prefix._18, prefix._19, prefix._20

#define SF_EXPAND_OBJ_MEM(...) SF_INVOKE_VAR_MACRO(SF_EXPAND_ARGS_, __VA_ARGS__)

#define SF_EXPAND_ARGS_WITH_NAME_2(prefix, _1) std::string(#_1), prefix._1
#define SF_EXPAND_ARGS_WITH_NAME_3(prefix, _1, _2) \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2
#define SF_EXPAND_ARGS_WITH_NAME_4(prefix, _1, _2, _3)        \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2, \
        std::string(#_3), prefix._3
#define SF_EXPAND_ARGS_WITH_NAME_5(prefix, _1, _2, _3, _4)    \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2, \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4
#define SF_EXPAND_ARGS_WITH_NAME_6(prefix, _1, _2, _3, _4, _5)    \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,     \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4, \
        std::string(#_5), prefix._5
#define SF_EXPAND_ARGS_WITH_NAME_7(prefix, _1, _2, _3, _4, _5, _6) \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,      \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,  \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6
#define SF_EXPAND_ARGS_WITH_NAME_8(prefix, _1, _2, _3, _4, _5, _6, _7) \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,          \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,      \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,      \
        std::string(#_7), prefix._7
#define SF_EXPAND_ARGS_WITH_NAME_9(prefix, _1, _2, _3, _4, _5, _6, _7, _8) \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,              \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,          \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,          \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8
#define SF_EXPAND_ARGS_WITH_NAME_10(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9)                                                                     \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9
#define SF_EXPAND_ARGS_WITH_NAME_11(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10)                                                                \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10
#define SF_EXPAND_ARGS_WITH_NAME_12(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11)                                                           \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11
#define SF_EXPAND_ARGS_WITH_NAME_13(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12)                                                      \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12
#define SF_EXPAND_ARGS_WITH_NAME_14(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13)                                                 \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13
#define SF_EXPAND_ARGS_WITH_NAME_15(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14)                                            \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14
#define SF_EXPAND_ARGS_WITH_NAME_16(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15)                                       \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14,       \
        std::string(#_15), prefix._15
#define SF_EXPAND_ARGS_WITH_NAME_17(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16)                                  \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14,       \
        std::string(#_15), prefix._15, std::string(#_16), prefix._16
#define SF_EXPAND_ARGS_WITH_NAME_18(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16,                                  \
    _17)                                                                    \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14,       \
        std::string(#_15), prefix._15, std::string(#_16), prefix._16,       \
        std::string(#_17), prefix._17
#define SF_EXPAND_ARGS_WITH_NAME_19(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16,                                  \
    _17, _18)                                                               \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14,       \
        std::string(#_15), prefix._15, std::string(#_16), prefix._16,       \
        std::string(#_17), prefix._17, std::string(#_18), prefix._18
#define SF_EXPAND_ARGS_WITH_NAME_20(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16,                                  \
    _17, _18, _19)                                                          \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14,       \
        std::string(#_15), prefix._15, std::string(#_16), prefix._16,       \
        std::string(#_17), prefix._17, std::string(#_18), prefix._18,       \
        std::string(#_19), prefix._19
#define SF_EXPAND_ARGS_WITH_NAME_21(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16,                                  \
    _17, _18, _19, _20)                                                     \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14,       \
        std::string(#_15), prefix._15, std::string(#_16), prefix._16,       \
        std::string(#_17), prefix._17, std::string(#_18), prefix._18,       \
        std::string(#_19), prefix._19, std::string(#_20), prefix._20

#define SF_EXPAND_ARGS_WITH_NAME_22(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16,                                  \
    _17, _18, _19, _20, _21)                                                \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14,       \
        std::string(#_15), prefix._15, std::string(#_16), prefix._16,       \
        std::string(#_17), prefix._17, std::string(#_18), prefix._18,       \
        std::string(#_19), prefix._19, std::string(#_20), prefix._20,       \
        sstd::string(#_21), prefix._21

#define SF_EXPAND_ARGS_WITH_NAME_23(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16,                                  \
    _17, _18, _19, _20, _21, _22)                                           \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14,       \
        std::string(#_15), prefix._15, std::string(#_16), prefix._16,       \
        std::string(#_17), prefix._17, std::string(#_18), prefix._18,       \
        std::string(#_19), prefix._19, std::string(#_20), prefix._20,       \
        std::string(#_21), prefix._21, std::string(#_22), prefix._22

#define SF_EXPAND_ARGS_WITH_NAME_24(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16,                                  \
    _17, _18, _19, _20, _21, _22, _23)                                      \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14,       \
        std::string(#_15), prefix._15, std::string(#_16), prefix._16,       \
        std::string(#_17), prefix._17, std::string(#_18), prefix._18,       \
        std::string(#_19), prefix._19, std::string(#_20), prefix._20,       \
        std::string(#_21), prefix._21, std::string(#_22), prefix._22,       \
        std::string(#_23), prefix._23

#define SF_EXPAND_ARGS_WITH_NAME_25(prefix, _1, _2, _3, _4, _5, _6, _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16,                                  \
    _17, _18, _19, _20, _21, _22, _23, _24)                                 \
    std::string(#_1), prefix._1, std::string(#_2), prefix._2,               \
        std::string(#_3), prefix._3, std::string(#_4), prefix._4,           \
        std::string(#_5), prefix._5, std::string(#_6), prefix._6,           \
        std::string(#_7), prefix._7, std::string(#_8), prefix._8,           \
        std::string(#_9), prefix._9, std::string(#_10), prefix._10,         \
        std::string(#_11), prefix._11, std::string(#_12), prefix._12,       \
        std::string(#_13), prefix._13, std::string(#_14), prefix._14,       \
        std::string(#_15), prefix._15, std::string(#_16), prefix._16,       \
        std::string(#_17), prefix._17, std::string(#_18), prefix._18,       \
        std::string(#_19), prefix._19, std::string(#_20), prefix._20,       \
        std::string(#_21), prefix._21, std::string(#_22), prefix._22,       \
        std::string(#_23), prefix._23, std::string(#_24), prefix._24

#define SF_EXPAND_OBJ_MEM_WITH_NAME(...) \
    SF_INVOKE_VAR_MACRO(SF_EXPAND_ARGS_WITH_NAME_, __VA_ARGS__)

namespace skyfire {

// 网络缓冲区大小
constexpr std::size_t sf_default_buffer_size = 4096;

///////////////////////////////////////////////////////////////////////////////////////////////////

// TCP相关定义

// 用户可用的起始位置
constexpr int tcp_pkg_type_user = 0x70000000;

// RPC请求包类型
constexpr int rpc_req_type = 0x0000ffff;

// RPC响应包类型
constexpr int rpc_res_type = 0x0000fffe;

// 以下为系统定义type类型
// NAT穿透获取客户端列表
enum sf_nat_pkg_type {
    type_nat_traversal_get_list = 0x00010001,

    // NAT穿透客户端列表
    type_nat_traversal_list = 0x00010002,

    // NAT穿透客户端注册
    type_nat_traversal_reg = 0x00010003,

    // NAT穿透请求连接远端peer
    type_nat_traversal_require_connect_peer = 0x00010004,

    // NAT设置client id
    type_nat_traversal_set_id = 0x00010005,

    // NAT穿透错误
    type_nat_traversal_error = 0x00010005,

    // NAT穿透新连接请求
    type_nat_traversal_new_connect_required = 0x00010006,

    // NAT穿透服务器回复B的地址
    type_nat_traversal_server_reply_b_addr = 0x00010007,

    // NAT穿透B端回复地址
    type_nat_traversal_b_reply_addr = 0x00010008,

    // NAT穿透连接成功
    type_nat_traversal_connected = 0x00010009
};

// http
//////////////////////////////////////////////////////////////////////////////////////////////////
// http静态目录路由优先级
constexpr int default_http_static_priority = 100;
// http 中间路由优先级
constexpr int default_http_part_priority = 50;
// http 终端路由优先级
constexpr int default_http_endpoint_priority = 10;

// 最大缓存数量
constexpr int default_cache_max_count = 1024;

// 最大缓存数量
constexpr int default_cache_max_file_size = 1024 * 1024;

///////////////////////////////////////////////////////////////////////////////////////////////////

// error_code

enum sf_error_code {
    // 无错误
    sf_err_ok = 0,

    // 资源不存在
    sf_err_not_exist = 0x00010001,

    // 连接断开
    sf_err_disconnect = 0x00010002,

    // 远程错误
    sf_err_remote_err = 0x00010003,

    // 监听错误
    sf_err_listen_err = 0x00010004,

    // 绑定错误
    sf_err_bind_err = 0x00010005,
};
} // namespace skyfire

#pragma clang diagnostic pop