#include "bybit.h"

int main(int argc, char **argv) {
    Bybit bybit(argc, argv);

    bybit.run();
    return 0;
}