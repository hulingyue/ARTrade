#include "mouse.h"

int main(int argc, char **argv) {
    Mouse mouse(argc, argv);

    mouse.run();
    return 0;
}