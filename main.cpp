#include <iostream>
#include "TArgumentParser.h"

int main(int argc, char* argv[]) {
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);
    TArgumentParser t;
    if (!t.parse(argc, argv)) {
        std::cerr << "Invalid parameters." << std::endl;
        t.read();
    } else {
        t.run();
    }

    return 0;
}


