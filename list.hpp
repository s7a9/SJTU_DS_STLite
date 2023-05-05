#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {

    template <typename T>
    class _listnode_t {
    public:
        T data;
        _listnode_t* prev, * next;

        explicit _listnode_t(const T& data) :
            data(data), prev(nullptr), next(nullptr) {}

        explicit _listnode_t(T&& data) :
            data(data), prev(nullptr), next(nullptr) {}
    };

    /**
     * a data container like std::list
     * allocate random memory addresses for data and they are doubly-linked in a list.
     */
    template<typename T, class node_t = _listnode_t<T>>
    class list {
    protected:
        /**
         * add data members for linked list as protected members
         */
        using node = node_t;

        node* _begin, * _end;

        size_t _size;

        /**
         * insert node cur before node pos
         * return the inserted node cur
         */
        node* _insert(node* pos, node* cur) noexcept {
            ++_size;
            if (pos == nullptr) { // insert at end of the list
                cur->prev = _end, cur->next = nullptr;
                if (_end) _end->next = cur;
                else _begin = cur;
                return _end = cur; // update _end
            }
            cur->next = pos, cur->prev = pos->prev;
            if (pos->prev) pos->prev->next = cur;
            else _begin = cur; // update _begin if inserted at front
            pos->prev = cur;
            return cur;
        }
        /**
         * remove node pos from list (no need to delete the node)
         * return the removed node pos
         */
        node* _erase(node* pos) noexcept {
            // if (pos == nullptr) throw sjtu::invalid_iterator();
            --_size;
            if (pos->next) pos->next->prev = pos->prev;
            else _end = pos->prev;
            if (pos->prev) pos->prev->next = pos->next;
            else _begin = pos->next;
            node* ret = pos->next;
            pos->next = pos->prev = nullptr;
            return ret;
        }

        static void _quick_sort(node** nds, size_t l, size_t r) {
            if (l + 1 >= r) return;
            if (l + 2 == r) {
                if (nds[l + 1]->data < nds[l]->data)
                    std::swap(nds[l], nds[l + 1]);
                return;
            }
            size_t pivot = rand() % (r - l) + l, i = l + 1, j = r - 1;
            std::swap(nds[l], nds[pivot]);
            while (true) {
                while (i < j && !(nds[l]->data < nds[i]->data)) ++i;
                while (i < j && !(nds[j]->data < nds[l]->data)) --j;
                if (i < j) {
                    std::swap(nds[i], nds[j]);
                }
                else break;
            }
            if (nds[i]->data < nds[l]->data)
                std::swap(nds[l], nds[i]);
            _quick_sort(nds, l, i), _quick_sort(nds, i, r);
        }

        void _clear() {
            if (_begin == nullptr || _end == nullptr) return;
            while (_begin != _end) {
                _begin = _begin->next;
                delete _begin->prev;
            }
            delete _end;
            _begin = _end = nullptr;
            _size = 0;
        }

    public:
        class const_iterator;
        class iterator {
        private:
            node* _p, ** _end; // The reference _end refers to the pointer _end from the list

            friend const_iterator;

            friend list;

        public:
            iterator() : _p(nullptr), _end(nullptr) {}

            iterator(node* p, node** end) : _p(p), _end(end) {}

            iterator(const iterator& other) :
                _p(other._p), _end(other._end) {}

            iterator& operator= (const iterator& rhs) {
                _p = rhs._p;
                _end = rhs._end;
                return *this;
            }
            /**
             * iter++
             */
            iterator operator++(int) {
                iterator ret(_p, _end);
                ++(*this);
                return ret;
            }
            /**
             * ++iter
             */
            iterator& operator++() {
                if (_p) {
                    _p = _p->next;
                }
                else throw sjtu::invalid_iterator();
                return *this;
            }
            /**
             * iter--
             */
            iterator operator--(int) {
                iterator ret(_p, _end);
                --(*this);
                return ret;
            }
            /**
             * --iter
             */
            iterator& operator--() {
                if (_p) {
                    _p = _p->prev;
                }
                else if (_end && *_end) _p = *_end;
                else throw sjtu::invalid_iterator();
                return *this;
            }
            /**
             * TODO *it
             * remember to throw if iterator is invalid
             */
            T& operator *() const {
                if (_p) return _p->data;
                throw sjtu::invalid_iterator();
            }
            /**
             * TODO it->field
             * remember to throw if iterator is invalid
             */
            T* operator ->() const {
                if (_p) return &_p->data;
                throw sjtu::invalid_iterator();
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator& rhs) const {
                return _p == rhs._p && _end == rhs._end;
            }
            bool operator==(const const_iterator& rhs) const {
                return _p == rhs._p && _end == rhs._end;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator& rhs) const {
                return _p != rhs._p || _end != rhs._end;
            }
            bool operator!=(const const_iterator& rhs) const {
                return _p != rhs._p || _end != rhs._end;
            }
        };
        /**
         * TODO
         * has same function as iterator, just for a const object.
         * should be able to construct from an iterator.
         */
        class const_iterator {
        private:
            const node* _p, * const* _end;

            friend iterator;

        public:
            const_iterator() : _p(nullptr), _end(nullptr) {}

            const_iterator(const const_iterator& iter) :
                _p(iter._p), _end(iter._end) {}

            const_iterator(const iterator& iter) :
                _p(iter._p), _end(iter._end) {}

            explicit const_iterator(const node* p, const node* const* end) :
                _p(p), _end(end) {}

            /**
             * iter++
             */
            const_iterator operator++(int) {
                const_iterator ret(_p, _end);
                ++(*this);
                return ret;
            }
            /**
             * ++iter
             */
            const_iterator& operator++() {
                if (_p) _p = _p->next;
                else throw sjtu::invalid_iterator();
                return *this;
            }
            /**
             * iter--
             */
            const_iterator operator--(int) {
                const_iterator ret(_p, _end);
                --(*this);
                return ret;
            }
            /**
             * --iter
             */
            const_iterator& operator--() {
                if (_p) {
                    _p = _p->prev;
                }
                else if (_end && *_end) _p = *_end;
                else throw sjtu::invalid_iterator();
                return *this;
            }
            /**
             * TODO *it
             * remember to throw if iterator is invalid
             */
            const T& operator *() const {
                if (_p) return _p->data;
                throw sjtu::invalid_iterator();
            }
            /**
             * TODO it->field
             * remember to throw if iterator is invalid
             */
            const T* operator ->() const {
                if (_p) return &_p->data;
                throw sjtu::invalid_iterator();
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator& rhs) const {
                return _p == rhs._p && _end == rhs._end;
            }
            bool operator==(const const_iterator& rhs) const {
                return _p == rhs._p && _end == rhs._end;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator& rhs) const {
                return _p != rhs._p || _end != rhs._end;
            }
            bool operator!=(const const_iterator& rhs) const {
                return _p != rhs._p || _end != rhs._end;
            }
        };
        /**
         * TODO Constructs
         * Atleast two: default constructor, copy constructor
         */
        list() : _begin(nullptr), _end(nullptr), _size(0) {}

        list(const list& other) {
            _begin = _end = nullptr;
            _size = 0;
            for (node* nd = other._begin; nd; nd = nd->next) {
                _insert(nullptr, new node(nd->data));
            }
        }

        list(list&& other) :
            _begin(other._begin), _end(other._end), _size(other._size) {
            other._begin = other._end = nullptr;
            other._size = 0;
        }
        /**
         * TODO Destructor
         */
        virtual ~list() {
            _clear();
        }
        /**
         * TODO Assignment operator
         */
        list& operator=(const list& other) {
            if (this == &other) return *this;
            _clear();
            for (node* nd = other._begin; nd; nd = nd->next) {
                _insert(nullptr, new node(nd->data));
            }
            return *this;
        }
        /**
         * access the first / last element
         * throw container_is_empty when the container is empty.
         */
        const T& front() const {
            if (_begin) return _begin->data;
            throw sjtu::container_is_empty();
        }
        const T& back() const {
            if (_end) return _end->data;
            throw sjtu::container_is_empty();
        }
        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            return iterator(_begin, _end);
        }

        const_iterator cbegin() const {
            return const_iterator(_begin, _end);
        }
        /**
         * returns an iterator to the end.
         */
        iterator end() {
            return iterator(nullptr, _end);
        }
        const_iterator cend() const {
            return const_iterator(nullptr, &_end);
        }
        /**
         * checks whether the container is empty.
         */
        virtual bool empty() const {
            return _size == 0;
        }
        /**
         * returns the number of elements
         */
        virtual size_t size() const {
            return _size;
        }

        /**
         * clears the contents
         */
        inline void clear() {
            _clear();
        }
        /**
         * insert value before pos (pos may be the end() iterator)
         * return an iterator pointing to the inserted value
         * throw if the iterator is invalid
         */
        virtual iterator insert(iterator pos, const T& value) {
            if (&_end != pos._end) throw invalid_iterator();
            return iterator(_insert(pos._p, new node(value)), &_end);
        }

        virtual iterator insert(iterator pos, T&& value) {
            if (&_end != pos._end) throw invalid_iterator();
            return iterator(_insert(pos._p, new node(value)), &_end);
        }
        /**
         * remove the element at pos (the end() iterator is invalid)
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid
         */
        virtual iterator erase(iterator pos) {
            if (empty()) throw sjtu::container_is_empty();
            if (pos._p == nullptr) throw sjtu::invalid_iterator();
            iterator ret(_erase(pos._p), &_end);
            delete pos._p;
            return ret;
        }
        /**
         * adds an element to the end
         */
        void push_back(const T& value) {
            _insert(nullptr, new node(value));
        }

        void push_back(T&& value) {
            _insert(nullptr, new node(value));
        }
        /**
         * removes the last element
         * throw when the container is empty.
         */
        void pop_back() {
            if (empty()) throw sjtu::container_is_empty();
            node* nd = _end;
            _erase(_end);
            delete nd;
        }
        /**
         * inserts an element to the beginning.
         */
        void push_front(const T& value) {
            _insert(_begin, new node(value));
        }

        void push_front(T&& value) {
            _insert(_begin, new node(value));
        }
        /**
         * removes the first element.
         * throw when the container is empty.
         */
        void pop_front() {
            if (empty()) throw sjtu::container_is_empty();
            node* nd = _begin;
            _erase(_begin);
            delete nd;
        }
        /**
         * sort the values in ascending order with operator< of T
         */
        void sort() {
            if (_size == 0 || _size == 1) return;
            size_t i = 0, new_size = 0;
            node** nds = new node * [_size + 1];
            for (node* nd = _begin; nd; nd = nd->next) {
                nds[new_size++] = nd;
            }
            _begin = _end = nullptr;
            _size = 0;
            _quick_sort(nds, 0, new_size);
            for (i = 0; i < new_size; ++i) _insert(nullptr, nds[i]);
            delete[] nds;
        }
        /**
         * merge two sorted lists into one (both in ascending order)
         * compare with operator< of T
         * container other becomes empty after the operation
         * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
         * the order of equivalent elements of *this and other does not change.
         * no elements are copied or moved
         */
        void merge(list& other) {
            node* nd1 = _begin, * nd2 = other._begin, * tmp_pnd;
            _begin = _end = other._begin = other._end = nullptr;
            _size = other._size = 0;
            while (nd1 && nd2) {
                if (nd2->data < nd1->data) {
                    tmp_pnd = nd2->next;
                    _insert(nullptr, nd2);
                    nd2 = tmp_pnd;
                }
                else {
                    tmp_pnd = nd1->next;
                    _insert(nullptr, nd1);
                    nd1 = tmp_pnd;
                }
            }
            while (nd1) {
                tmp_pnd = nd1->next;
                _insert(nullptr, nd1);
                nd1 = tmp_pnd;
            }
            while (nd2) {
                tmp_pnd = nd2->next;
                _insert(nullptr, nd2);
                nd2 = tmp_pnd;
            }
        }
        /**
         * reverse the order of the elements
         * no elements are copied or moved
         */
        void reverse() {
            node* cur_nd = _end, * nxt_nd = nullptr;
            _begin = _end = nullptr;
            _size = 0;
            while (cur_nd) {
                nxt_nd = cur_nd->prev;
                _insert(nullptr, cur_nd);
                cur_nd = nxt_nd;
            }
        }
        /**
         * remove all consecutive duplicate elements from the container
         * only the first element in each group of equal elements is left
         * use operator== of T to compare the elements.
         */
        void unique() {
            node* nd = _begin, * nxt_nd;
            while (nd && nd->next) {
                if (nd->data == nd->next->data) {
                    nxt_nd = nd->next;
                    _erase(nd->next);
                    delete nxt_nd;
                }
                else {
                    nd = nd->next;
                }
            }
        }

    protected:
        inline bool _check_iterator(const iterator& iter) const {
            return iter._end && *iter._end == _end;
        }
    };

}

#endif //SJTU_LIST_HPP
