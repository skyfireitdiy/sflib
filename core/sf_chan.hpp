#include "sf_chan.h"

namespace skyfire{
    template <typename  T>
    sf_chan<T>::sf_chan(int buffer_size) {
        if(buffer_size<1){
            buffer_size = 1;
        }
        max_size__=buffer_size;
    }

    template <typename T>
    void sf_chan<T>::close() {
        closed__ = true;
    }

}