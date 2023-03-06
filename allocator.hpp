#ifndef STLITE_ALLOCATOR_HPP
#define STLITE_ALLOCATOR_HPP

#include <cstdlib>
#include "algorithm.hpp"

// Basic allocator
template <class elemType>
class __malloc_allocator {
private:
    elemType* _data; // First address of all data

    bool* _used;

    size_t _num; // Total size

public:
    // use malloc() to allocate a piece of memory
    __malloc_allocator(const size_t num) noexcept : 
        _num(num) {
            _used = static_cast<bool*>(calloc(num, sizeof(bool)));
            _data = static_cast<elemType*>(calloc(num, sizeof(elemType)));
        }

    // copy constructor
    __malloc_allocator(const __malloc_allocator& x) {
        _num = x._num;
        _used = static_cast<bool*>(calloc(_num, sizeof(bool)));
        _data = static_cast<elemType*>(calloc(_num, sizeof(elemType)));
        for (size_t i = 0; i < _num; ++i) {
            if (x.has_value(i)) {
                new(_data + i) elemType(*x[i]);
                _used[i] = true;
            }
        }
    }

    // Move constructor
    __malloc_allocator(__malloc_allocator&& x) noexcept {
        _data = x._data;
        _num = x._num;
        _used = x._used;
        x._data = nullptr;
        x._used = nullptr;
        x._num = 0;
    }

    // free all memory when being deconstructed
    ~__malloc_allocator() noexcept {
        for (size_t i = 0; i < _num; ++i) {
            if (has_value(i))
                (_data + i)->~elemType();
        }
        free(_data);
        free(_used);
    }
    
    // resize the memory
    elemType* reallocate(const size_t num) noexcept {
        elemType* new_data = static_cast<elemType*>(calloc(num, sizeof(elemType)));
        bool* new_used = static_cast<bool*>(calloc(num, sizeof(bool)));
        size_t i;
        for (i = 0; i < _num && i < num; ++i) {
            if (has_value(i)) {
                new(new_data + i) elemType(Move(_data[i]));
                (_data + i)->~elemType();
                new_used[i] = true;
            }
        }
        for (; i < _num; ++i) {
            if (has_value(i)) (_data + i)->~elemType();
        }
        free(_data);
        free(_used);
        _data = new_data;
        _num = num;
        _used = new_used;
        return _data;
    }

    // resize the memory and initialize rest memory with value
    /*elemType* reallocate(const size_t num, const elemType& value) noexcept {
        elemType* new_data = static_cast<elemType*>(calloc(num, sizeof(elemType)));
        size_t i;
        for (i = 0; i < _num && i < num; ++i) {
            if (has_value(i)) {
                new(new_data + i) elemType(Move(_data[i]));
                (_data + i)->~elemType();
            }
        }
        for (; i < num; ++i) {
            new(new_data + i) elemType(value);
            _used[i] = true;
        }
        for (; i < _num; ++i) {
            if (has_value(i)) (_data + i)->~elemType();
        }
        free(_data);
        free(_used);
        _data = new_data;
        _num = num;
        _used = static_cast<bool*>(calloc(num, sizeof(bool)));
        return _data;
    }*/

    void copy(const __malloc_allocator& x) {
        clean();
        free(_data), free(_used);
        _num = x._num;
        _used = static_cast<bool*>(calloc(_num, sizeof(bool)));
        _data = static_cast<elemType*>(calloc(_num, sizeof(elemType)));
        for (size_t i = 0; i < _num; ++i) {
            if (x.has_value(i)) {
                new(_data + i) elemType(*x[i]);
                _used[i] = true;
            }
        }
    }

    inline void remove(size_t idx) {
        if (has_value(idx)) {
            (_data + idx)->~elemType();
            _used[idx] = false;
        }
    }

    inline void construct(size_t idx, const elemType& value) {
        new(_data + idx) elemType(value);
        _used[idx] = true;
    }

    inline void construct(size_t idx, elemType&& value) {
        new(_data + idx) elemType(value);
        _used[idx] = true;
    }

    inline void clean() {
        for (size_t i = 0; i < _num; ++i)
            remove(i);
    }

    inline elemType* data(size_t idx) noexcept {
        return _data + idx;
    }

    inline const elemType* data(size_t idx) const noexcept {
        return _data + idx;
    }

    inline elemType* operator[](size_t idx) noexcept {
        return _data + idx;
    }

    inline const elemType* operator[](size_t idx) const noexcept {
        return _data + idx;
    }

    inline size_t length() const noexcept {
        return _num;
    }

    inline size_t size() const noexcept {
        return _num * sizeof(elemType);
    }

    inline bool has_value(size_t idx) const noexcept {
        return _used[idx];
    }

    inline void set_used(size_t idx, bool val) noexcept {
        _used[idx] = val;
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