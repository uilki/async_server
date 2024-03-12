#ifndef __BBK2QN6O8HDS5RD99R8R4F30641Y1I8A7Q0VS0EN9PM4W0ISWW1L69DCMQPGSADZHR0MNJXWW__H__
#define __BBK2QN6O8HDS5RD99R8R4F30641Y1I8A7Q0VS0EN9PM4W0ISWW1L69DCMQPGSADZHR0MNJXWW__H__

#include <cstddef>
#include <list>
#include <string>

namespace common {
struct AuthentificationRequest
{
    enum ErrorCode { signIn, signUp, invalidToken, userExists, badPassword, unregistered, success, refused, BadRequest};

    explicit AuthentificationRequest(size_t e, const std::list<std::string>& data = {} );
    AuthentificationRequest() = default;
    AuthentificationRequest(const AuthentificationRequest&) = default;

    ErrorCode              id  ;
    std::list<std::string> data;
};

struct SessionRequest
    {
        enum { BadRequest = AuthentificationRequest::BadRequest};

        SessionRequest(size_t                        id  ,
                       const std::string&            name,
                       const std::list<std::string>& args );
        SessionRequest() = default;
        SessionRequest(const SessionRequest&) = default;

        size_t                 id  ;
        std::string            name;
        std::list<std::string> args;
    };
} // namespace common

#endif // __BBK2QN6O8HDS5RD99R8R4F30641Y1I8A7Q0VS0EN9PM4W0ISWW1L69DCMQPGSADZHR0MNJXWW__H__