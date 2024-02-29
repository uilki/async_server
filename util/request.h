#ifndef __AOXV73F3PFCDPUK3OSAQITQK8T5XKFN2CDAFUPM24YK4LJ3Q7I6L31L1V4DDRHSJ2RMYV49DK__H__
#define __AOXV73F3PFCDPUK3OSAQITQK8T5XKFN2CDAFUPM24YK4LJ3Q7I6L31L1V4DDRHSJ2RMYV49DK__H__

#include <list>
#include <rapidjson/document.h>

namespace util {

class BadRequest : public std::exception {
    const char* mes_;
public:
    explicit BadRequest(const char* mes) : mes_{mes} {}

public:
    inline const char *what() const noexcept override
        { return mes_; }
};

class RequestParser
{
public:
    struct Request{
        enum { signIn, signUp, invalidToken, userExists, badPassword, unregistered, success, refused, BadRequest };
        static constexpr auto invalidMessage{"parse error"};

        size_t                 id  {};
        std::string            name{};
        std::list<std::string> args{};
    };

public:
    RequestParser();

    std::string fromString(int transactionID, std::string name = "", std::string other = "") const;
    Request parse(const char* json) const;

private:
    rapidjson::Document schemaDoc_;
};
} // namespace util
#endif // __AOXV73F3PFCDPUK3OSAQITQK8T5XKFN2CDAFUPM24YK4LJ3Q7I6L31L1V4DDRHSJ2RMYV49DK__H__
