#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <type_traits>

#include "reverse_iterator.hpp"

template<bool IsConst, typename T>
class base_iterator {
public:
    using value_type        = T;
    using reference         = std::conditional_t<IsConst, const T&, T&>;
    using pointer           = std::conditional_t<IsConst, const T*, T*>;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = int;

private:
    pointer ptr_;
    T* mem_start_;
    size_t real_idx_;
    size_t cap_;

    pointer move_ptr(pointer ptr, difference_type n) const noexcept {
        int dif = (n >= 0 ? mem_start_ + cap_ - 1 - ptr : ptr - mem_start_);
        if (dif >= abs(n)) {
            ptr += n;
        }
        else {
            n >= 0 ? n -= dif : n += dif;
            ptr = mem_start_ + (n >= 0 ? n - 1 : cap_ + n);
        }
        return ptr;
    }

public:

    base_iterator() : ptr_(nullptr), mem_start_(nullptr), real_idx_(0), cap_(0) {}

    explicit base_iterator(pointer ptr, T* start, size_t cap, size_t idx) 
        : ptr_(ptr), mem_start_(start), cap_(cap), real_idx_(idx) {}


    base_iterator(const base_iterator& other) = default; 

    friend class base_iterator<!IsConst, T>; 

    base_iterator(const base_iterator<false, T>& other) requires IsConst
         : ptr_(other.ptr_), mem_start_(other.mem_start_), cap_(other.cap_), real_idx_(other.real_idx_) {}

    reference operator*() const noexcept {
        return *ptr_;
    }

    pointer operator->() const noexcept {
        return ptr_;
    }

    reference operator[](difference_type n) const noexcept {
        pointer tmp = ptr_;
        tmp = move_ptr(tmp, n);
        return *tmp;
    }

    base_iterator& operator++() noexcept {
        return (*this) += 1;
    }

    base_iterator operator++(int) noexcept {
        base_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    base_iterator& operator--() noexcept {
        return (*this) -= 1;
    }

    base_iterator operator--(int) noexcept {
        base_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    base_iterator& operator+=(difference_type n) noexcept {
        real_idx_ += n;
        ptr_ = move_ptr(ptr_, n);
        return *this;
    }

    base_iterator& operator-=(difference_type n) noexcept {
        return *this += (-n);
    }

    base_iterator operator+(difference_type n) const {
        base_iterator temp = *this;
        return temp += n;
    }

    base_iterator operator-(difference_type n) const {
        base_iterator temp = *this;
        return temp -= n;
    }

    difference_type operator-(const base_iterator& other) const {
        return real_idx_ - other.real_idx_;
    }

    auto operator<=>(const base_iterator& other) const noexcept {
        return real_idx_ <=> other.real_idx_;
    }

    bool operator==(const base_iterator& other) const noexcept {
        return real_idx_ == other.real_idx_;
    }

    bool operator!=(const base_iterator& other) const noexcept {
        return !(*this == other);
    }

    pointer raw_ptr() const noexcept {
        return ptr_;
    }
};


template<bool IsConst, typename T>
base_iterator<IsConst, T> 
        operator+(typename base_iterator<IsConst, T>::difference_type n, 
                  const base_iterator<IsConst, T>& a) {;
    return a + n;
}
