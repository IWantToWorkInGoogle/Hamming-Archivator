#ifndef HAMARC_HAMMINGWRITER_H
#define HAMARC_HAMMINGWRITER_H

#include <fstream>
#include <cstdint>
#include <filesystem>

class HammingWriter {
    std::fstream * out;
    char lst;
    size_t pos;
public:
    explicit HammingWriter(std::fstream &_out) : out(&_out), lst(0), pos(0) {}
    void convertByte(char ch);
    void writeRest();
    void convertLong(uint64_t n);
    bool isOpen();
    std::pair<size_t,int8_t> getPos();
};


#endif //HAMARC_HAMMINGWRITER_H
