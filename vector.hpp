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
    class iterator {};

    class const_iterator {};

    vector() noexcept(noexcept(Allocator())) : _allocator(10), _size(0) {}

    vector(size_t num, const elemType& value) : _allocator(num * 2), _size(num) {
        for (size_t i = 0; i < _size; ++i) {
            new(_allocator[i]) elemType(value);
        }
    }

    vector(const vector& x) : _allocator(x._allocator.length()), _size(x._size) {
        for (size_t i = 0; i < _size; ++i) {
            new(_allocator[i]) elemType(x[i]);
        }
    }

    vector(vector&& x) noexcept :
        _allocator(move(x._allocator)), _size(x.size) {}

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
        _allocator.reallocate(_size);
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

    const elemType& at(size_t) const {
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
        
    }

    void push_back(elemType&& x) {

    }

    void pop_back() {

    }

    iterator begin() noexcept {

    }

    const_iterator begin() const noexcept {

    }

    iterator end() noexcept {

    }

    const_iterator end() const noexcept {

    }

    iterator insert(const_iterator position, const elemType& x) {

    }

    iterator insert(const_iterator position, elemType&& x) {

    }

    iterator insert(const_iterator position, size_t n, const elemType& x) {

    }

    iterator erase(const_iterator position) {

    }

    iterator erase(const_iterator first, const_iterator last) {

    }

    void swap(vector& rhs) {

    }

    void clear() noexcept {

    }
};

#endif // STLITE_VECTOR_HPP