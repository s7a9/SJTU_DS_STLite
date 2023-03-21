#ifndef STLITE_QUEUE_HPP
#define STLITE_QUEUE_HPP

#include <functional>
#include "exceptions.hpp"
#include "vector.hpp"

namespace s7a9 {

	template <typename T, class Compare = std::less<T>>
	class priority_queue {
	private:
		struct node_t {
			T* pval;
			size_t dis;
			node_t* ls, * rs;

			node_t(T* pval, size_t dis) :
				pval(pval), dis(dis) {
				ls = rs = nullptr;
			}

			inline void del_subtree() {
				if (ls) ls->del_subtree();
				if (rs) rs->del_subtree();
				delete ls;
				delete rs;
				delete pval;
			}
		};

		node_t* _root;

		size_t _size;

		//static Compare _compare;

		static node_t* _merge(node_t* a, node_t* b) {
			static Compare comp;
			if (a == nullptr) return b;
			if (b == nullptr) return a;
			if (comp(*a->pval, *b->pval)) s7a9::swap(a, b);
			a->rs = _merge(a->rs, b);
			if (a->ls == nullptr || a->ls->dis < a->rs->dis) {
				s7a9::swap(a->ls, a->rs);
			}
			if (a->ls == nullptr || a->rs == nullptr) {
				a->dis = 0;
			}
			else {
				a->dis = s7a9::val_min(a->ls->dis, a->rs->dis);
			}
			return a;
		}

		inline node_t* _copy_subtree(const node_t* node) {
			node_t* nd = new node_t(new T(*node->pval), node->dis);
			if (node->ls) nd->ls = _copy_subtree(node->ls);
			if (node->rs) nd->rs = _copy_subtree(node->rs);
			return nd;
		}

		inline void _clean() {
			_root->del_subtree();
			delete _root;
			_size = 0;
		}

	public:
		priority_queue() noexcept {
			_size = 0;
			_root = nullptr;
		}

		priority_queue(const priority_queue& other):
			_size(other._size) {
			_root = _copy_subtree(other._root);
		}

		priority_queue(priority_queue&& other) noexcept :
			_size(other._size), _root(other._root)  {
			other._size = 0, other._root = nullptr;
		}

		~priority_queue() {
			if (_root) _clean();
		}

		priority_queue& operator=(const priority_queue& other) {
			if (this == &other) return *this;
			_clean();
			_root = _copy_subtree(other._root);
			_size = other._size;
			return *this;
		}

		const T& top() const {
			if (empty()) throw sjtu::container_is_empty();
			return *(_root->pval);
		}

		T& top() {
			if (empty()) throw sjtu::container_is_empty();
			return *(_root->pval);
		}

		void push(const T& e) {
			node_t* pn = new node_t(new T(e), 0);
			try {
				_root = _merge(_root, pn);
			}
			catch (...) {
				delete pn->pval;
				delete pn;
				return;
			}
			++_size;
		}

		void pop() {
			node_t* old_root = _root;
			_root = _merge(_root->ls, _root->rs);
			delete old_root->pval;
			delete old_root;
			--_size;
		}

		size_t size() const noexcept {
			return _size;
		}

		[[nodiscard]] bool empty() const noexcept {
			return _size == 0;
		}

		void merge(const priority_queue& other) {
			_root = _merge(_root, _copy_subtree(other._root));
			_size += other._size;
		}

		void merge(priority_queue&& other) {
			_root, _merge(_root, other._root);
			_size += other._size;
			other._root = nullptr;
			other._size = 0;
		}
	};

}

#endif // STLITE_QUEUE_HPP