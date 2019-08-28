#include <iostream>
#include "param/Engine.h"


int main(int argc, char* argv[]) {
    param::Engine engine(&argc, &argv);
    const param::Object& root = engine.getRoot();
    std::cout << root;

    return 0;
}