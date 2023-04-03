#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstdlib>
#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

	/**
	 * a container like std::priority_queue which is a heap internal.
	 */
	template<typename T, class Compare>
	class __binary_heap {
	private:
		struct node_t {
			T val;

			size_t degree;

			node_t* son, * fa, * nxt;

			node_t(const T& val) : val(val) {
				degree = 0;
				son = fa = nxt = nullptr;
			}

			node_t(T&& val) : val(val) {
				degree = 0;
				son = fa = nxt = nullptr;
			}
		};

		node_t* _head, * _ptop;

		size_t _size;

		static node_t* _merge(node_t* head1, node_t* head2) {
			static Compare _comp;
			node_t* head = nullptr, * cur = nullptr, * fa = nullptr, * son = nullptr, * lst = nullptr;
			// merge two sorted linktables to one
			bool select;
			while (head1 && head2) {
				if (head1->degree < head2->degree) {
					if (head == nullptr) head = head1;
					else cur->nxt = head1;
					cur = head1, head1 = head1->nxt;
				}
				else {
					if (head == nullptr) head = head2;
					else cur->nxt = head2;
					cur = head2, head2 = head2->nxt;
				}
			}
			while (head1) {
				if (head == nullptr) head = head1;
				else cur->nxt = head1;
				cur = head1, head1 = head1->nxt;
			}
			while (head2) {
				if (head == nullptr) head = head2;
				else cur->nxt = head2;
				cur = head2, head2 = head2->nxt;
			}
			// find nodes with same degree and merge the last two
			if (head) {
				cur = head;
				while (cur->nxt) {
					if (cur->degree == cur->nxt->degree) {
						if (cur->nxt->nxt && cur->degree == cur->nxt->nxt->degree)
							lst = cur, cur = cur->nxt;
						if (_comp(cur->nxt->val, cur->val)) {
							fa = cur, son = cur->nxt;
							fa->nxt = son->nxt;
						}
						else {
							fa = cur->nxt, son = cur;
							if (lst) lst->nxt = fa;
							else head = fa;
						}
						++fa->degree;
						son->nxt = fa->son, fa->son = son, son->fa = fa;
						cur = fa; // set *cur* to *fa*, or *cur* may go to subtree and program crashes!
					}
					else lst = cur, cur = cur->nxt;
				}
			}
			return head;
		}

		static node_t* _copy_recursive(const node_t* node, node_t* fa) {
			if (node == nullptr) return nullptr;
			node_t* ret = new node_t(node->val);
			ret->fa = fa, ret->degree = node->degree;
			if (node->son) ret->son = _copy_recursive(node->son, ret);
			if (node->nxt) ret->nxt = _copy_recursive(node->nxt, fa);
			return ret;
		}

		static void _clear_recursive(node_t* node) {
			if (node == nullptr) return;
			if (node->son) _clear_recursive(node->son);
			if (node->nxt) _clear_recursive(node->nxt);
			delete node;
		}

		inline void _update_top() {
			static Compare _comp;
			_ptop = _head;
			if (_head) {
				node_t* cur = _head->nxt;
				while (cur) {
					if (_comp(_ptop->val, cur->val))
						_ptop = cur;
					cur = cur->nxt;
				}
			}
		}

	public:
		/**
		 * TODO constructors
		 */
		inline __binary_heap(): _size(0) {
			_head = _ptop = nullptr;
		}

		__binary_heap(const __binary_heap& other):
			_size(other._size) {
			_head = _copy_recursive(other._head, nullptr);
			_update_top();
		}
		/**
		 * TODO deconstructor
		 */
		inline ~__binary_heap() {
			_clear_recursive(_head);
		}
		/**
		 * TODO Assignment operator
		 */
		__binary_heap& operator=(const __binary_heap& other) {
			if (this == &other) return *this;
			_clear_recursive(_head);
			_size = other._size;
			_head = _copy_recursive(other._head, nullptr);
			_update_top();
			return *this;
		}
		/**
		 * get the top of the queue.
		 * @return a reference of the top element.
		 * throw container_is_empty if empty() returns true;
		 */
		inline const T& top() const {
			if (empty()) throw sjtu::container_is_empty();
			return _ptop->val;
		}
		/**
		 * TODO
		 * push new element to the priority queue.
		 */
		inline void push(const T& e) {
			_head = _merge(_head, new node_t(e));
			_update_top();
			++_size;
		}

		inline void push(T&& e) {
			_head = _merge(_head, new node_t(e));
			_update_top();
			++_size;
		}
		/**
		 * TODO
		 * delete the top element.
		 * throw container_is_empty if empty() returns true;
		 */
		void pop() {
			if (empty()) throw sjtu::container_is_empty();
			node_t* son_cur, * son_lst = nullptr, * son_nxt;
			if (_head == _ptop) _head = _ptop->nxt;
			else {
				for (node_t* cur = _head; cur->nxt; cur = cur->nxt) {
					if (cur->nxt == _ptop) {
						cur->nxt = cur->nxt->nxt;
						break;
					}
				}
			}
			for (son_cur = _ptop->son; son_cur; son_cur = son_nxt) {
				son_cur->fa = nullptr;
				son_nxt = son_cur->nxt;
				son_cur->nxt = son_lst;
				son_lst = son_cur;
			}
			_head = _merge(_head, son_lst);
			delete _ptop;
			_update_top();
			--_size;
		}
		/**
		 * return the number of the elements.
		 */
		inline size_t size() const {
			return _size;
		}
		/**
		 * check if the container has at least an element.
		 * @return true if it is empty, false if it has at least an element.
		 */
		inline bool empty() const {
			return _head == nullptr;
		}
		/**
		 * merge two priority_queues with at least O(logn) complexity.
		 * clear the other priority_queue.
		 */
		void merge(__binary_heap& other) {
			_head = _merge(_head, other._head);
			_update_top();
			other._head = other._ptop = nullptr;
			other._size = 0;
		}
	};


	template<typename T, class Compare = std::less<T>>
	using priority_queue = __binary_heap<T, Compare>;

}

#endif