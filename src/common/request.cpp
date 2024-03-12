#include "request.h"

common::AuthentificationRequest::AuthentificationRequest(size_t e, const std::list<std::string> &data)
    : id  {static_cast<ErrorCode>(e)}
    , data{data}
{}

common::SessionRequest::SessionRequest(size_t id, const std::string &name, const std::list<std::string> &args)
    : id  {id}
    , name{name}
    , args{args}
{}
