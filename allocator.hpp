#ifndef STLITE_ALLOCATOR_HPP
#define STLITE_ALLOCATOR_HPP

#include "algorithm.hpp"
#include <cstdlib>

// Basic allocator
template <class elemType>
class __malloc_allocator {
private:
    elemType* _data; // First address of all data

    size_t _num; // Total size

public:
    // use malloc() to allocate a piece of memory
    __malloc_allocator(const size_t num) noexcept : 
        _num(num), _data(calloc(num, sizeof(elemType))) {}

    // copy constructor
    __malloc_allocator(const __malloc_allocator& x) {
        _num = x._num;
        _data = calloc(_num, sizeof(elemType));
        for (size_t i = 0; i < _num; ++i) {
            if (x[i]) new(_data + i) elemType(*x[i]);
        }
    }

    // move constructor
    __malloc_allocator(__malloc_allocator&& x) noexcept {
        _data = x._data;
        _num = x._num;
        x._data = nullptr;
        x._num = 0;
    }

    // free all memory when being deconstructed
    ~__malloc_allocator() noexcept {
        for (size_t i = 0; i < _num; ++i) {
            if (_data[i])
                (_data + i)->~elemType();
        }
        free(_data);
    }
    
    // resize the memory
    elemType* reallocate(const size_t num) noexcept {
        elemType* new_data = calloc(num, sizeof(elemType));
        size_t i;
        for (i = 0; i < _num && i < num; ++i) {
            if (_data[i]) new(new_data + i) elemType(move(_data[i]));
        }
        for (; i < _num; ++i) {
            if (_data[i]) (_data + i)->~elemType();
        }
        free(_data);
        _data = new_data;
        _num = num;
        return _data;
    }

    // resize the memory and initialize rest memory with value
    elemType* reallocate(const size_t num, const elemType& value) noexcept {
        elemType* new_data = calloc(num, sizeof(elemType));
        size_t i;
        for (i = 0; i < _num && i < num; ++i) {
            if (_data[i]) new(new_data + i) elemType(move(_data[i]));
        }
        for (; i < num; ++i) {
            new(new_data + i) elemType(value);
        }
        for (; i < _num; ++i) {
            if (_data[i]) (_data + i)->~elemType();
        }
        free(_data);
        _data = new_data;
        _num = num;
        return _data;
    }

    void remove(size_t idx) {
        if (_data[idx]) {
            (_data + idx)->~elemType();
            _data[idx] = nullptr;
        }
    }

    void clean() {
        for (size_t i = 0; i < _num; ++i)
            remove(i);
    }

    elemType* operator[](size_t idx) {
        return _data + idx;
    }

    size_t length() const noexcept {
        return _num;
    }

    size_t size() const noexcept {
        return _num * sizeof(elemType);
    }
};

template <class elemType>
class __linknode_allocator : public __malloc_allocator<elemType> {
private:
    __linknode_allocator* _next;

public:
    __linknode_allocator(const size_t num, const __linknode_allocator* next) noexcept :
        __malloc_allocator<elemType>(num), _next(next) {}
    
    const __linknode_allocator* next() const noexcept {
        return _next;
    }

    __linknode_allocator*& next() noexcept {
        return _next;
    }
};

#endif // STLITE_ALLOCATOR_HPP