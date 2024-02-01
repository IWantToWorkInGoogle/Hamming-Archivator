#include "HamArc.h"
#include "HammingReader.h"
#include "HammingWriter.h"
#include <iostream>

bool HamArc::parse(const std::filesystem::path &path) {
    file_count = 0;
    archive_name.clear();
    file_names.clear();
    file_pos.clear();
    if (path.extension() != ".haf") {
        std::cerr << "Wrong format." << std::endl;
        return false;
    }
    bool _result = true;
    std::fstream in(path, std::ios_base::in | std::ios_base::binary);
    HammingReader hr(in);
    if (!hr.isOpen()) {
        std::cerr << "There is no archive with such name in this directory.";
        return false;
    }
    archive_name = path.filename();
    _result &= hr.readLong(file_count);
    if (!_result) return false;
    for (size_t i = 0;i < file_count;i++) {
        std::string file_name;
        size_t size = 0;
        _result &= hr.readLong(size);
        if (!_result) return false;
        for (size_t j = 0;j < size;j++) {
            char ch;
            _result &= hr.readByte(ch);
            if (!_result) return false;
            file_name.push_back(ch);
        }
        file_names.insert(file_name);
        size = 0;
        _result &= hr.readLong(size);
        if (!_result) return false;
        file_pos[file_name] = hr.getPos();
        for (size_t j = 0;j < size;j++) {
            char ch;
            _result &= hr.readByte(ch);
            if (!_result) return false;
        }
    }
    return _result;
}

bool HamArc::create(const std::filesystem::path &path, const std::vector<std::filesystem::path> &file_paths) {
    if (exists(path)) {
        std::cerr << "There is an archive with the same name already. It would be overwritten." << std::endl;
    }

    std::fstream out(path, std::ios_base::out | std::ios_base::binary);
    HammingWriter hw(out);

    if (!hw.isOpen()) {
        std::cerr << "Couldn't create the archive." << std::endl;
        return false;
    }

    archive_name = path.filename();

    for (auto& p : file_paths) {
        file_names.insert(p.filename());
    }
    file_count = file_names.size();
    hw.convertLong(file_count);
    for (auto& p : file_paths) {
        std::filesystem::path filename = p.filename();
        if (file_pos.find(filename) != file_pos.end() || !exists(p)) {
            continue;
        }
        hw.convertLong(filename.string().size());
        for (char ch : filename.string()) {
            hw.convertByte(ch);
        }
        std::fstream in(p,std::ios_base::in | std::ios_base::binary);
        size_t size = std::filesystem::file_size(p);

        file_pos[filename] = hw.getPos();
        hw.convertLong(size);
        for (size_t i = 0;i < size;i++) {
            char ch;
            in.read(&ch,sizeof(char));
            if (in.eof()) {
                return false;
            }
            hw.convertByte(ch);
        }
    }
    hw.writeRest();
    std::cout << archive_name << " archive has been created." << std::endl;
    return true;
}

bool HamArc::delete_files(const std::filesystem::path &path, const std::vector<std::filesystem::path> &files) {
    bool _result = true;
    std::filesystem::path tmp_path = std::filesystem::path(path).replace_extension(".rep");
    std::fstream out(tmp_path, std::ios_base::out | std::ios_base::binary);
    std::fstream in(path,std::ios_base::in | std::ios_base::binary);
    HammingWriter hw(out);
    HammingReader hr(in);
    size_t tmp = 0;
    _result &= hr.readLong(tmp);
    for (auto& p : files) {
        if (file_names.count(p)) {
            file_count--;
            file_names.erase(p);
            file_pos.erase(p);
        }
    }
    hw.convertLong(file_count);
    for (size_t i = 0;i < tmp;i++) {
        size_t name_size = 0;
        _result &= hr.readLong(name_size);
        std::string filename;
        for (size_t j = 0;j < name_size;j++) {
            char ch;
            _result &= hr.readByte(ch);
            filename.push_back(ch);
        }
        size_t cur_file_size = 0;
        _result &= hr.readLong(cur_file_size);
        bool contains = file_names.count(filename);
        if (contains) {
            hw.convertLong(name_size);
            for (char ch : filename) {
                hw.convertByte(ch);
            }
            hw.convertLong(cur_file_size);
            file_pos[filename] = hw.getPos();
        }
        for (size_t j = 0;j < cur_file_size;j++) {
            char ch;
            _result &= hr.readByte(ch);
            if (contains) {
                hw.convertByte(ch);
            }
        }
    }
    hw.writeRest();

    in.close();
    out.close();
    std::filesystem::remove(path.c_str());
    std::filesystem::rename(tmp_path.c_str(),path.c_str());

    std::cout << "All found files for deleting erased." << std::endl;
    return _result;
}

bool HamArc::add_files(const std::filesystem::path &path, const std::vector<std::filesystem::path> &file_paths) {
    bool _result = true;
    std::filesystem::path tmp_path = std::filesystem::path(path).replace_extension(".rep");
    std::fstream out(tmp_path, std::ios_base::out | std::ios_base::binary);
    std::fstream in(path, std::ios_base::in | std::ios_base::binary);
    HammingWriter hw(out);
    HammingReader hr(in);
    size_t tmp = 0;
    _result &= hr.readLong(tmp);
    for (auto& p : file_paths) {
        if (!file_names.count(p.filename()) && exists(p)) {
            file_names.insert(p.filename());
            file_count++;
        }
    }
    hw.convertLong(file_count);
    for (size_t i = 0;i < tmp;i++) {
        size_t cur_file_size = 0, name_size = 0;
        _result &= hr.readLong(name_size);
        hw.convertLong(name_size);
        std::string filename;
        for (size_t j = 0;j < name_size;j++) {
            char ch;
            _result &= hr.readByte(ch);
            filename.push_back(ch);
            hw.convertByte(ch);
        }
        _result &= hr.readLong(cur_file_size);
        file_pos[filename] = hw.getPos();
        hw.convertLong(cur_file_size);
        for (size_t j = 0;j < cur_file_size;j++) {
            char ch;
            _result &= hr.readByte(ch);
            hw.convertByte(ch);
        }
    }

    for (auto& p : file_paths) {
        if (file_pos.find(p.filename()) == file_pos.end() && exists(p)) {
            hw.convertLong(p.filename().string().size());
            for (char ch : p.filename().string()) {
                hw.convertByte(ch);
            }
            file_pos[p.filename()] = hw.getPos();
            size_t size = std::filesystem::file_size(p);
            hw.convertLong(size);
            std::fstream bin(p, std::ios_base::in | std::ios_base::binary);
            for (size_t i = 0;i < size;i++) {
                char ch;
                _result &= hr.readByte(ch);
                if (bin.eof()) {
                    return false;
                }
                hw.convertByte(ch);
            }
        }
    }
    hw.writeRest();

    in.close();
    out.close();
    std::filesystem::remove(path.c_str());
    std::filesystem::rename(tmp_path.c_str(),path.c_str());

    std::cout << "The files were added." << std::endl;
    return _result;
}

void HamArc::return_list() {
    std::cout << "There are " << file_count << " files in " << archive_name << " archive." << std::endl;
    std::cout << "Files:" << std::endl;
    for (const auto& p : file_names) {
        std::cout << "- " << p << std::endl;
    }
}

bool HamArc::extract_files(const std::filesystem::path &path, const std::set<std::filesystem::path> &files) const {
    bool _result = true;
    std::fstream in(path,std::ios_base::in | std::ios_base::binary);
    HammingReader hr(in);
    size_t size = 0;
    _result &= hr.readLong(size);
    for (size_t i = 0;i < file_count;i++) {
        std::string file_name;
        _result &= hr.readLong(size);
        for (size_t j = 0;j < size;j++) {
            char ch;
            _result &= hr.readByte(ch);
            file_name.push_back(ch);
        }
        if (files.count(file_name)) {
            _result &= hr.readLong(size);
            std::fstream out(std::filesystem::current_path() / file_name, std::ios_base::out | std::ios_base::binary);
            for (size_t j = 0;j < size;j++) {
                char ch;
                _result &= hr.readByte(ch);
                out.write(&ch,sizeof(char));
            }
        }
    }
    if (_result) {
        std::cout << "Extraction of the files finished successfully." << std::endl;
    } else {
        std::cerr << "Extraction of the files finished with error." << std::endl;
    }
    return _result;
}

bool HamArc::merge_archives(const std::filesystem::path &path1, const std::filesystem::path &path2,
                            const std::filesystem::path &path3) {
    if (equivalent(path1, path2)) {
        std::cerr << "You can't merge 1 archive" << std::endl;
        return false;
    }

    bool _result = true;
    HamArc other;
    _result &= other.parse(path2);
    size_t tmp = file_count;
    for (auto &p: other.file_names) {
        if (!file_names.count(p)) {
            file_count++;
            file_names.insert(p);
        }
    }

    std::fstream in1(path1, std::ios_base::in | std::ios_base::binary);
    std::fstream out(path3, std::ios_base::out | std::ios_base::binary);
    HammingWriter hw(out);
    HammingReader hr1(in1);

    size_t garbage = 0;
    hw.convertLong(file_count);
    _result &= hr1.readLong(garbage);
    for (size_t i = 0; i < tmp; i++) {
        size_t cur_file_size = 0, name_size = 0;
        _result &= hr1.readLong(name_size);
        hw.convertLong(name_size);
        std::string filename;
        for (size_t j = 0; j < name_size; j++) {
            char ch;
            _result &= hr1.readByte(ch);
            filename.push_back(ch);
            hw.convertByte(ch);
        }
        _result &= hr1.readLong(cur_file_size);
        file_pos[filename] = hw.getPos();
        hw.convertLong(cur_file_size);
        for (size_t j = 0; j < cur_file_size; j++) {
            char ch;
            _result &= hr1.readByte(ch);
            hw.convertByte(ch);
        }
    }

    std::fstream in2(path2, std::ios_base::in | std::ios_base::binary);
    HammingReader hr2(in2);
    _result &= hr2.readLong(garbage);
    for (size_t i = 0; i < other.file_count; i++) {
        size_t name_size = 0;
        _result &= hr2.readLong(name_size);
        std::string filename;
        for (size_t j = 0; j < name_size; j++) {
            char ch;
            _result &= hr2.readByte(ch);
            filename.push_back(ch);
        }
        size_t cur_file_size = 0;
        _result &= hr2.readLong(cur_file_size);
        bool contains = file_pos.find(filename) == file_pos.end();
        if (contains) {
            hw.convertLong(name_size);
            for (char ch: filename) {
                hw.convertByte(ch);
            }
            hw.convertLong(cur_file_size);
            file_pos[filename] = hw.getPos();
        }
        for (size_t j = 0; j < cur_file_size; j++) {
            char ch;
            _result &= hr2.readByte(ch);
            if (contains) {
                hw.convertByte(ch);
            }
        }
    }
    hw.writeRest();

    std::cout << "The archives merged successfully." << std::endl;

    return _result;
}

std::set<std::filesystem::path> HamArc::get_file_names() {
    return file_names;
}