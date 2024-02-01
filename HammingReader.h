#ifndef HAMARC_HAMMINGREADER_H
#define HAMARC_HAMMINGREADER_H


#include <cstdio>
#include <fstream>
#include <filesystem>

class HammingReader {
    std::fstream* in;
    char lst;
    int8_t pos;
    bool require_new;
public:
    explicit HammingReader(std::fstream& _in) :
        in(&_in), lst(0), pos(0), require_new(true) {}
    bool isOpen();
    bool readByte(char &ch);
    bool readLong(uint64_t &n);
    std::pair<size_t,int8_t> getPos();
    ~HammingReader();
};

#endif //HAMARC_HAMMINGREADER_H
