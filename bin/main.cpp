#include <functional>
#include <lib/CCirtucalBuffer.h>


int main(int argc, char** argv) {
    CCirtucalBuffer<int> buffer(3,0);
    buffer[0]=0;
    buffer[1]=1;
    buffer[2]=2;
    for (int i = 0; i <= 3; i++) {
        std::cout<<buffer[i];
    }
    return 0;

}
