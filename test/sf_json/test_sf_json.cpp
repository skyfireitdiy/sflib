#include <sf_json>
#include <sf_stdc++>
#include <sf_test>

using namespace skyfire;
using namespace std;

bool test_construct(){
    sf_json js = 5;
    sf_test_equal(static_cast<int>(js), 5);

    return true;
}

int main(){
    sf_test(test_construct);

    return sf_test_run();
}