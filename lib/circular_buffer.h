#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <memory>

#include "iterator.hpp"
#include "reverse_iterator.hpp"

#define is_input_iterator(It) std::is_base_of_v<std::input_iterator_tag, \
                typename std::iterator_traits<It>::iterator_category>


template<typename T, bool Extendable = false, typename Allocator = std::allocator<T>>
class circular_buffer { 
public:
    using const_iterator         = base_iterator<true, T>;
    using iterator               = base_iterator<false, T>;
    using const_reverse_iterator = reverse_iterator_ad<const_iterator>;
    using reverse_iterator       = reverse_iterator_ad<iterator>;
    using value_type             = T;
    using reference              = T&;
    using const_reference        = const T&;
    using difference_type        = int;
    using size_type              = size_t;
    using allocator_type         = Allocator;

    using alloc_traits = std::allocator_traits<Allocator>;

private:
    T* buff_;
    size_type sz_;
    size_type cap_;
    size_type begin_;
    size_type end_;
    Allocator alloc_;


    void rollback(T* buf, size_type n, size_type buf_size) noexcept {
        for (size_type i = 0; i < n; ++i) {
            alloc_traits::destroy(alloc_, buf + i);
        }
        alloc_.deallocate(buf, buf_size);
    }

    template<typename U>
    void copy_to_end(T* dest, size_type dest_size, T* dest_start, size_type count, const U& obj_) {
        size_type idx = 0;
        try {   
            U obj = obj_;
            for (; idx < count; ++idx) {
                if constexpr (std::input_iterator<U>) {
                    alloc_traits::construct(alloc_, dest_start + idx, *obj);
                    ++obj;
                }
                else {
                    alloc_traits::construct(alloc_, dest_start + idx, obj);
                }
            }
        }
        catch(...) {
            rollback(dest, dest_start - dest + idx, dest_size);
            throw;
        }
    }

    void update_buf(T* new_buff, size_type new_cap) noexcept {
        for (auto it = begin(); it != end(); ++it)
            alloc_traits::destroy(alloc_, &*it);
        alloc_.deallocate(buff_, cap_);
        buff_ = new_buff;
        cap_ = new_cap;
    }


    void ext_push(const T& val, bool is_front) {
        size_type place = 0;
        
        if (sz_ < cap_ ) {
            alloc_traits::construct(alloc_, is_front ? (begin() - 1).raw_ptr() : end().raw_ptr(), val);
        }
        else {
            size_type new_cap = sz_ == 0 ? 1 : sz_ * 2;
            T* new_buff = alloc_.allocate(new_cap);
            copy_to_end(new_buff, new_cap, new_buff, end() - begin(), begin());

            try {
                alloc_traits::construct(alloc_, new_buff + (is_front ? new_cap - 1 : sz_), val);
            } catch (...) {
                rollback(new_buff, sz_, new_cap);
                throw;
            }

            update_buf(new_buff, new_cap);
        }

        ++sz_;
    }

    //single method without strong exception guarantee, only base guarantee
    void not_ext_push(const T& val, bool is_front) {
        if (cap_ == 0) {
            return;
        }

        iterator it = is_front ? begin() - 1 : end();

        if (full()) {
            alloc_traits::destroy(alloc_, it.raw_ptr()); 
            --sz_;
            if (is_front) {
                end_ == 0 ? end_ = cap_ - 1 : --end_;
            }
            else {
                begin_ == cap_ - 1 ? begin_ = 0 : ++begin_;
            }
        }

        alloc_traits::construct(alloc_, it.raw_ptr(), val);

        if (is_front) {
            begin_ == 0 ? begin_ = cap_ - 1 : --begin_;
        }
        else {
            end_ == cap_ - 1 ? end_ = 0 : ++end_;
        }
        ++sz_;

    }

    T* ptr_to_begin() const noexcept {
        return sz_ == 0 ? buff_ : buff_ + begin_;
    }

    T* ptr_to_end() const noexcept {
        return sz_ == 0 ? buff_ : buff_ + (end_ + 1) % cap_;
    }

    void set_default_bounds() {
        begin_ = 0;
        end_ = sz_ - 1;
    }


public:
    circular_buffer() : buff_(nullptr), sz_(0), cap_(0), begin_(0), end_(cap_ - 1), alloc_{} {}
    
    explicit circular_buffer(size_type capacity) : 
        buff_(nullptr), cap_(capacity), sz_(0), begin_(0), end_(cap_ - 1), alloc_{} 
        {
            if (cap_ > 0) {
                buff_ = alloc_traits::allocate(alloc_, cap_);
            }
        }
    
    explicit circular_buffer(size_type capacity, const Allocator& alloc) : 
        buff_(nullptr), cap_(capacity), sz_(0), begin_(0), end_(cap_ - 1),
        alloc_(alloc) 
        {
            if (cap_ > 0) {
                buff_ = alloc_traits::allocate(alloc_, cap_);
            }
        }
    
    circular_buffer(const Allocator& alloc) : circular_buffer(0, alloc) {}

    explicit circular_buffer(size_type size, const T& value) : 
        buff_(nullptr), cap_(0), sz_(0), begin_(0), end_(cap_ - 1), alloc_{} 
    {
        if (size > 0) {
            T* new_buff = alloc_.allocate(size);
            copy_to_end(new_buff, size, new_buff, size, value);
            update_buf(new_buff, size);
            sz_ = size;
            set_default_bounds();
        }
    }

    template<typename InputIt, macro_requires(is_input_iterator(InputIt))>
    explicit circular_buffer(InputIt first, InputIt last) : 
        buff_(nullptr), cap_(0), sz_(0), begin_(0), end_(cap_ - 1), alloc_{} 
    {
        if (first != last) {
            size_type size = std::distance(first, last);
            T* new_buff = alloc_.allocate(size);
            copy_to_end(new_buff, size, new_buff, size, first);
            update_buf(new_buff, size);
            sz_ = size;
            set_default_bounds();
        }
    }
    
    circular_buffer(const std::initializer_list<T>& il) : 
            circular_buffer(il.begin(), il.end()) {}        

    circular_buffer (const circular_buffer& other) :
        buff_(nullptr), cap_(other.cap_), sz_(other.sz_), begin_(0), end_(other.sz_ - 1),
        alloc_(alloc_traits::select_on_container_copy_construction(other.alloc_))
    {
        if (other.cap_ > 0) {
            T* new_buff = alloc_.allocate(other.cap_);
            copy_to_end(new_buff, other.cap_, new_buff, other.sz_, other.begin());
            buff_ = new_buff;
        }
    }

    circular_buffer& operator= (const circular_buffer& other) {
        circular_buffer tmp(other);
        this->swap(tmp);
        return *this;
    }

    circular_buffer& operator= (const std::initializer_list<T>& il) {
        circular_buffer tmp(il);
        this->swap(tmp);
        return *this;
    }




    allocator_type get_allocator() const noexcept {
        return alloc_;
    }




    iterator begin() noexcept {
        return iterator(ptr_to_begin(), buff_, cap_, 0);
    }

    iterator end() noexcept {
        return iterator(ptr_to_end(), buff_, cap_, sz_);
    }

    const_iterator begin() const noexcept {
        return const_iterator(ptr_to_begin(), buff_, cap_, 0);
    }

    const_iterator end() const noexcept {
        return const_iterator(ptr_to_end(), buff_, cap_, sz_);
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(ptr_to_begin(), buff_, cap_, 0);
    }

    const_iterator cend() const noexcept {
        return const_iterator(ptr_to_end(), buff_, cap_, sz_);
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }




    void push_back(const T& val) {
        if constexpr (Extendable) {
            ext_push(val, false);
            end_ == cap_ - 1 ? end_ = 0 : ++end_;
        }
        else {
            not_ext_push(val, false);
        }    
    }

    void push_front(const T& val) {
        if constexpr (Extendable) {
            ext_push(val, true);
            begin_ == 0 ? begin_ = cap_ - 1 : --begin_;
        }
        else {
            not_ext_push(val, true);
        }
        
    }


    template<typename InputIt, macro_requires(is_input_iterator(InputIt) && Extendable)>
    iterator insert(const_iterator pos, InputIt first, InputIt last) { 
        if (cap_ == 0 && first != last) {
            T* new_buff = alloc_.allocate(1);
            update_buf(new_buff, 1);
        }

        size_type count = last - first;
        size_type new_cap = cap_;
        while (sz_ + count > new_cap) {
            new_cap *= 2;
        }

        T* new_buff = new_cap == 0 ? nullptr : alloc_.allocate(new_cap);
        size_type first_idx = pos - begin();
        size_type idx = 0;

        copy_to_end(new_buff, new_cap, new_buff, pos - begin(), begin());

        copy_to_end(new_buff, new_cap, new_buff + first_idx, count, first);

        copy_to_end(new_buff, new_cap, new_buff + first_idx + count, cend() - pos, pos);

        update_buf(new_buff, new_cap);

        sz_ += count;

        set_default_bounds();

        return begin() + first_idx;
    }

    iterator insert(const_iterator pos, const T& value) requires Extendable {
        return insert(pos, 1, value);
    }

    template<bool B = Extendable, std::enable_if<B, bool> = false>
    iterator insert(const_iterator pos, const T& value) {
        return insert(pos, 1, value);
    }


    template<typename InputIt, macro_requires(is_input_iterator(InputIt) && !Extendable)>
    iterator insert(const_iterator pos, InputIt first, InputIt last) { 
        circular_buffer<T, true> temp{begin(), end()};
        temp.insert(temp.begin() + (pos - begin()), first, last);
        circular_buffer<T, false> temp2{temp.end() - capacity(), temp.end()};
        swap(temp2);
        return begin();
    }

    iterator insert(const_iterator pos, const T& value) requires (!Extendable) {
        if (pos == begin() && full()) {
            return begin();
        }
        size_type offset = pos - begin();
        if (full()) {
            --offset;
        }
        insert(pos, 1, value);
        return begin() + offset;
    }



    iterator insert(const_iterator pos) {
        return insert(pos, T());
    }

    iterator insert(const_iterator pos, size_type count, const T& value) {
        circular_buffer temp(count, value);
        return insert(pos, temp.begin(), temp.end());
    }

    iterator insert(const_iterator pos, const std::initializer_list<T>& il) {
        return insert(pos, il.begin(), il.end());
    }




    void pop_back() {
        if (sz_ == 0) {
            throw std::out_of_range("empty container");
        }
        alloc_traits::destroy(alloc_, buff_ + end_);
        end_ == 0 ? end_ = cap_ - 1 : --end_;
        --sz_;
    }

    void pop_front() {
        if (sz_ == 0) {
            throw std::out_of_range("empty container");
        }
        alloc_traits::destroy(alloc_, buff_ + begin_);
        begin_ == cap_ - 1 ? begin_ = 0 : ++begin_;
        --sz_;
    }

    iterator erase(const_iterator first, const_iterator last) {
        T* new_buff = alloc_.allocate(cap_);
        size_type diff = first - begin();
        
        copy_to_end(new_buff, cap_, new_buff, 
                first - cbegin(), begin());
        copy_to_end(new_buff, cap_, new_buff + diff, 
                cend() - last, begin() + (last - cbegin()));
        update_buf(new_buff, cap_);

        sz_ -= (last - first);
        set_default_bounds();

        return begin() + diff;
    }

    iterator erase(const_iterator pos) {
        return erase(pos, std::next(pos));
    }




    void swap(circular_buffer& other) noexcept {
        std::swap(sz_, other.sz_);
        std::swap(cap_, other.cap_);
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
        std::swap(buff_, other.buff_);

        if constexpr (alloc_traits::propagate_on_container_swap::value) {
            std::swap(alloc_, other.alloc_);
        } else {
            assert(alloc_ == other.alloc_); 
        }
    }
    

    void clear() noexcept {
        for (auto it = begin(); it != end(); ++it)
            alloc_traits::destroy(alloc_, &*it);
        sz_ = 0;
        set_default_bounds();
    }


    void reserve(size_type new_cap) {
        if (new_cap < cap_) {
            return;
        }
        T* new_buff = alloc_.allocate(new_cap);
        copy_to_end(new_buff, new_cap, new_buff, sz_, begin());
        update_buf(new_buff, new_cap);
        set_default_bounds();
    }


    void resize(size_type new_size, const T& value) {
        circular_buffer tmp(begin(), begin() + std::min(new_size, sz_));
        if constexpr (!Extendable) {
            tmp.reserve(new_size);
        }
        for (size_type idx = sz_; idx < new_size; ++idx) {
            tmp.push_back(value);
        }
        swap(tmp);
    }

    void resize(size_type new_size) {
        resize(new_size, T());
    }


    void assign(size_type count, const T& value) {
        circular_buffer tmp(count, value);
        this->swap(tmp);
    }
    
    template<typename InputIt, macro_requires(is_input_iterator(InputIt))>
    void assign(InputIt first, InputIt last) {
        circular_buffer tmp(first, last);
        this->swap(tmp);
    }

    void assign(const std::initializer_list<T>& il) {
        assign(il.begin(), il.end());
    }




    reference front() noexcept {
        return *begin();
    }

    const_reference front() const noexcept {
        return *cbegin();
    }

    const_reference cfront() const noexcept {
        return *cbegin();
    }

    reference back() noexcept {
        return *std::prev(end());
    }

    const_reference back() const noexcept {
        return *std::prev(cend());
    }

    const_reference cback() const noexcept {
        return *std::prev(cend());
    }
    
    reference operator[](size_type n) noexcept {
        return *(begin() + n);
    }

    const_reference operator[](size_type n) const noexcept {
        return *(cbegin() + n);
    }

    reference at(size_type n) {
        if (n >= sz_) {
            throw std::out_of_range{""};
        }
        return (*this)[n];
    }

    const_reference at(size_type n) const {
        if (n >= sz_) {
            throw std::out_of_range{""};
        }
        return (*this)[n];
    }




    auto operator<=>(const circular_buffer& other) const {
        return std::lexicographical_compare_three_way(begin(), end(), other.begin(), other.end());
    }

    bool operator==(const circular_buffer& other) const noexcept {
        return std::equal(this->begin(), this->end(), other.begin(), other.end()); 
    }

    bool operator!=(const circular_buffer& other) const noexcept {
        return !(*this == other);
    }




    size_type size() const noexcept {
        return sz_;
    }

    size_type capacity() const noexcept {
        return cap_;
    }

    size_type max_size() const noexcept {
        return std::allocator_traits<Allocator>::max_size(alloc_);
    }

    bool empty() const noexcept {
        return sz_ == 0;
    }

    bool full() const noexcept {
        return sz_ == cap_;
    }




    ~circular_buffer() {
        clear();
        alloc_.deallocate(buff_, cap_);
    }
};

template<typename T, bool Extendable = false, typename Allocator = std::allocator<T>>
void swap(circular_buffer<T, Extendable, Allocator>& lhs, circular_buffer<T, Extendable, Allocator>& rhs) {
    lhs.swap(rhs);
}