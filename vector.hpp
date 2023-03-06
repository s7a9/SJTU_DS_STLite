#ifndef STLITE_VECTOR_HPP
#define STLITE_VECTOR_HPP

#include "allocator.hpp"

class VectorIndexOutOfRangeException {};

template <class elemType, class Allocator = __malloc_allocator<elemType>>
class vector {
private:
    Allocator _allocator;

    size_t _size;

public:
    class iterator {
    private:
        Allocator* _palloc;

        size_t _idx;

    public:
        iterator(Allocator* p_alloc, size_t idx) {
            _palloc = p_alloc;
            _idx = idx;
        }

        elemType& operator*() {
            return *_palloc[_idx];
        }

        iterator& operator++() {
            if (_palloc.has_value(_idx)) ++_idx;
            return *this;
        }

        iterator operator++(int) {
            iterator iter(*this);
            ++(*this);
            return iter;
        }

        iterator& operator--() {
            if (_idx) --_idx;
            return *this;
        }

        iterator operator--(int) {
            iterator iter(*this);
            --(*this);
            return iter;
        }

        bool operator==(const iterator& rhs) const {
            return _palloc == rhs._palloc && _idx == rhs._idx;
        }

        bool operator!=(const iterator& rhs) const {
            return !(*this == rhs);
        }
    };

    class const_iterator {
    private:
        const Allocator* _palloc;

        size_t _idx;

    public:
        const_iterator(Allocator* p_alloc, size_t idx) :
            _palloc(p_alloc), _idx(idx) {}

        const elemType& operator*() {
            return *_palloc[_idx];
        }

        iterator& operator++() {
            if (_palloc.has_value(_idx)) ++_idx;
            return *this;
        }

        iterator operator++(int) {
            iterator iter(*this);
            ++(*this);
            return iter;
        }

        iterator& operator--() {
            if (_idx) --_idx;
            return *this;
        }

        iterator operator--(int) {
            iterator iter(*this);
            --(*this);
            return iter;
        }

        bool operator==(const iterator& rhs) const {
            return _palloc == rhs._palloc && _idx == rhs._idx;
        }

        bool operator!=(const iterator& rhs) const {
            return !(*this == rhs);
        }
    };

    vector() noexcept(noexcept(Allocator())) : _allocator(10), _size(0) {}

    vector(size_t num, const elemType& value) : _allocator(num * 2), _size(num) {
        for (size_t i = 0; i < _size; ++i) {
            _allocator.construct(i, value);
        }
    }

    vector(const vector& x) : _allocator(x._allocator.length()), _size(x._size) {
        for (size_t i = 0; i < _size; ++i) {
            _allocator.construct(i, x[i]);
        }
    }

    vector(vector&& x) noexcept :
        _allocator(move(x._allocator)), _size(x.size) {
            x._size = 0;
        }

    ~vector() {}

    vector& operator= (const vector& rhs) {
        _allocator.clean();
        _allocator.reallocate(rhs._allocator.length());
        for (size_t i = 0; i < rhs._size; ++i) {
            new(_allocator[i]) elemType(*(rhs._allocator[i]));
        }
        _size = rhs._size;
        return *this;
    }

    vector& operator= (vector&& rhs) {
        _allocator.clean();
        _allocator.reallocate(rhs._allocator.length());
        for (size_t i = 0; i < rhs._size; ++i) {
            new(_allocator[i]) elemType(move(*(rhs._allocator[i])));
            rhs._allocator[i] = nullptr;
        }
        _size = rhs._size;
        return *this;
    }

    [[nodiscard]] bool empty() const noexcept {
        return _size > 0;
    }

    size_t size() const noexcept {
        return _size;
    }

    size_t capacity() const noexcept {
        return _allocator.length();
    }

    void resize(size_t sz) {
        _allocator.reallocate(sz);
    }

    void resize(size_t sz, const elemType& value) {
        _allocator.reallocate(sz, value);
    }

    void shrink_to_fit() {
        _allocator.reallocate(max(10ULL, _size * 2));
    }

    elemType& operator[](size_t idx) {
        return *(_allocator[idx]);
    }

    const elemType& operator[](size_t idx) const {
        return *(_allocator[idx]);
    }

    elemType& at(size_t idx) {
        if (idx >= _size) throw VectorIndexOutOfRangeException();
        return *(_allocator[idx]);
    }

    const elemType& at(size_t idx) const {
        if (idx >= _size) throw VectorIndexOutOfRangeException();
        return *(_allocator[idx]);
    }

    elemType& front() {
        return *(_allocator[0]);
    }

    const elemType& front() const {
        return *(_allocator[0]);
    }

    elemType& back() {
        return *(_allocator[_size - 1]);
    }

    const elemType& back() const {
        return *(_allocator[_size - 1]);
    }

    elemType* data() {
        return _allocator[0];
    }

    const elemType* data() const {
        return _allocator[0];
    }

    void push_back(const elemType& x) {
        if (_allocator.length() == _size + 1) {
            _allocator.reallocate(_allocator.length() * 2);
        }
        new(_allocator[_size]) elemType(x);
        ++_size;
    }

    void push_back(elemType&& x) {
        if (_allocator.length() == _size + 1) {
            _allocator.reallocate(_allocator.length() * 2);
        }
        new(_allocator[_size]) elemType(x);
        ++_size;
    }

    void pop_back() {
        --_size;
        _allocator.remove(_size);
    }

    iterator begin() noexcept {
        return iterator(_allocator, 0);
    }

    const_iterator begin() const noexcept {
        return const_iterator(_allocator, 0);
    }

    iterator end() noexcept {
        return iterator(_allocator, _size);
    }

    const_iterator end() const noexcept {
        return const_iterator(_allocator, _size);
    }

    iterator insert(const_iterator position, const elemType& x) {
        if (_allocator.length() == _size + 1) {
            _allocator.reallocate(_allocator.length() * 2);
        }
        size_t pos = position._idx;
        for (size_t i = _size; i > pos; --i) {
            _allocator.construct(i, move(*_allocator[i - 1]));
            _allocator.remove(i - 1);
        }
        _allocator.construct(i, x);
    }

    iterator insert(const_iterator position, elemType&& x) {
        if (_allocator.length() == _size + 1) {
            _allocator.reallocate(_allocator.length() * 2);
        }
        size_t pos = position._idx;
        for (size_t i = _size; i > pos; --i) {
            _allocator.construct(i, move(*_allocator[i - 1]));
            _allocator.remove(i - 1);
        }
        _allocator.construct(pos, x);
        return iterator(_allocator, pos);
    }

    iterator insert(const_iterator position, size_t n, const elemType& x) {
        if (_allocator.length() == _size + n) {
            _allocator.reallocate(_allocator.length() * 2);
        }
        size_t pos = position._idx;
        for (size_t i = _size; i > pos; --i) {
            _allocator.construct(i + n - 1, move(*_allocator[i - 1]));
            _allocator.remove(i - 1);
        }
        for (size_t i = 0; i < n; ++i)
            _allocator.construct(pos + i, x);
        return iterator(_allocator, pos);
    }

    iterator erase(const_iterator position) {
        size_t pos = position._idx;
        if (pos >= _size) {
            return end();
        }
        _allocator.remove(pos);
        for (size_t i = pos; i < _size; ++i) {
            _allocator.construct(i, move(*_allocator[i + 1]));
            _allocator.remove(i + 1);
        }
        return iterator(_allocator, pos);
    }

    iterator erase(const_iterator first, const_iterator last) {
        size_t pos1 = first._idx, pos2 = last._idx, i;
        for (i = pos1; i < pos2; ++i) {
            _allocator.remove(i);
        }
        while (pos2 < _size) {
            _allocator.construct(pos1, move(*_allocator[pos2]));
            _allocator.remove(pos2);
            ++pos1, ++pos2;
        }
        return iterator(_allocator, first._idx);
    }

    void swap(vector& rhs) {

    }

    void clear() noexcept {
        _allocator.clean();
        //_allocator.reallocate(10);
        _size = 0;
    }
};

#endif // STLITE_VECTOR_HPP