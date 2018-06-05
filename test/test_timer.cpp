#include "sf_timer.h"
#include "sf_eventloop.h"
#include <iostream>


using namespace std;
using namespace skyfire;

sf_eventloop loop;
sf_timer timer;

void func(){
    static int t = 0;
    if(t == 5){
        timer.stop();
        loop.quit();
    }else{
        cout<<t++<<endl;
    }
}

int main() {

    sf_bind_signal(&timer, timeout, func, false);
    timer.start(3000);
    timer.stop();
    timer.start(3000);
    timer.stop();
    timer.start(3000);
    timer.stop();
    timer.start(3000);
    loop.exec();
}