#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>

#define is_random_access_iterator(It) std::is_base_of_v<std::random_access_iterator_tag, \
                typename std::iterator_traits<It>::iterator_category>

#define is_bidirect_iterator(It) std::is_base_of_v<std::bidirectional_iterator_tag, \
                typename std::iterator_traits<It>::iterator_category>

#define macro_requires(condition) \
        std::enable_if_t<\
            condition,\
            bool> = true\

template<class It>
class reverse_iterator_ad{
    It current_;

public:
    using iterator_type     = It;
    using iterator_concept  = std::conditional_t<
            std::random_access_iterator<It>, 
            std::random_access_iterator_tag, 
            std::bidirectional_iterator_tag>;

    using iterator_category = std::conditional_t<
            std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>, 
            std::random_access_iterator_tag, 
            typename std::iterator_traits<It>::iterator_category>;

    using value_type        = std::iter_value_t<It>;
    using difference_type   = int;
    using pointer           = std::iterator_traits<It>::pointer;
    using reference         = std::iter_reference_t<It>;

    reverse_iterator_ad() = default;

    explicit reverse_iterator_ad(It itr) : current_(itr) {}

    reverse_iterator_ad& operator= (const reverse_iterator_ad& other) = default;

    It base() const {
        return current_;
    }

    reference operator*() const {
        return *std::prev(current_);
    }

    pointer operator->() const {
        return current_;
    }

    template<typename FuncIterType = It, 
        std::enable_if_t<
            std::is_base_of_v<
                std::random_access_iterator_tag,
                typename std::iterator_traits<FuncIterType>::iterator_category> &&
            std::is_same_v<FuncIterType, It>,
            bool> 
        = true>
    reference operator[](difference_type n) {
        return current_[-n - 1];
    }
 
    reverse_iterator_ad& operator++() { 
        --current_; 
        return *this; 
    }

    reverse_iterator_ad operator++(int) {
        reverse_iterator_ad tmp = *this;
        --current_; 
        return tmp; 
    }
 
    reverse_iterator_ad& operator--() { 
        ++current_; 
        return *this; 
    }

    reverse_iterator_ad operator--(int) { 
        reverse_iterator_ad tmp = *this; 
        ++current_; 
        return tmp; 
    }

    template<typename T = It, macro_requires(is_random_access_iterator(T))>
    reverse_iterator_ad operator+(difference_type n) const {
        return reverse_iterator_ad(current_ - n);
    }

    template<typename T = It, macro_requires(is_random_access_iterator(T))>
    reverse_iterator_ad operator-(difference_type n) const {
        return reverse_iterator_ad(current_ + n);
    }

    template<typename T = It, macro_requires(is_random_access_iterator(T))>
    difference_type operator- (const reverse_iterator_ad& other) const {
        return other.base() - this->base();
    }

    template<typename T = It, macro_requires(is_random_access_iterator(T))>
    reverse_iterator_ad operator+= (int n) {
        current_ -= n; return *this;
    }

    template<typename T = It, macro_requires(is_random_access_iterator(T))>
    reverse_iterator_ad operator-= (int n) {
        current_ += n; return *this;
    }

    bool operator== (const reverse_iterator_ad& other) const noexcept {
        return this->base() == other.base();
    }

    bool operator!= (const reverse_iterator_ad& other) const noexcept {
        return this->base() != other.base();
    }

    auto operator<=> (const reverse_iterator_ad& other) const noexcept {
        return this->base() <=> other.base();
    }

};

template<typename  It, macro_requires(is_random_access_iterator(It))>
reverse_iterator_ad<It> operator+(typename reverse_iterator_ad<It>::difference_type n, const reverse_iterator_ad<It>& other) {
    return other + n;
}

#undef is_random_access_iterator