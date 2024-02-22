#ifndef __SENMY3J84LHRUS916VYXIPYJFG9223WMGADOE12NZJJEC7KY2SW18IY2HJ0Q1S7VQDEDT1BBL__H__
#define __SENMY3J84LHRUS916VYXIPYJFG9223WMGADOE12NZJJEC7KY2SW18IY2HJ0Q1S7VQDEDT1BBL__H__


#include <array>
#include <bitset>
#include <cassert>
#include <condition_variable>
#include <mutex>

namespace util {
template<size_t Capacity>
class MessagePersistence
{
public:
    size_t persist(std::string mes)
    {
        std::unique_lock lck(mtx_);
        slotsFree_.wait(lck, [this](){ return !occupiedSlots_.all(); });
        size_t slot = 0;
        do {
            if (!occupiedSlots_.test(slot))
                { break; }
        } while (++slot < Capacity);
        assert(slot < Capacity);
        occupiedSlots_.flip(slot);
        messages_[slot] = std::move(mes);
        return slot;
    }

    void release(size_t slot)
    {
        std::unique_lock lck(mtx_);
        occupiedSlots_.flip(slot);
    }

    const std::string &operator[](size_t slot) const
    {
        assert(slot < Capacity);
        return messages_[slot];
    }


private:
    std::bitset<Capacity>             occupiedSlots_;
    std::array<std::string, Capacity> messages_     ;
    std::mutex                        mtx_          ;
    std::condition_variable           slotsFree_    ;
};
} // namespace util

#endif // __SENMY3J84LHRUS916VYXIPYJFG9223WMGADOE12NZJJEC7KY2SW18IY2HJ0Q1S7VQDEDT1BBL__H__
