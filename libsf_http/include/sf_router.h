#pragma once

namespace skyfire
{
    class sf_router
    {
        public:
        virtual int get_priority() const
        {
            return 0;
        }
    };
}