#ifndef __HIRWE22WEYCRZQBAE91EZQ1V2FRXGDOGIY87ETX131F3WPXZHXA69TQOF9TC2XF2BI1UGQWDA__H__
#define __HIRWE22WEYCRZQBAE91EZQ1V2FRXGDOGIY87ETX131F3WPXZHXA69TQOF9TC2XF2BI1UGQWDA__H__

#include <boost/asio/post.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/thread_pool.hpp>

namespace net {
class ThreadPool : private boost::noncopyable
{
public:
    explicit ThreadPool(std::size_t pool_size);
    ~ThreadPool();

    template<typename Fun>
    inline void post(Fun f)
        { boost::asio::post(pool_, f); }

private:
    boost::asio::thread_pool pool_;
    std::unique_ptr<boost::asio::executor_work_guard<boost::asio::thread_pool::executor_type>> work_;
};
} // namespace net

#endif // __HIRWE22WEYCRZQBAE91EZQ1V2FRXGDOGIY87ETX131F3WPXZHXA69TQOF9TC2XF2BI1UGQWDA__H__
