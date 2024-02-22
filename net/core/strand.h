#ifndef __WMM6L8SDAJUKWX0L5CG9Z2F35NDKIEILO9DSC8NV5ZJBAGWOONO1PTM2IQ7WXDA63E5Y9UEKD__H__
#define __WMM6L8SDAJUKWX0L5CG9Z2F35NDKIEILO9DSC8NV5ZJBAGWOONO1PTM2IQ7WXDA63E5Y9UEKD__H__

#include <memory>

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

namespace net {
using ServiceType = boost::asio::io_service;
using StrandType = boost::asio::strand<ServiceType::executor_type>;
using StrandPtr = std::shared_ptr<StrandType>;

inline StrandPtr makeStrand(boost::asio::io_service& ioService)
    { return std::make_shared<StrandType>(boost::asio::make_strand(ioService)); }

inline bool isStrandThread(const StrandPtr& strand)
    { return strand->running_in_this_thread(); }
}
#endif // __WMM6L8SDAJUKWX0L5CG9Z2F35NDKIEILO9DSC8NV5ZJBAGWOONO1PTM2IQ7WXDA63E5Y9UEKD__H__
