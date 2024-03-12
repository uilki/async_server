#ifndef __KP1MJRSJPP9BDGIJEE5OZFHDKRP2ODTX9642AAGK0DRUB95IX5QK9R2UCNXDF5XQE36DD9UV6__H__
#define __KP1MJRSJPP9BDGIJEE5OZFHDKRP2ODTX9642AAGK0DRUB95IX5QK9R2UCNXDF5XQE36DD9UV6__H__

#include "strand.h"

#include <thread>

#include <boost/asio/io_service.hpp>

namespace net
{
using ServiceType = boost::asio::io_service;
class Executor
{
public:
    Executor();
    ~Executor();

    inline ServiceType& ioService() { return io_service_; }
    inline const ServiceType& ioService() const { return io_service_; }

    inline StrandPtr strand() { return strand_; }
    inline const StrandPtr strand() const { return strand_; }
    void stop(bool force = true);

private:
    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;

    boost::asio::io_service                                                       io_service_;
    std::unique_ptr<boost::asio::executor_work_guard<ServiceType::executor_type>> work_      ;
    StrandPtr                                                                     strand_    ;
    std::thread                                                                   thread_    ;
};
} // namespace net

#endif // __KP1MJRSJPP9BDGIJEE5OZFHDKRP2ODTX9642AAGK0DRUB95IX5QK9R2UCNXDF5XQE36DD9UV6__H__
