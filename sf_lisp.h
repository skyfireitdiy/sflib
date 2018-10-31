#pragma once

#include <string>
#include "sf_lex.h"
#include "sf_yacc.h"


namespace skyfire
{
    class sf_lisp
    {
    private:
        sf_lex lex__;
        sf_yacc yacc__;

    public:

        sf_lisp();

        bool run(const std::string &script, std::string &output);
    };


}