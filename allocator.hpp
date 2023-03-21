#ifndef STLITE_ALLOCATOR_HPP
#define STLITE_ALLOCATOR_HPP

#include <cstdlib>
#include "utilities.hpp"

namespace s7a9 {
    // Basic allocator
    template <class elemType>
    class __malloc_allocator {
    private:
        elemType* _data; // First address of all data

        bool* _used;

        size_t _num; // Total size

    public:
        // use malloc() to allocate a piece of memory
        explicit __malloc_allocator(const size_t num) noexcept :
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
        ~__malloc_allocator() {
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

        void copy(const __malloc_allocator& other) {
            if (this == &other) return;
            clean();
            free(_data), free(_used);
            _num = other._num;
            _used = static_cast<bool*>(calloc(_num, sizeof(bool)));
            _data = static_cast<elemType*>(calloc(_num, sizeof(elemType)));
            for (size_t i = 0; i < _num; ++i) {
                if (other.has_value(i)) {
                    new(_data + i) elemType(*other[i]);
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

        inline void move_elem(size_t dst, size_t src) {
            construct(dst, Move(_data[src]));
            remove(src);
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

        inline bool has_value(size_t idx) const noexcept {
            return _used[idx];
        }

        inline void set_used(size_t idx, bool val) noexcept {
            _used[idx] = val;
        }

        inline void swap(__malloc_allocator& other) noexcept {
            s7a9::swap(_data, other._data);
            s7a9::swap(_used, other._used);
            s7a9::swap(_num, other._num);
        }
    };

    // Basic allocator
    template <class elemType>
    class __new_allocator {
    private:
        elemType** _data; 

        size_t _num; // Total size

    public:
        // use new to allocate a pointer table
        explicit __new_allocator(const size_t num) noexcept :
            _num(num) {
            _data = new elemType * [num];
            for (size_t i = 0; i < num; ++i) {
                _data[i] = nullptr;
            }
        }

        // copy constructor
        __new_allocator(const __new_allocator& x) : 
            _num(x._num) {
            _data = new elemType * [_num];
            for (size_t i = 0; i < _num; ++i) {
                if (x[i]) _data[i] = new elemType(*x[i]);
                else _data[i] = nullptr;
            }
        }

        // Move constructor
        __new_allocator(__new_allocator&& x) noexcept {
            _num = x._num;
            _data = x._data;
            x._data = nullptr;
            x._num = 0;
        }

        // free all memory when being deconstructed
        ~__new_allocator() {
            clean();
            delete[] _data;
        }

        // resize the memory
        elemType* reallocate(const size_t num) noexcept {
            elemType** new_data = new elemType * [num];
            size_t i;
            for (i = 0; i < _num && i < num; ++i) {
                new_data[i] = _data[i];
            }
            for (; i < num; ++i) {
                new_data[i] = nullptr;
            }
            for (; i < _num; ++i) {
                delete _data[i];
            }
            delete[] _data;
            _num = num;
            return *(_data = new_data);
        }

        void copy(const __new_allocator& x) {
            if (this == &x) return;
            clean();
            delete[] _data;
            _num = x._num;
            _data = new elemType * [_num];
            for (size_t i = 0; i < _num; ++i) {
                if (x[i]) _data[i] = new elemType(*x[i]);
                else _data[i] = nullptr;
            }
        }

        inline void remove(size_t idx) {
            delete _data[idx];
            _data[idx] = nullptr;
        }

        inline void move_elem(size_t dst, size_t src) {
            _data[dst] = _data[src];
            _data[src] = nullptr;
        }

        inline void construct(size_t idx, const elemType& value) {
            _data[idx] = new elemType(value);
        }

        inline void construct(size_t idx, elemType&& value) {
            _data[idx] = new elemType(value);
        }

        inline void clean() {
            if (_data == nullptr) return;
            for (size_t i = 0; i < _num; ++i)
                remove(i);
        }

        inline elemType* data(size_t idx) noexcept {
            return _data[idx];
        }

        inline const elemType* data(size_t idx) const noexcept {
            return _data[idx];
        }

        inline elemType* operator[](size_t idx) noexcept {
            return _data[idx];
        }

        inline const elemType* operator[](size_t idx) const noexcept {
            return _data[idx];
        }

        inline size_t length() const noexcept {
            return _num;
        }

        inline bool has_value(size_t idx) const noexcept {
            return _data[idx] != nullptr;
        }

        inline void set_used(size_t idx, bool val) noexcept {
            // Useless in this kind of allocator
        }

        inline void swap(__new_allocator& other) noexcept {
            s7a9::swap(_data, other._data);
            s7a9::swap(_num, other._num);
        }
    };
}

#endif // STLITE_ALLOCATOR_HPP