#include "logger.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <memory>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

enum class StampType : uint8_t { Full, Short};

std::string timeStamp(StampType type = StampType::Full)
{
    auto tp{ std::chrono::system_clock::now() };
    time_t now = std::chrono::system_clock::to_time_t(tp);
    const auto tse = tp.time_since_epoch();
    std::chrono::seconds::rep ms = std::chrono::duration_cast<std::chrono::milliseconds>(tse).count() % 1000;

    auto lt = localtime(&now);
    std::ostringstream os;

    os << std::setfill('0') << std::setw(2) << lt->tm_mday << '-'
        << std::setfill('0') << std::setw(2) << lt->tm_mon << '-'
        << 1900 + lt->tm_year;

    if (type == StampType::Full)
        os << ' '
           << std::setfill('0') << std::setw(2) << lt->tm_hour << ':'
           << std::setfill('0') << std::setw(2) << lt->tm_min << ':'
           << std::setfill('0') << std::setw(2) << lt->tm_sec << '.'
           << std::setfill('0') << std::setw(3) << ms;
    else
        os << '_'
           << std::setfill('0') << std::setw(2) << lt->tm_hour
           << std::setfill('0') << std::setw(2) << lt->tm_min
           << std::setfill('0') << std::setw(2) << lt->tm_sec
           << std::setfill('0') << std::setw(3) << ms;

    return os.str();
}

class Writer {
    enum { MAX_SIZE = 50 };
    static constexpr auto log_file{ "log_" };

    std::thread             writer;
    std::mutex              writer_mtx;
    std::condition_variable cv_empty;
    std::condition_variable cv_full;
    std::queue<std::string> queue;
    std::atomic<bool>       exit_request{ false};
    bool                    valid       { true };

    Writer()
        : writer{ &Writer::doWork, this }
        {}

    ~Writer()
    {
        exit_request.store(true);
        cv_full.notify_one();
        if (writer.joinable())
            writer.join();
    }

    void doWork()
    {
        std::ofstream ofs{ std::string{log_file} + timeStamp(StampType::Short) };
        if (!ofs.is_open()) {
            std::cerr << "Writer::doWork --> Can't open log file" << std::endl;
            valid = false;
            return;
        }


        while (true) {
            std::unique_lock<std::mutex> lock{ writer_mtx };
            cv_full.wait(lock, [this] { return !queue.empty() || exit_request; });
            if (exit_request || ofs.bad()){
                valid = false;
                return;
            }

            auto tmp = queue.front();
            queue.pop();
            lock.unlock();
            cv_empty.notify_all();
            ofs << tmp << std::endl;
        }
    }
public:
    static Writer& instance()
    {
        static Writer writer{};
        return writer;
    }

    void log(const std::string& mes)
    {
        if (!valid) {
            std::cerr << "Can't write to log file" << std::endl;
            return;
        }

        std::unique_lock<std::mutex> lock{ writer_mtx };
        cv_empty.wait(lock, [this] { return queue.size() < MAX_SIZE; });
        queue.push(mes);

        lock.unlock();
        cv_full.notify_one();
    }
};


util::Logger::Logger(const char *file, const char *fun, int line)
{
    stream << timeStamp() << " tid [" << std::hex << std::this_thread::get_id() << "] ["
           << file << "|" << fun << ":" << std::dec << line << "] ";
}

util::Logger::Logger(Logger &&rh)
    : stream(std::move(rh.stream))
    { rh.valid = false; }

util::Logger::~Logger()
{
    try {
        if (!valid) return;
        std::string const& message = stream.str();
        Writer::instance().log(message);
    }
    catch (...) {}
}

util::Logger &util::Logger::operator<<(std::ostream &(*F)(std::ostream &))
    { F(stream); return *this; }

