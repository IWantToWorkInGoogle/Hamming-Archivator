#ifndef HAMARC_TARGUMENTPARSER_H
#define HAMARC_TARGUMENTPARSER_H

#include <cstdint>
#include <string>
#include <vector>
#include "HamArc.h"

class TArgumentParser {
    uint8_t flags = 0;
    std::filesystem::path arch_name;
    std::vector<std::filesystem::path> files;
    std::set<std::filesystem::path> extracting_files;
    std::vector<std::filesystem::path> deleting_files;
    std::vector<std::filesystem::path> appending_files;
    std::vector<std::filesystem::path> concatenating_archs;
    HamArc *hamArc = new HamArc();
public:
    bool correct_input();
    bool parse(int argc, char** argv);
    void read();
    void run();
};

#endif //HAMARC_TARGUMENTPARSER_H
