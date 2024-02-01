#include <iostream>
#include "HammingWriter.h"

int hamming_code(char ch) {
    int code = 0, ch_pos = 0;
    for (int i = 0;i < 12;i++) {
        if (__builtin_popcount(i + 1) == 1) {
            continue;
        }
        if (ch & (1 << ch_pos)) {
            code ^= (1 << i);
            for (int j = 0;j < 4;j++) {
                if ((i + 1) & (1 << j)) {
                    code ^= (1 << ((1 << j) - 1));
                }
            }
        }
        ch_pos++;
    }
    return code;
}

void HammingWriter::convertByte(char ch) {
    int code = hamming_code(ch);
    for (int i = 0;i < 12;i++) {
        if (code & (1 << i)) {
            lst ^= (1 << pos);
        }
        if (pos == 7) {
            out->write(&lst,sizeof(char));
            lst = 0;
        }
        pos = (pos + 1) % 8;
    }
}

void HammingWriter::writeRest() {
    if (pos) {
        out->write(&lst,sizeof(char));
        lst = 0;
        pos = 0;
    }
}

void HammingWriter::convertLong(uint64_t n) {
    for (int i = 0;i < 4;i++) {
        unsigned char b = n % (1 << 8);
        n >>= 8;
        convertByte((char)b);
    }
}

std::pair<size_t, int8_t> HammingWriter::getPos() {
    return {out->tellg(), pos};
}

bool HammingWriter::isOpen() {
    return out->is_open();
}