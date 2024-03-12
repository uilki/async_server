#include "threadpool.h"

namespace net {
ThreadPool::ThreadPool(std::size_t pool_size)
    : pool_{pool_size}
    , work_{std::make_unique<boost::asio::executor_work_guard<boost::asio::thread_pool::executor_type>>(pool_.get_executor())}
    {}

ThreadPool::~ThreadPool()
{
    pool_.stop();
    pool_.join();
}
} //namespace net
