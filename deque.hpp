#ifndef STLITE_DEQUE_HPP
#define STLITE_DEQUE_HPP

#include "allocator.hpp"
#include "exceptions.hpp"
#include <initializer_list>

namespace s7a9 {

	template <class elemType, class Allocator = __malloc_allocator<elemType>>
	class deque {
	private:
		struct allocator_linknode {
			Allocator* palloc;
			allocator_linknode* next, * prev;

			allocator_linknode() {
				palloc = nullptr;
				next = prev = nullptr;
			}
		};

		allocator_linknode* _front, * _end;

		size_t _front_idx, _end_idx, _size, _num_per_block;

		void _clean() {
			if (_front == nullptr || _end == nullptr) return;
			if (_front->prev) _front = _front->prev;
			while (_front != _end) {
				delete _front->palloc;
				_front = _front->next;
				delete _front->prev;
			}
			delete _end->palloc;
			delete _end;
			_front = _end = nullptr;
			_front_idx = _end_idx = _size = 0;
		}

		inline void _expand_front() {
			if (_front_idx == 0) {
				if (_front->prev == nullptr) {
					_front->prev = new allocator_linknode();
					_front->prev->palloc = new Allocator(_num_per_block);
					_front->prev->next = _front;
				}
				_front = _front->prev;
				_front_idx = _num_per_block;
			}
			--_front_idx, ++_size;
		}

		inline void _expand_back() {
			if (_end_idx == _end->palloc->length()) {
				if (_end->next == nullptr) {
					_end->next = new allocator_linknode();
					_end->next->palloc = new Allocator(_num_per_block);
					_end->next->prev = _end;
				}
				_end = _end->next;
				_end_idx = 0;
			}
			++_end_idx, ++_size;
		}

	public:
		deque() noexcept :
			deque(256) {}

		explicit deque(size_t num_per_block) noexcept :
			_num_per_block(num_per_block) {
			_front = _end = new allocator_linknode();
			_front->palloc = new Allocator(num_per_block);
			_front_idx = _end_idx = num_per_block / 2;
			_size = 0;
		}

		deque(std::initializer_list<elemType> init, 
			size_t num_per_block = 256) : 
			deque(num_per_block) {
			for (auto i = init.begin(); i != init.end(); ++i) {
				push_back(*i);
			}
		}

		deque(const deque& other) {
			allocator_linknode* cur_node, * lst_node = nullptr;
			_front = cur_node = new allocator_linknode();
			for (allocator_linknode* node = other._front; 
				; node = node->next) {
				cur_node->palloc = new Allocator(*node->palloc);
				cur_node->prev = lst_node;
				if (lst_node) lst_node->next = cur_node;
				if (node != other._end) {
					cur_node = new allocator_linknode();
					lst_node = cur_node;
				}
				else break;
			}
			_end = cur_node;
			_front_idx = other._front_idx, _end_idx = other._end_idx;
			_size = other._size, _num_per_block = other._num_per_block;
		}

		deque(deque&& other) noexcept {
			_front = other._front, _end = other._end;
			_front_idx = other._front_idx, _end_idx = other._end_idx;
			_size = other._size, _num_per_block = other._num_per_block;
			other._front = other._end = nullptr;
			other._size = other._front_idx = other._end_idx = 0;
			other._num_per_block = 0;
		}

		~deque() {
			_clean();
		}

		deque& operator=(const deque& other) {
			_clean();
			allocator_linknode* cur_node, * lst_node = nullptr;
			_front = cur_node = new allocator_linknode();
			for (allocator_linknode* node = other._front;
				; node = node->next) {
				cur_node->palloc = new Allocator(*node->palloc);
				cur_node->prev = lst_node;
				if (lst_node) lst_node->next = cur_node;
				if (node != other._end) {
					cur_node = new allocator_linknode();
					lst_node = cur_node;
				}
				else break;
			}
			_end = cur_node;
			_front_idx = other._front_idx, _end_idx = other._end_idx;
			_size = other._size, _num_per_block = other._num_per_block;
		}

		deque& operator=(deque&& other) {
			_clean();
			_front = other._front, _end = other._end;
			_front_idx = other._front_idx, _end_idx = other._end_idx;
			_size = other._size, _num_per_block = other._num_per_block;
			other._front = other._end = nullptr;
			other._size = other._front_idx = other._end_idx = 0;
			other._num_per_block = 0;
		}

		elemType& at(size_t idx) {
			size_t i = 0, i_blk = _front_idx;
			allocator_linknode* node = _front;
			while (i < idx) {
				++i_blk, ++i;
				if (i_blk == node->palloc->length()) {
					i_blk = 0;
					if (node == _end) {
						throw sjtu::index_out_of_bound();
					}
					node = node->next;
				}
			}
			return *(_front->palloc->data(i_blk));
		}

		elemType& operator[](size_t idx) noexcept {
			size_t i = 0, i_blk = _front_idx;
			allocator_linknode* node = _front;
			while (i < idx) {
				++i_blk, ++i;
				if (i_blk == node->palloc->length()) {
					i_blk = 0;
					node = node->next;
				}
			}
			return *(_front->palloc->data(i_blk));
		}

		elemType& front() {
			if (_size == 0) throw sjtu::container_is_empty();
			return *(_front->palloc->data(_front_idx));
		}

		elemType& back() {
			if (_size == 0) throw sjtu::container_is_empty();
			if (_end_idx == 0) {
				auto palloc = _end->prev->palloc;
				return *(palloc->data(palloc->length() - 1));
			}
			return *(_end->palloc->data(_end_idx - 1));
		}

		[[nodiscard]] inline bool empty() const {
			return _size == 0;
		}

		inline size_t size() const {
			return _size;
		}

		void clear() {
			_clean();
			_front = _end = new allocator_linknode();
			_front->palloc = new Allocator(_num_per_block);
			_front_idx = _end_idx = _num_per_block / 2;
		}

		void push_back(const elemType& x) {
			_expand_back();
			_end->palloc->construct(_end_idx - 1, x);
		}

		void push_back(elemType&& x) {
			_expand_back();
			_end->palloc->construct(_end_idx - 1, x);
		}

		void pop_back() {
			if (_size == 0) return;
			if (_end_idx == 0) {
				if (_end->next) {
					delete _end->next->palloc;
					delete _end->next;
					_end->next = nullptr;
				}
				_end = _end->prev;
				_end_idx = _end->palloc->length();
			}
			--_end_idx, --_size;
			_end->palloc->remove(_end_idx);
		}

		void push_front(const elemType& x) {
			_expand_front();
			_front->palloc->construct(_front_idx, x);
		}

		void push_front(elemType&& x) {
			_expand_front();
			_front->palloc->construct(_front_idx, x);
		}

		void pop_front() {
			_front->palloc->remove(_front_idx);
			++_front_idx, --_size;
			if (_front_idx == _front->palloc->length()) {
				if (_front->prev) {
					delete _front->prev->palloc;
					delete _front->prev;
					_front->prev = nullptr;
				}
				_front = _front->next;
				_front_idx = 0;
			}
		}

		void swap(deque& other) {
			s7a9::swap(_front, other._front);
			s7a9::swap(_end, other._end);
			s7a9::swap(_num_per_block, other._num_per_block);
			s7a9::swap(_front_idx, other._front_idx);
			s7a9::swap(_end_idx, other._end_idx);
			s7a9::swap(_size, other._size);
		}

	};

}

#endif // STLITE_DEQUE_HPP