#ifndef STLITE_VECTOR_HPP
#define STLITE_VECTOR_HPP

#ifdef VECTOR_INITIALIZER_LIST_ENABLED
#include <initializer_list>
#endif
#include "allocator.hpp"
#include "exceptions.hpp"

namespace s7a9 {
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
                if (lhs._palloc != rhs._palloc) throw sjtu::invalid_iterator();
                return (long long)lhs._idx - rhs._idx;
            }

            iterator& operator--() {
                if (_idx) --_idx;
                else throw sjtu::invalid_iterator();
                return *this;
            }

            iterator& operator+=(int offset) {
                _idx += offset;
                return *this;
            }

            iterator& operator-=(int offset) {
                _idx -= offset;
                return *this;
            }

            iterator operator--(int) {
                iterator iter(*this);
                --(*this);
                return iter;
            }

            bool operator==(const iterator& rhs) const {
                if (_palloc != rhs._palloc) throw sjtu::invalid_iterator();
                return _palloc == rhs._palloc && _idx == rhs._idx;
            }

            bool operator!=(const iterator& rhs) const {
                if (_palloc != rhs._palloc) throw sjtu::invalid_iterator();
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
                else throw sjtu::invalid_iterator();
                return *this;
            }

            const_iterator& operator+=(int offset) {
                _idx += offset;
                return *this;
            }

            const_iterator& operator-=(int offset) {
                _idx -= offset;
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
                if (lhs._palloc != rhs._palloc) throw sjtu::invalid_iterator();
                return (long long)lhs._idx - rhs._idx;
            }

            bool operator==(const const_iterator& rhs) const {
                if (_palloc != rhs._palloc) throw sjtu::invalid_iterator();
                return _palloc == rhs._palloc && _idx == rhs._idx;
            }

            bool operator!=(const const_iterator& rhs) const {
                if (_palloc != rhs._palloc) throw sjtu::invalid_iterator();
                return !(*this == rhs);
            }
        };

        vector() noexcept : _allocator(10), _size(0) {}

#ifdef VECTOR_INITIALIZER_LIST_ENABLED
        vector(std::initializer_list<elemType> list) :
            _allocator(list.size() * 2), _size(list.size()) {
            for (auto i = list.begin(); i != list.end(); ++i) {
                _allocator.construct(i - list.begin(), *i);
            }
        }
#endif

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
            _allocator(Move(x._allocator)), _size(x.size) {
            x._size = 0;
        }

        ~vector() {}

        vector& operator= (const vector& rhs) {
            if (this == &rhs) return *this;
            _allocator.clean();
            _allocator.reallocate(rhs._allocator.length());
            for (size_t i = 0; i < rhs._size; ++i) {
                _allocator.construct(i, rhs[i]);
            }
            _size = rhs._size;
            return *this;
        }

        vector& operator= (vector&& rhs) {
            if (this == &rhs) return *this;
            _allocator.clean();
            _allocator.reallocate(rhs._allocator.length());
            for (size_t i = 0; i < rhs._size; ++i) {
                _allocator.construct(i, Move(rhs[i]));
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
            if (idx >= _size) throw sjtu::index_out_of_bound();
            return *(_allocator[idx]);
        }

        const elemType& operator[](size_t idx) const {
            if (idx >= _size) throw sjtu::index_out_of_bound();
            return *(_allocator[idx]);
        }

        elemType& at(size_t idx) {
            if (idx >= _size) throw sjtu::index_out_of_bound();
            return *(_allocator[idx]);
        }

        const elemType& at(size_t idx) const {
            if (idx >= _size) throw sjtu::index_out_of_bound();
            return *(_allocator[idx]);
        }

        elemType& front() {
            if (_size == 0) throw sjtu::container_is_empty();
            return *(_allocator[0]);
        }

        const elemType& front() const {
            if (_size == 0) throw sjtu::container_is_empty();
            return *(_allocator[0]);
        }

        elemType& back() {
            if (_size == 0) throw sjtu::container_is_empty();
            return *(_allocator[_size - 1]);
        }

        const elemType& back() const {
            if (_size == 0) throw sjtu::container_is_empty();
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

        const_iterator cbegin() const noexcept {
            return const_iterator(&_allocator, 0);
        }

        iterator end() noexcept {
            return iterator(&_allocator, _size);
        }

        const_iterator end() const noexcept {
            return const_iterator(&_allocator, _size);
        }

        const_iterator cend() const noexcept {
            return const_iterator(&_allocator, _size);
        }

        iterator insert(const_iterator position, const elemType& x) {
            if (_allocator.length() == _size + 1) {
                _allocator.reallocate(_allocator.length() * 2);
            }
            size_t pos = position._idx;
            if (pos > _size) throw sjtu::index_out_of_bound();
            for (size_t i = _size; i > pos; --i) {
                _allocator.construct(i, Move(*_allocator[i - 1]));
                _allocator.remove(i - 1);
            }
            _allocator.construct(pos, x);
            ++_size;
            return iterator(&_allocator, pos);
        }

        iterator insert(size_t index, const elemType& x) {
            return insert(begin() + index, x);
        }

        iterator insert(size_t index, elemType&& x) {
            return insert(begin() + index, x);
        }

        iterator insert(const_iterator position, elemType&& x) {
            if (_allocator.length() == _size + 1) {
                _allocator.reallocate(_allocator.length() * 2);
            }
            size_t pos = position._idx;
            if (pos > _size) throw sjtu::index_out_of_bound();
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
            if (pos > _size) throw sjtu::index_out_of_bound();
            for (size_t i = _size; i > pos; --i) {
                _allocator.construct(i + n - 1, Move(*_allocator[i - 1]));
                _allocator.remove(i - 1);
            }
            for (size_t i = 0; i < n; ++i)
                _allocator.construct(pos + i, x);
            _size += n;
            return iterator(&_allocator, pos);
        }

        iterator erase(size_t idx) {
            return erase(begin() + idx);
        }

        iterator erase(const_iterator position) {
            size_t pos = position._idx;
            if (pos >= _size) {
                throw sjtu::index_out_of_bound();
            }
            _allocator.remove(pos);
            for (size_t i = pos; i < _size; ++i) {
                _allocator.construct(i, Move(*_allocator[i + 1]));
                _allocator.remove(i + 1);
            }
            --_size;
            if (_size < _allocator.length() / 2 && _allocator.length() > 10) {
                _allocator.reallocate(_allocator.length() / 2);
            }
            return iterator(&_allocator, pos);
        }

        iterator erase(const_iterator first, const_iterator last) {
            size_t pos1 = first._idx, pos2 = last._idx, i;
            if (pos1 >= _size || pos2 >= _size) {
                throw sjtu::index_out_of_bound();
            }
            for (i = pos1; i < pos2; ++i) {
                _allocator.remove(i);
            }
            while (pos2 < _size) {
                _allocator.construct(pos1, Move(*_allocator[pos2]));
                _allocator.remove(pos2);
                ++pos1, ++pos2;
            }
            _size -= pos2 - pos1;
            if (_size < _allocator.length() / 2 && _allocator.length() > 10) {
                _allocator.reallocate(_allocator.length() / 2);
            }
            return iterator(&_allocator, first._idx);
        }

        void swap(vector& x) {
            _allocator.swap(x._allocator);
            s7a9::swap(_size, x._size);
        }

        void clear() noexcept {
            _allocator.clean();
            _allocator.reallocate(10);
            _size = 0;
        }
    };
}

#endif // STLITE_VECTOR_HPP