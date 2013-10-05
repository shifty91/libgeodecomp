#ifndef LIBGEODECOMP_MISC_STDCONTAINEROVERLOADS_H
#define LIBGEODECOMP_MISC_STDCONTAINEROVERLOADS_H

#include <algorithm>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <vector>
#include <libgeodecomp/misc/stdcontaineroverloads.h>

#include <libgeodecomp/config.h>
#ifdef LIBGEODECOMP_FEATURE_BOOST_SERIALIZATION
#include <boost/serialization/vector.hpp>
#endif

namespace LibGeoDecomp {

using std::vector;

/**
 * vector:
 */

/**
 * Deletes items from @param vec that are equal to @param obj
 */
template <typename T, typename Allocator, typename U>
inline void del(std::vector<T, Allocator> & vec, const U& obj)
{
    vec.erase(std::remove(vec.begin(), vec.end(), obj), vec.end());
}

template <typename T, typename Allocator>
inline void append(std::vector<T, Allocator>& target, const std::vector<T, Allocator>& other)
{
    target.insert(target.end(), other.begin(), other.end());
}

template <typename T, typename Allocator>
inline void push_front(std::vector<T, Allocator>& vec, const T& obj)
{
    vec.insert(vec.begin(), obj);
}

template <typename T, typename Allocator>
inline T pop_front(std::vector<T, Allocator>& vec)
{
    T ret = vec.front();
    vec.erase(vec.begin());
    return ret;
}

template <typename T, typename Allocator>
inline T pop(std::vector<T, Allocator>& vec)
{
    T ret = vec.back();
    vec.pop_back();
    return ret;
}

template <typename T, typename Allocator>
inline T sum(const std::vector<T, Allocator>& vec)
{
    return std::accumulate(vec.begin(), vec.end(), T());

}

template <typename T, typename Allocator>
inline bool contains(const std::vector<T, Allocator>& vec, const T& element)
{
    return std::find(vec.begin(), vec.end(), element) != vec.end();
}

template <typename T, typename Allocator>
inline void sort(std::vector<T, Allocator>& vec)
{
    std::sort(vec.begin(), vec.end());
}

template <typename T, typename Allocator>
T& (max)(std::vector<T, Allocator>& vec)
{
    return *(std::max_element(vec.begin(), vec.end()));
}

template <typename T, typename Allocator>
const T& (max)(const std::vector<T, Allocator>& vec)
{
    return *(std::max_element(vec.begin(), vec.end()));
}

template <typename T, typename Allocator, typename U>
inline std::vector<T, Allocator>& operator<<(std::vector<T, Allocator>& vec, const U& obj)
{
    vec.push_back(obj);
    return vec;
}

template <typename T, typename Allocator>
inline std::vector<T, Allocator> operator+(std::vector<T, Allocator>& target, const std::vector<T, Allocator>& other)
{
    std::vector<T, Allocator> ret(target);
    append(ret, other);
    return ret;
}

/**
 * set
 */
template <typename T, typename Allocator, typename U>
inline std::set<T, Allocator>& operator<<(std::set<T, Allocator>& set, const U& obj)
{
    set.insert(obj);
    return set;
}

template <typename T, typename Allocator>
const T& (min)(const std::set<T, Allocator>& set)
{
    return *set.begin();
}

template <typename T, typename Allocator>
const T& (max)(const std::set<T, Allocator>& set)
{
    return *set.rbegin();
}

template <typename T, typename Allocator>
void erase_min(std::set<T, Allocator>& set)
{
    set.erase(set.begin());
}

template <typename T, typename Allocator>
inline std::set<T, Allocator>& operator<<(std::set<T, Allocator>& set, const T& elem)
{
    set.insert(elem);
    return set;
}

template <typename T, typename Allocator>
inline std::set<T, Allocator> operator&&(
    const std::set<T, Allocator>& set,
    const std::set<T, Allocator>& other)
{
    std::set<T, Allocator> result;
    std::set_intersection(
        set.begin(), set.end(),
        other.begin(), other.end(),
        std::inserter(result, result.begin()));
    return result;
}

template <typename T, typename Allocator>
inline std::set<T, Allocator> operator||(
    const std::set<T, Allocator>& set,
    const std::set<T, Allocator>& other)
{
    std::set<T, Allocator> result;
    std::set_union(
        set.begin(), set.end(),
        other.begin(), other.end(),
        std::inserter(result, result.begin()));
    return result;
}

/**
 * Output
 */
template<typename _CharT, typename _Traits, typename T, typename Allocator>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& os,
           const std::vector<T, Allocator>& vec)
{
    os << "[";

    if (vec.size()) {
        typename std::vector<T, Allocator>::const_iterator i = vec.begin();
        os << *i;
        ++i;

        for (; i != vec.end(); ++i) {
            os << ", " << *i;
        }
    }

    os << "]";

    return os;
}

template<typename _CharT, typename _Traits, typename Key, typename Value, typename Allocator>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& os,
           const std::map<Key, Value, Allocator>& map)
{
    os << "{";

    if (map.size()) {
        typename std::map<Key, Value, Allocator>::const_iterator i = map.begin();
        os << i->first << " => " << i->second;
        ++i;

        for (; i != map.end(); ++i) {
            os << ", " << i->first << " => " << i->second;
        }

    }

    os << "}";

    return os;
}

template<typename _CharT, typename _Traits, typename _T1, typename _T2>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& os,
           const std::pair<_T1, _T2>& p)
{
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

template<typename _CharT, typename _Traits, typename T, typename Allocator>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& os,
           const std::set<T, Allocator>& set)
{
    os << "{";

    if (set.size()) {
        typename std::set<T, Allocator>::const_iterator i = set.begin();
        os << *i;
        ++i;

        for (; i != set.end(); ++i) {
            os << ", " << *i;
        }
    }

    os << "}";

    return os;
}


}

#endif