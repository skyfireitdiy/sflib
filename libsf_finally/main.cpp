#include <iostream>
#include "sf_finally.h"

using namespace std;
using namespace skyfire;

int main()
{
    try{
        sf_finally t([](){
            cout<<"123456"<<endl;
        });
        cout<<"hello world"<<endl;
        {
            sf_finally t([](){
                cout<<"666"<<endl;
            });
            throw int(5);
        }
    }
    catch(int t)
    {
        cout<<"catch "<<t<<endl;
    }
}