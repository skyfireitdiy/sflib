#include "compiler_utils.h"

namespace skyfire
{
bool operator<(const yacc_state_node_t& a,
               const yacc_state_node_t& b)
{
    return a.id < b.id;
}
bool operator==(const yacc_state_node_t& a,
                const yacc_state_node_t& b)
{
    return a.id == b.id;
}
} // namespace skyfire
