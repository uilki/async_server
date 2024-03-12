#ifndef __XQY4RC8LGF3CWSRICR42T3NOMZALV7Q8T8C6O6ZTGKQOGRE6MD5M39E6U8J34SISP61FMRLT9__H__
#define __XQY4RC8LGF3CWSRICR42T3NOMZALV7Q8T8C6O6ZTGKQOGRE6MD5M39E6U8J34SISP61FMRLT9__H__

#include <type_traits>
#include <deque>
#include <vector>
#include <list>

namespace util::details
{
template<typename T>
struct is_array_like : std::false_type {};

template<typename T, typename Aloc>
struct is_array_like<std::vector<T, Aloc>> : std::true_type {};

template<typename T, typename Aloc>
struct is_array_like<std::list<T, Aloc>> : std::true_type {};

template<typename T, typename Aloc>
struct is_array_like<std::deque<T, Aloc>> : std::true_type {};

template<typename T>
inline constexpr bool is_array_like_v = is_array_like<T>::value;

template<typename T>
struct remove_cv_ref { using type = std::remove_reference_t<std::remove_const_t<T>>; };

template<typename T>
using remove_cv_ref_t = typename remove_cv_ref<T>::type;

} // namespace util::details
#endif // __XQY4RC8LGF3CWSRICR42T3NOMZALV7Q8T8C6O6ZTGKQOGRE6MD5M39E6U8J34SISP61FMRLT9__H__
