#ifndef HAMARC_HAMARC_H
#define HAMARC_HAMARC_H


#include <cstdlib>
#include <set>
#include <string>
#include <map>
#include <cstdint>
#include <vector>
#include <filesystem>

class HamArc {
    size_t file_count = 0;
    std::filesystem::path archive_name;
    std::set<std::filesystem::path> file_names;
    std::map<std::filesystem::path,std::pair<size_t,int8_t>> file_pos;
public:
    bool parse(const std::filesystem::path &path);
    bool create(const std::filesystem::path &path, const std::vector<std::filesystem::path> &file_paths);
    bool delete_files(const std::filesystem::path &path, const std::vector<std::filesystem::path> &file_names);
    bool add_files(const std::filesystem::path &path, const std::vector<std::filesystem::path> &file_paths);
    void return_list();
    bool merge_archives(const std::filesystem::path &path1, const std::filesystem::path &path2, const std::filesystem::path &path3);
    bool extract_files(const std::filesystem::path &path, const std::set<std::filesystem::path> &files) const;
    std::set<std::filesystem::path> get_file_names();
};


#endif //HAMARC_HAMARC_H
