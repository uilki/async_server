#include "filepasswordserver.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace net::server {
FilePasswordServer::FilePasswordServer(const char *file)
    : fileName_{file}
{
    if (!fs::exists(fs::current_path().append(file)))
        return;
    std::ifstream is(file);
    if (!is.is_open())
        return;
    std::string line;
    while(std::getline(is, line)) {
        auto separatorPos = line.find_first_of(';');
        users_.emplace(line.substr(0, separatorPos), std::stoull(line.substr(separatorPos +1),nullptr, 16));
    }
}

FilePasswordServer::~FilePasswordServer()
{
    saveEntries();
}

std::pair<bool, std::size_t> FilePasswordServer::getUserHash(const  std::string &user)
{
    if (users_.find(user) == users_.end()) return {false,{}};
    return { true, users_[user]};
}

bool FilePasswordServer::addUser(const std::string &user, std::size_t hash)
{
    if (users_.find(user) != users_.end()) return false;
    users_[user] = hash;
    return true;
}

void FilePasswordServer::saveEntries()
{
    std::ofstream os(fileName_);
    if (!os.is_open())
        return;
    for (const auto& entry : users_) {
        os << entry.first << ';' << std::hex << entry.second << '\n';
    }
}
} // namespace net::server
