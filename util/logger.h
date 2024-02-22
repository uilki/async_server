#ifndef __M0YU81XX0JE9XI5EZRS92FH3VXL62NQ17RYRPZPYWTLASQ453CRRRVWG99ITCCOYF7P6MWSMF__H__
#define __M0YU81XX0JE9XI5EZRS92FH3VXL62NQ17RYRPZPYWTLASQ453CRRRVWG99ITCCOYF7P6MWSMF__H__

#include <sstream>

namespace util {
class Logger {
    Logger(Logger const&) = delete;
    Logger& operator = (Logger const&) = delete;
    Logger& operator = (Logger&&) = delete;
public:
    Logger(const char* file, const char* fun, int line);

    Logger(Logger&& rh);

    ~Logger();
    template<typename T>
    Logger& operator << (T const& t)
    {
        stream << t;
        return *this;
    }

    template<typename T>
    Logger& operator << (T const* t)
    {
        stream << t;
        return *this;
    }

    //to handle manipulators like endl
    Logger& operator<<(std::ostream& (*F)(std::ostream&));
private:
    static int depth;
    std::ostringstream stream;
    bool valid{ true };
};

}

#define log_info \
util::Logger(__FILE__, __func__, __LINE__)

#endif // __M0YU81XX0JE9XI5EZRS92FH3VXL62NQ17RYRPZPYWTLASQ453CRRRVWG99ITCCOYF7P6MWSMF__H__
