#ifndef __ZUZPISHYNWF3UO7K6N2PQQ265IENL6KTTBHYW38NCAKS2T17YW1OBMBUPXA2B1FV2W50DLRUW__H__
#define __ZUZPISHYNWF3UO7K6N2PQQ265IENL6KTTBHYW38NCAKS2T17YW1OBMBUPXA2B1FV2W50DLRUW__H__

#include <list>
#include <memory>

namespace holder {

class Registry
{
    Registry();
public:
    static Registry& registry();
    std::string call(const std::string& funName, std::list<std::string>& args);
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
} // namespace holder

#endif // __ZUZPISHYNWF3UO7K6N2PQQ265IENL6KTTBHYW38NCAKS2T17YW1OBMBUPXA2B1FV2W50DLRUW__H__
