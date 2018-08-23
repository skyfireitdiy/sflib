#include "sf_timer.hpp"
#include "sf_eventloop.hpp"
#include <iostream>

using namespace skyfire;

sf_eventloop loop;
sf_timer timer;

void func(){
    static int t = 0;
    if(t == 5){
        timer.stop();
        loop.quit();
    }else{
        std::cout<<t++<<std::endl;
    }
}

int main() {

    sf_bind_signal(&timer, timeout, func, true);
    timer.start(3000);
    timer.stop();
    timer.start(3000);
    timer.stop();
    timer.start(3000);
    timer.stop();
    timer.start(3000);
    loop.exec();
}