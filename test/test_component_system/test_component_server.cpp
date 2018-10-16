#include "sf_component_server.hpp"

#include <iostream>

using namespace std;
using namespace skyfire;

int main(){
    auto p_server = sf_component_server::make_server(".");
}