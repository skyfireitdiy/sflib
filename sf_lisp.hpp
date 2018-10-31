#pragma once

#include "sf_lisp.h"
#include "sf_lex.hpp"
#include "sf_yacc.hpp"


namespace skyfire
{
    inline bool sf_lisp::run(const std::string &script, std::string &output)
    {
        return false;
    }

    sf_lisp::sf_lisp()
    {
        lex__.add_rules({
                                {"string", R"("([^\\"]|(\\["\\/bnrt]|(u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])))*")"},
                                {"(",      R"(\[)"},
                                {")",      R"(\])"},
                                {"number", R"(-?(0|[1-9]\d*)(\.\d+)?(e|E(\+|-)?0|[1-9]\d*)?)"},
                                {"ws",     R"([\r\n\t ]+)"},
                                {"take",   R"(take)"},
                                {"join",   R"(join)"},
                                {"append", R"(append)"},
                                {"remove", R"(remove)"},
                                {"eq",     R"(eq)"},
                                {"+",      R"(\+)"},
                                {"=",      R"(=)"},
                                {"empty",  R"(empty)"},
                                {"if",     R"(if)"},
                                {"null",   R"(null)"},
                                {"clone",  R"(clone)"}
                        });

        yacc__.add_rules({
                                 {

                                 }
                         });
    }
}