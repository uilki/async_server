#include "executor.h"
#include "logger.h"

namespace net {
Executor::Executor()
    : work_  {std::make_unique<boost::asio::executor_work_guard<ServiceType::executor_type>>(io_service_.get_executor())}
    , strand_{makeStrand(io_service_)}
    , thread_{[this](){
              boost::system::error_code ec;
              ioService().run(ec);
              if (ec)
                  { log_info << "Executor: " << ec.message(); }
          }
      }
    {}

Executor::~Executor()
    { stop(); }

void Executor::stop(bool force)
{
    if (!thread_.joinable())
        { return; }

    if (force)
        { ioService().stop(); }
    else
        { work_.reset(); }

    assert((std::this_thread::get_id() != thread_.get_id()) && "Executor: self join detected!");
    thread_.join();
}
} // namespace net
