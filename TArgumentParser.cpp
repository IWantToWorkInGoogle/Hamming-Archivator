#include <iostream>
#include <cstring>
#include <regex>
#include "TArgumentParser.h"

std::regex re("^-.+$");

uint8_t f(int c) {
    return ((uint8_t)1 << c);
}

bool TArgumentParser::parse(int argc, char** argv) {
    bool _res = true;
    for (size_t i = 1;i < argc;i++) {
        if (!strcmp(argv[i],"-c") || !strcmp(argv[i],"--create")) {
            if (flags & f(1)) {
                _res = false;
                break;
            }
            flags ^= f(1);
        } else if (!strcmp(argv[i],"-f")) {
            if (flags & f(0)) {
                _res = false;
                break;
            }
            flags ^= f(0);
            if (i + 1 < argc && !std::regex_match(argv[i + 1],re)) {
                arch_name = argv[i + 1];
                i++;
            } else {
                _res = false;
                break;
            }
        } else if (!strcmp(argv[i],"--file")) {
            if (flags & f(0)) {
                _res = false;
                break;
            }
        } else if (!strcmp(argv[i],"-l") || !strcmp(argv[i],"--list")) {
            if (flags & f(2)) {
                _res = false;
                break;
            }
            flags ^= f(2);
            // no other arguments
        } else if (!strcmp(argv[i],"-x") || !strcmp(argv[i],"--extract")) {
            if (flags & f(3)) {
                _res = false;
                break;
            }
            flags ^= f(3);
            if (i + 1 < argc && !std::regex_match(argv[i + 1],re)) {
                extracting_files.insert(argv[i + 1]);
                i++;
            }
        } else if (!strcmp(argv[i],"-a") || !strcmp(argv[i],"--append")) {
            if (flags & f(4)) {
                _res = false;
                break;
            }
            flags ^= f(4);
            if (i + 1 < argc && !std::regex_match(argv[i + 1],re)) {
                appending_files.emplace_back(argv[i + 1]);
                i++;
            } else {
                _res = false;
                break;
            }
        } else if (!strcmp(argv[i],"-d") || !strcmp(argv[i],"--delete")) {
            if (flags & f(5)) {
                _res = false;
                break;
            }
            flags ^= f(5);
            if (i + 1 < argc && !std::regex_match(argv[i + 1],re)) {
                deleting_files.emplace_back(argv[i + 1]);
                i++;
            } else {
                _res = false;
                break;
            }
        } else if (!strcmp(argv[i],"-A") || !strcmp(argv[i],"--concatenate")) {
            if (flags & f(6)) {
                _res = false;
                break;
            }
            flags ^= f(6);
            for (int j = 0;j < 2;j++) {
                if (i + 1 < argc && !std::regex_match(argv[i + 1],re)) {
                    concatenating_archs.emplace_back(argv[i + 1]);
                    i++;
                } else {
                    _res = false;
                    break;
                }
            }
        } else {
            files.emplace_back(argv[i]);
        }
    }

    return _res;
}

void TArgumentParser::read() {
    std::cout << "flags: ";
    for (int i = 0;i < 8;i++) {
        std::cout << ((flags & (1 << i)) ? 1 : 0);
    }
    std::cout << "\n";
    std::cout << "arch_name: " << arch_name << "\n";
    std::cout << "files: " << "\n";
    for (const auto& p : files) {
        std::cout << p << "\n";
    }
    std::cout << "extracting_files: " << "\n";
    for (const auto& p : extracting_files) {
        std::cout << p << "\n";
    }
    std::cout << "deleting_files: " << "\n";
    for (const auto& p : deleting_files) {
        std::cout << p << "\n";
    }
    std::cout << "appending_files: " << "\n";
    for (const auto& p : appending_files) {
        std::cout << p << "\n";
    }
    std::cout << "concatenating_archs: " << "\n";
    for (const auto& p : concatenating_archs) {
        std::cout << p << "\n";
    }
}

bool TArgumentParser::correct_input() {
    return !arch_name.empty();
}

void TArgumentParser::run() {
    if (!correct_input()) {
        std::cerr << "There is no object." << std::endl;
        return;
    }
    if (flags & f(0) && flags && f(1) && !(flags ^ (f(0) | f(1)))) {
        hamArc->create(arch_name,files);
    } else if (flags & f(0) && flags & f(2) && !(flags ^ (f(0) | f(2)))) {
        if (!hamArc->parse(arch_name)) {
            std::cerr << "Can't decode correctly." << std::endl;
            return;
        }
        hamArc->return_list();
    } else if (flags & f(0) && flags & f(3) && !(flags ^ (f(0) | f(3)))) {
        if (!hamArc->parse(arch_name)) {
            std::cerr << "Can't decode correctly." << std::endl;
            return;
        }
        if (extracting_files.empty()) {
            hamArc->extract_files(arch_name,extracting_files);
        } else {
            hamArc->extract_files(arch_name, hamArc->get_file_names());
        }
    } else if (flags & f(0) && flags & f(4) && !(flags ^ (f(0) | f(4)))) {
        if (!hamArc->parse(arch_name)) {
            std::cerr << "Can't decode correctly." << std::endl;
            return;
        }
        hamArc->add_files(arch_name,appending_files);
    } else if (flags & f(0) && flags & f(5) && !(flags ^ (f(0) | f(5)))) {
        if (!hamArc->parse(arch_name)) {
            std::cerr << "Can't decode correctly." << std::endl;
            return;
        }
        hamArc->delete_files(arch_name,deleting_files);
    } else if (flags & f(0) && flags & f(6) && !(flags ^ (f(0) | f(6)))) {
        if (concatenating_archs.size() != 2) {
            std::cerr << "Wrong number of parameters(required 2 parameters)." << std::endl;
            return;
        }
        if (!hamArc->parse(concatenating_archs[0])) {
            std::cerr << "Can't decode correctly." << std::endl;
            return;
        }
        hamArc->merge_archives(concatenating_archs[0],concatenating_archs[1],arch_name);
    } else {
        std::cerr << "Wrong parameters(you can't use more than 1 operation)." << std::endl;
    }
}