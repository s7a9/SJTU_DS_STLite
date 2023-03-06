#ifndef STLITE_VECTOR_HPP
#define STLITE_VECTOR_HPP

#include <initializer_list>
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

        friend class vector::const_iterator;

    public:
        iterator(Allocator* p_alloc, size_t idx) {
            _palloc = p_alloc;
            _idx = idx;
        }

        elemType& operator*() {
            return *(_palloc->data(_idx));
        }

        iterator& operator++() {
            if (_palloc->has_value(_idx)) ++_idx;
            return *this;
        }

        iterator operator+(int offset) {
            return iterator(_palloc, _idx + offset);
        }

        iterator operator-(int offset) {
            return iterator(_palloc, _idx - offset);
        }

        iterator operator++(int) {
            iterator iter(*this);
            ++(*this);
            return iter;
        }

        friend long long operator-(const iterator& lhs, const iterator& rhs) {
            return (long long)lhs._idx - rhs._idx;
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

        friend class vector;

    public:
        const_iterator(const iterator& iter) :
            _palloc(iter._palloc), _idx(iter._idx) {}

        const_iterator(const Allocator* p_alloc, size_t idx) :
            _palloc(p_alloc), _idx(idx) {}

        const elemType& operator*() const {
            return *(_palloc->data(_idx));
        }

        const_iterator& operator++() {
            if (_palloc->has_value(_idx)) ++_idx;
            return *this;
        }

        const_iterator operator++(int) {
            iterator iter(*this);
            ++(*this);
            return iter;
        }

        const_iterator& operator--() {
            if (_idx) --_idx;
            return *this;
        }

        const_iterator operator--(int) {
            iterator iter(*this);
            --(*this);
            return iter;
        }

        const_iterator operator+(int offset) {
            return const_iterator(_palloc, _idx + offset);
        }

        const_iterator operator-(int offset) {
            return const_iterator(_palloc, _idx - offset);
        }

        friend long long operator-(const const_iterator& lhs, const const_iterator& rhs) {
            return (long long)lhs._idx - rhs._idx;
        }

        bool operator==(const const_iterator& rhs) const {
            return _palloc == rhs._palloc && _idx == rhs._idx;
        }

        bool operator!=(const const_iterator& rhs) const {
            return !(*this == rhs);
        }
    };

    vector() noexcept : _allocator(10), _size(0) {}

    vector(std::initializer_list<elemType> list) : 
        _allocator(list.size() * 2), _size(list.size()) {
        for (auto i = list.begin(); i != list.end(); ++i) {
            _allocator.construct(i - list.begin(), *i);
        }
    }

    vector(size_t num, const elemType& value) : 
        _allocator(num * 2), _size(num) {
        for (size_t i = 0; i < _size; ++i) {
            _allocator.construct(i, value);
        }
    }

    vector(const vector& x) : 
        _allocator(x._allocator.length()), _size(x._size) {
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
            _allocator.construct(i, *(rhs._allocator[i]));
        }
        _size = rhs._size;
        return *this;
    }

    vector& operator= (vector&& rhs) {
        _allocator.clean();
        _allocator.reallocate(rhs._allocator.length());
        for (size_t i = 0; i < rhs._size; ++i) {
            _allocator.construct(i, move(*(rhs._allocator[i])));
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
        if (_size * 2 < 10)
            _allocator.reallocate(10);
        else
            _allocator.reallocate(_size * 2);
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
        _allocator.construct(_size, x);
        ++_size;
    }

    void push_back(elemType&& x) {
        if (_allocator.length() == _size + 1) {
            _allocator.reallocate(_allocator.length() * 2);
        }
        _allocator.construct(_size, x);
        ++_size;
    }

    void pop_back() {
        --_size;
        _allocator.remove(_size);
    }

    iterator begin() noexcept {
        return iterator(&_allocator, 0);
    }

    const_iterator begin() const noexcept {
        return const_iterator(&_allocator, 0);
    }

    iterator end() noexcept {
        return iterator(&_allocator, _size);
    }

    const_iterator end() const noexcept {
        return const_iterator(&_allocator, _size);
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
        _allocator.construct(pos, x);
    }

    iterator insert(const_iterator position, elemType&& x) {
        if (_allocator.length() == _size + 1) {
            _allocator.reallocate(_allocator.length() * 2);
        }
        size_t pos = position._idx;
        for (size_t i = _size; i > pos; --i) {
            _allocator.construct(i, Move(*_allocator[i - 1]));
            _allocator.remove(i - 1);
        }
        _allocator.construct(pos, x);
        ++_size;
        return iterator(&_allocator, pos);
    }

    iterator insert(const_iterator position, size_t n, const elemType& x) {
        if (_allocator.length() == _size + n) {
            _allocator.reallocate(_allocator.length() * 2);
        }
        size_t pos = position._idx;
        for (size_t i = _size; i > pos; --i) {
            _allocator.construct(i + n - 1, Move(*_allocator[i - 1]));
            _allocator.remove(i - 1);
        }
        for (size_t i = 0; i < n; ++i)
            _allocator.construct(pos + i, x);
        _size += n;
        return iterator(&_allocator, pos);
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
        --_size;
        return iterator(&_allocator, pos);
    }

    iterator erase(const_iterator first, const_iterator last) {
        size_t pos1 = first._idx, pos2 = last._idx, i;
        for (i = pos1; i < pos2; ++i) {
            _allocator.remove(i);
        }
        while (pos2 < _size) {
            _allocator.construct(pos1, Move(*_allocator[pos2]));
            _allocator.remove(pos2);
            ++pos1, ++pos2;
        }
        _size -= pos2 - pos1;
        return iterator(&_allocator, first._idx);
    }

    void swap(vector& x) {
        Allocator allocator(x._allocator);
        _allocator.copy(x._allocator);
        x._allocator.copy(allocator);
        ::swap(this->_num, x._num);
    }

    void clear() noexcept {
        _allocator.clean();
        //_allocator.reallocate(10);
        _size = 0;
    }
};

#endif // STLITE_VECTOR_HPP