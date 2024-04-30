#include<exception>
#include <iostream>
#include "init_storage.h"

int main() {

    try {
        class init_storage* my_storage = new class init_storage();
        my_storage->my_storage = my_storage;
        my_storage->start_programm();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}