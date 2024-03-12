#include "somefunctions.h"

#include <iostream>
#include <list>
#include <numeric>
#include <filesystem>

namespace fs = std::filesystem;

std::string findFile(const std::string &name, const std::string &hint, std::string type)
{
    if (type.empty())
        { type = "regular"; }

    auto testType = [&type](const fs::file_status& status) -> bool
    {
        switch (status.type()) {
        case fs::file_type::regular  : return type == "regular"  ;
        case fs::file_type::directory: return type == "directory";
        case fs::file_type::symlink  : return type == "symlink"  ;
        default                      : return false              ;
        }
    };

    try
        { fs::current_path(fs::path{hint}); }
    catch (fs::filesystem_error& e)
        { fs::current_path(fs::current_path().root_directory()); }

    std::list<std::string> res;

    auto search = [&res, &name,&testType](const std::filesystem::path& path, auto&& search) -> void {
        for (const fs::directory_entry& dir_entry : fs::directory_iterator(path)) {
            if (name == dir_entry.path().filename() && testType(dir_entry.status()))
                { res.emplace_back(dir_entry.path().parent_path().c_str()); }
            if (fs::file_status{dir_entry.status()}.type() == fs::file_type::directory)
                { search(dir_entry, search); }
        }
    };

    search(fs::current_path(), search);

    return [i = res.size(),&res]() mutable -> std::string {
        std::string result;
        for(const auto& p : res)
            { result += p + (--i ? " " : ""); }
        return result; }();
}

double divide(int i, double d)
    { return i/d; }
