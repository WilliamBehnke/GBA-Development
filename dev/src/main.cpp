#include "bn_core.h"

int main() {
    bn::core::init();

    while(true) {
        bn::core::update(); // Advance one frame
    }

    return 0;
}
