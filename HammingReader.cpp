#include <iostream>
#include "HammingReader.h"

bool HammingReader::readByte(char &ch) {

    if (!in->is_open() || in->eof()) {
        return false;
    }
    ch = 0;

    int code = 0;
    for (int i = 0;i < 12;i++) {
        if (require_new) {
            in->read(&lst,sizeof(char));
            require_new = false;
        }
        if (in->eof()) {
            return false;
        }
        code ^= lst & (1 << pos) ? (1 << i) : 0;
        pos = (pos + 1) % 8;
        if (!pos) {
            require_new = true;
        }
    }

    int wrong_pos = -1;
    while (wrong_pos) {
        wrong_pos = 0;
        int xor_sum[4];
        std::fill(xor_sum,xor_sum + 4,0);
        for (int i = 0;i < 12;i++) {
            if (code & (1 << i)) {
                for (int j = 0;j < 4;j++) {
                    if ((i + 1) & (1 << j)) {
                        xor_sum[j] ^= 1;
                    }
                }
            }
        }
        for (int j = 0;j < 4;j++) {
            if (xor_sum[j]) {
                wrong_pos += (1 << j);
            }
        }
        if (wrong_pos) code ^= (1 << (wrong_pos - 1));
    }

    int ch_pos = 0;
    for (int i = 0;i < 12;i++) {
        if (__builtin_popcount(i + 1) == 1) {
            continue;
        }
        ch ^= code & (1 << i) ? (1 << ch_pos) : 0;
        ch_pos++;
    }

    return true;
}

bool HammingReader::readLong(uint64_t &n) {
    n = 0;
    bool res = true;
    char ch;
    for (int i = 0;i < 4;i++) {
        res &= readByte(ch);
        if (!res) break;
        n += (1 << (8 * i)) * (unsigned char)(ch);
    }
    return res;
}

bool HammingReader::isOpen() {
    return in->is_open();
}

std::pair<size_t, int8_t> HammingReader::getPos() {
    return {in->tellg(), pos};
}

HammingReader::~HammingReader() {
    in->close();
}
