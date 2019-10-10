#pragma once
#include <iterator>

namespace dv::util {
// Class allowing iteration over scoped enumerations
template<typename E>
class EnumIterator final
{
    using ValueType = typename std::underlying_type_t<E>;

public:
    // Traits to iterate
    using iterator_category = std::random_access_iterator_tag;
    using value_type = E;
    using difference_type = std::ptrdiff_t;
    using pointer = E*;
    using reference = E&;

    bool operator==(const EnumIterator& rhs) const { return value == rhs.value; }
    bool operator!=(const EnumIterator& rhs) const { return !(*this == rhs); }
    bool operator<(const EnumIterator& rhs) const { return value < rhs.value; }
    bool operator<=(const EnumIterator& rhs) const { return value <= rhs.value;}
    bool operator>(const EnumIterator& rhs) const { return value > rhs.value; }
    bool operator>=(const EnumIterator& rhs) const { return value >= rhs.value; }

    E operator*() const { return static_cast<E>(value); }

    EnumIterator& operator++() { ++value; return *this; }
    EnumIterator operator++(int) { EnumIterator ret(value); ++value; return ret; }
    EnumIterator& operator--() { --value; return *this; }
    EnumIterator operator--(int) { EnumIterator ret(value); --value; return ret; }

    EnumIterator operator+(ValueType val) const { return EnumIterator(value + val); }
    EnumIterator operator-(ValueType val) const { return EnumIterator(value - val); }
    EnumIterator& operator+=(ValueType val) { value += val; return *this; }
    EnumIterator& operator-=(ValueType val) { value -= val; return *this; }

//    ValueType operator-(const EnumIterator& rhs) const { return EnumIterator(value - rhs.value); } //check usage
private:
    EnumIterator(E enumValue) : value(static_cast<ValueType>(enumValue)) {};
    EnumIterator(ValueType val) : value(val) {};
    
    ValueType value; 
    template<typename T> friend class EnumIteratorProxy;
};

template<typename E>
class EnumIteratorProxy final
{
    using ValueType = typename std::underlying_type_t<E>;
public:
    EnumIterator<E> Begin() const { return EnumIterator<E>(0); }
    EnumIterator<E> End() const { return EnumIterator<E>(E::_COUNT); }
};

template<typename E> EnumIteratorProxy<E> IterateEnum() { return EnumIteratorProxy<E>{}; }
template<typename E> typename ::dv::util::EnumIterator<E> begin(const EnumIteratorProxy<E>& rhs) { return rhs.Begin(); }
template<typename E> typename ::dv::util::EnumIterator<E> end(const EnumIteratorProxy<E>& rhs) { return rhs.End(); }
} // end namespace dv::util