#pragma once
/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

 // only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

	template<
		class Key,
		class T,
		class Compare = std::less<Key>
	> class map {
	public:
		/**
		 * the internal type of data.
		 * it should have a default constructor, a copy constructor.
		 * You can use sjtu::map as value_type by typedef.
		 */
		typedef pair<const Key, T> value_type;

	private:
		struct Node {
			static const bool RED = true, BLK = false;

			bool _clr;

			value_type _val;

			Node* _fa, * _son[2];

			Node(value_type&& val, 
				bool color, 
				Node* fa = nullptr, 
				Node* ls = nullptr, 
				Node* rs = nullptr) :
				_val(val), _clr(color), _fa(fa) {
				_son[0] = ls; _son[1] = rs;
			}

			Node(const value_type& val,
				bool color,
				Node* fa = nullptr,
				Node* ls = nullptr,
				Node* rs = nullptr) :
				_val(val), _clr(color), _fa(fa) {
				_son[0] = ls; _son[1] = rs;
			}

			Node* succ() {
				Node* nd = _son[1];
				while (nd->_son[0])
					nd = nd->_son[0];
				return nd;
			}

			Node* prev() {
				Node* nd = this;
				if (_son[0]) {
					nd = nd->_son[0];
					while (nd->_son[1])
						nd = nd->_son[1];
				}
				else {
					while (nd->_fa && nd->_fa->_son[0] == nd)
						nd = nd->_fa;
					nd = nd->_fa;
				}
				return nd;
			}

			const Node* prev() const {
				const Node* nd = this;
				if (_son[0]) {
					nd = nd->_son[0];
					while (nd->_son[1])
						nd = nd->_son[1];
				}
				else {
					while (nd->_fa && nd->_fa->_son[0] == nd)
						nd = nd->_fa;
					nd = nd->_fa;
				}
				return nd;
			}

			Node* next() {
				Node* nd = this;
				if (_son[1]) {
					nd = nd->_son[1];
					while (nd->_son[0])
						nd = nd->_son[0];
				}
				else {
					while (nd->_fa && nd->_fa->_son[1] == nd)
						nd = nd->_fa;
					nd = nd->_fa;
				}
				return nd;
			}

			const Node* next() const {
				const Node* nd = this;
				if (_son[1]) {
					nd = nd->_son[1];
					while (nd->_son[0])
						nd = nd->_son[0];
				}
				else {
					while (nd->_fa && nd->_fa->_son[1] == nd)
						nd = nd->_fa;
					nd = nd->_fa;
				}
				return nd;
			}

			void rotate() {
				if (_fa == nullptr) return;
				Node* fa = _fa, * gf = fa->_fa;
				bool ws = fa->_son[1] == this;
				fa->_son[ws] = _son[ws ^ 1];
				if (_son[ws ^ 1]) _son[ws ^ 1]->_fa = fa;
				_son[ws ^ 1] = fa;
				fa->_fa = this;
				_fa = gf;
				if (gf)
					gf->_son[gf->_son[1] == fa] = this;
			}
		};

		Node* _root;

		size_t _size;

		Compare _comp;

		static Node* _copy_recursive(const Node* nd, Node* fa) {
			if (nd == nullptr) return nullptr;
			Node* ret = new Node(nd->_val, nd->_clr, fa);
			ret->_son[0] = _copy_recursive(nd->_son[0], ret);
			ret->_son[1] = _copy_recursive(nd->_son[1], ret);
			return ret;
		}

		static void _clear_recursive(Node* nd) {
			if (nd == nullptr) return;
			_clear_recursive(nd->_son[0]);
			_clear_recursive(nd->_son[1]);
			delete nd;
		}

		// If found, return true and pos is set to corresponding node
		// If not found, return false and pos is set to father node
		static bool _locate(const Key& key, Node*& pos) {
			Compare _comp;
			while (pos) {
				if (_comp(key, pos->_val.first)) {
					if (pos->_son[0]) pos = pos->_son[0];
					else return false;
				}
				else if (_comp(pos->_val.first, key)) {
					if (pos->_son[1]) pos = pos->_son[1];
					else return false;
				}
				else return true;
			}
			return false;
		}

		// give father node *pos* and insert one pair
		inline Node* _insert(Node* pos, Node* nd) {
			++_size;
			if (pos) {
				pos->_son[_comp(pos->_val.first, nd->_val.first)] = nd;
				_solve_double_red(nd);
				return nd;
			}
			else { // The tree is empty
				nd->_clr = Node::BLK;
				return _root = nd;
			}
		}

		void _erase(Node* pos) {
			if (pos == nullptr) return;
			--_size;
			Node* succ;
			while (pos->_son[0] || pos->_son[1]) {
				if (pos->_son[0] == nullptr)
					succ = pos->_son[1];
				else if (pos->_son[1] == nullptr)
					succ = pos->_son[0];
				else succ = pos->succ();
				// Swap the connection of node and its successor
				// Need special judge if they are father and son
				bool pos_ws = pos->_fa ? pos->_fa->_son[1] == pos : false,
					 succ_ws = succ->_fa ? succ->_fa->_son[1] == succ : false;
				std::swap(pos->_clr, succ->_clr);
				if (pos->_fa)
					pos->_fa->_son[pos_ws] = succ;
				else _root = succ;
				if (succ->_son[0])
					succ->_son[0]->_fa = pos;
				if (succ->_son[1])
					succ->_son[1]->_fa = pos;
				if (pos->_son[succ_ws ^ 1])
					pos->_son[succ_ws ^ 1]->_fa = succ;
				std::swap(pos->_son[succ_ws ^ 1],
					succ->_son[succ_ws ^ 1]);
				if (succ->_fa == pos) {
					pos->_son[succ_ws] = succ->_son[succ_ws];
					succ->_son[succ_ws] = pos;
					succ->_fa = pos->_fa;
					pos->_fa = succ;
				}
				else {
					if (pos->_son[succ_ws])
						pos->_son[succ_ws]->_fa = succ;
					succ->_fa->_son[succ_ws] = pos;
					std::swap(pos->_fa, succ->_fa);
					std::swap(pos->_son[succ_ws], succ->_son[succ_ws]);
				}
			}
			if (pos->_clr == Node::BLK) 
				_solve_double_black(pos);
			if (pos == _root) {
				_root = nullptr;
				delete pos;
				return;
			}
			pos->_fa->_son[pos->_fa->_son[1] == pos] = nullptr;
			delete pos;
			return;
		}

		inline void _solve_double_red(Node* pos) {
			while (pos->_fa == nullptr || pos->_fa->_clr == Node::RED) {
				if (pos == _root) {
					_root->_clr = Node::BLK;
					return;
				}
				Node* fa = pos->_fa;
				if (fa->_clr == Node::BLK) return;
				Node* gf = fa->_fa, * unc = nullptr;
				if (gf) unc = gf->_son[gf->_son[0] == fa];
				if (unc && unc->_clr == Node::RED) {
					gf->_clr = Node::RED;
					fa->_clr = Node::BLK;
					unc->_clr = Node::BLK;
					pos = gf;
				}
				else {
					if ((fa->_son[1] == pos) == (gf->_son[1] == fa)) {
						// same direction gf -> fa -> node
						fa->rotate();
						fa->_clr = Node::BLK;
						gf->_clr = Node::RED;
						if (gf == _root) _root = fa;
					}
					else {
						pos->rotate(); pos->rotate();
						pos->_clr = Node::BLK;
						gf->_clr = Node::RED;
						if (gf == _root) _root = pos;
					}
					return;
				}
			}
		}

		inline void _solve_double_black(Node* pos) {
			while (pos != _root) {
				Node* fa = pos->_fa,
					* bro = fa->_son[fa->_son[0] == pos];
				if (bro->_clr == Node::RED) {
					bro->_clr = Node::BLK;
					fa->_clr = Node::RED;
					if (_root == fa) _root = bro;
					bro->rotate();
					fa = pos->_fa; 
					bro = fa->_son[fa->_son[0] == pos];
				}
				if ((!bro->_son[0] || bro->_son[0]->_clr == Node::BLK)
					&& (!bro->_son[1] || bro->_son[1]->_clr == Node::BLK)) {
					if (fa->_clr == Node::RED) {
						fa->_clr = Node::BLK;
						bro->_clr = Node::RED;
						return;
					}
					else {
						bro->_clr = Node::RED;
						pos = fa;
					}
				}
				else {
					bool wc = bro->_son[1] && bro->_son[1]->_clr == Node::RED;
					Node* nep = bro->_son[wc];
					if (wc == (bro == fa->_son[1])) {
						if (_root == fa) _root = bro;
						bro->rotate();
						bro->_clr = fa->_clr;
						fa->_clr = nep->_clr = Node::BLK;
					}
					else {
						if (_root == fa) _root = nep;
						nep->rotate(); nep->rotate();
						nep->_clr = fa->_clr;
						fa->_clr = Node::BLK;
					}
					return;
				}
			}
		}

	public:
		/**
		 * see BidirectionalIterator at CppReference for help.
		 *
		 * if there is anything wrong throw invalid_iterator.
		 *     like it = map.begin(); --it;
		 *       or it = map.end(); ++end();
		 */
		class const_iterator;
		class iterator {
		private:
			/**
			 * TODO add data members
			 *   just add whatever you want.
			 */
			friend const_iterator;
			
			friend sjtu::map<Key, T, Compare>;

			// Used to mark the difference between 
			// *begin* and *end* iterator of an empty map
			bool _end_pos; 

			Node* _ptr, ** _root;

			iterator(Node* ptr, Node** root, bool _end_pos = false) noexcept :
				_ptr(ptr), _root(root), _end_pos(_end_pos) {}

		public:
			iterator() noexcept :
				_ptr(nullptr), _root(nullptr) {}

			iterator(const iterator& other) noexcept :
				_ptr(other._ptr), _root(other._root) {}

			/**
			 * TODO iter++
			 */
			iterator operator++(int) {
				iterator iter(*this);
				if (_ptr == nullptr) {
					_root = nullptr;
					throw sjtu::invalid_iterator();
				}
				_ptr = _ptr->next();
				if (_ptr == nullptr) _end_pos = true;
				return iter;
			}
			/**
			 * TODO ++iter
			 */
			iterator& operator++() {
				if (_ptr == nullptr) {
					_root = nullptr;
					throw sjtu::invalid_iterator();
				}
				_ptr = _ptr->next();
				if (_ptr == nullptr) _end_pos = true;
				return *this;
			}
			/**
			 * TODO iter--
			 */
			iterator operator--(int) {
				iterator iter(*this);
				if (_ptr == nullptr) {
					if (!_end_pos) 
						throw sjtu::invalid_iterator();
					_end_pos = false;
					_ptr = *_root;
					while (_ptr->_son[1])
						_ptr = _ptr->_son[1];
				}
				else {
					_ptr = _ptr->prev();
					if (_ptr == nullptr) {
						_root = nullptr;
						throw sjtu::invalid_iterator();
					}
				}
				return iter;
			}
			/**
			 * TODO --iter
			 */
			iterator& operator--() {
				if (_ptr == nullptr) {
					if (!_end_pos)
						throw sjtu::invalid_iterator();
					_end_pos = false;
					_ptr = *_root;
					while (_ptr->_son[1])
						_ptr = _ptr->_son[1];
				}
				else {
					_ptr = _ptr->prev();
					if (_ptr == nullptr) {
						_root = nullptr;
						throw sjtu::invalid_iterator();
					}
				}
				return *this;
			}

			value_type& operator*() const {
				if (_ptr == nullptr) throw sjtu::invalid_iterator();
				return _ptr->_val;
			}

			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			bool operator==(const iterator& rhs) const {
				return _ptr == rhs._ptr && _root == rhs._root;
			}

			bool operator==(const const_iterator& rhs) const {
				return _ptr == rhs._ptr && _root == rhs._root;
			}

			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator& rhs) const {
				return _ptr != rhs._ptr || _root != rhs._root;
			}

			bool operator!=(const const_iterator& rhs) const {
				return _ptr != rhs._ptr || _root != rhs._root;
			}

			/**
			 * for the support of it->first.
			 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
			 */
			value_type* operator->() const noexcept {
				return &_ptr->_val;
			}
		};

		class const_iterator {
			// it should has similar member method as iterator.
			//  and it should be able to construct from an iterator.
		private:
			friend iterator;

			friend map<Key, T, Compare>;

			// Same function as in iterator
			bool _end_pos;

			const Node* _ptr, *const* _root;

			const_iterator(const Node* ptr, 
				const Node*const* root, 
				bool end_pos = false) noexcept :
				_ptr(ptr), _root(root), _end_pos(end_pos)  {}

		public:
			const_iterator() noexcept :
				_ptr(nullptr), _root(nullptr) {}

			const_iterator(const const_iterator& other) noexcept :
				_ptr(other._ptr), _root(other._root) {}

			const_iterator(const iterator& other) noexcept :
				_ptr(other._ptr), _root(other._root) {}

			/**
			 * TODO iter++
			 */
			const_iterator operator++(int) {
				const_iterator iter(*this);
				if (_ptr == nullptr) throw sjtu::invalid_iterator();
				_ptr = _ptr->next();
				if (_ptr == nullptr) _end_pos = true;
				return iter;
			}
			/**
			 * TODO ++iter
			 */
			const_iterator& operator++() {
				if (_ptr == nullptr) {
					_root = nullptr;
					throw sjtu::invalid_iterator();
				}
				_ptr = _ptr->next();
				if (_ptr == nullptr) _end_pos = true;
				return *this;
			}
			/**
			 * TODO iter--
			 */
			const_iterator operator--(int) {
				const_iterator iter(*this);
				if (_ptr == nullptr) {
					if (!_end_pos)
						throw sjtu::invalid_iterator();
					_end_pos = false;
					_ptr = *_root;
					while (_ptr->_son[1])
						_ptr = _ptr->_son[1];
				}
				else {
					_ptr = _ptr->prev();
					if (_ptr == nullptr) {
						_root = nullptr;
						throw sjtu::invalid_iterator();
					}
				}
				return iter;
			}
			/**
			 * TODO --iter
			 */
			const_iterator& operator--() {
				if (_ptr == nullptr) {
					if (!_end_pos)
						throw sjtu::invalid_iterator();
					_end_pos = false;
					_ptr = *_root;
					while (_ptr->_son[1])
						_ptr = _ptr->_son[1];
				}
				else {
					_ptr = _ptr->prev();
					if (_ptr == nullptr) {
						_root = nullptr;
						throw sjtu::invalid_iterator();
					}
				}
				return *this;
			}

			const value_type& operator*() const {
				if (_ptr == nullptr) throw sjtu::invalid_iterator();
				return _ptr->_val;
			}

			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			bool operator==(const iterator& rhs) const {
				return _ptr == rhs._ptr && _root == rhs._root;
			}

			bool operator==(const const_iterator& rhs) const {
				return _ptr == rhs._ptr && _root == rhs._root;
			}

			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator& rhs) const {
				return _ptr != rhs._ptr || _root != rhs._root;
			}

			bool operator!=(const const_iterator& rhs) const {
				return _ptr != rhs._ptr || _root != rhs._root;
			}

			const value_type* operator->() const noexcept {
				return &(_ptr->_val);
			}
		};
		/**
		 * TODO two constructors
		 */
		map() noexcept :
			_root(nullptr), _size(0) {}

		map(const map& other) :
			_size(other._size) {
			_root = _copy_recursive(other._root, nullptr);
		}

		map(map&& other) noexcept :
			_size(other._size), _root(other._root) {}

		/**
		 * TODO assignment operator
		 */
		map& operator=(const map& other) {
			if (this == &other) return *this;
			_clear_recursive(_root);
			_size = other._size;
			_root = _copy_recursive(other._root, nullptr);
			return *this;
		}
		/**
		 * TODO Destructors
		 */
		~map() {
			_clear_recursive(_root);
		}

		/**
		 * TODO
		 * access specified element with bounds checking
		 * Returns a reference to the mapped value of the element with key equivalent to key.
		 * If no such element exists, an exception of type `index_out_of_bound'
		 */
		T& at(const Key& key) {
			Node* nd = _root;
			if (_locate(key, nd)) return nd->_val.second;
			throw sjtu::index_out_of_bound();
		}

		const T& at(const Key& key) const {
			Node* nd = _root;
			if (_locate(key, nd)) return nd->_val.second;
			throw sjtu::index_out_of_bound();
		}

		/**
		 * TODO
		 * access specified element
		 * Returns a reference to the value that is mapped to a key equivalent to key,
		 *   performing an insertion if such key does not already exist.
		 */
		T& operator[](const Key& key) {
			Node* nd = _root;
			if (_locate(key, nd)) return nd->_val.second;
			return _insert(nd, new Node(value_type(key, T()), Node::RED, nd))->_val.second;
		}

		/**
		 * behave like at() throw index_out_of_bound if such key does not exist.
		 */
		const T& operator[](const Key& key) const {
			return at(key);
		}

		/**
		 * return a iterator to the beginning
		 */
		iterator begin() {
			Node* nd = _root;
			if (nd) {
				while (nd->_son[0])
					nd = nd->_son[0];
				return iterator(nd, &_root);
			}
			return iterator(nullptr, &_root);
		}

		const_iterator cbegin() const {
			const Node* nd = _root;
			if (nd) {
				while (nd->_son[0])
					nd = nd->_son[0];
				return const_iterator(nd, &_root);
			}
			return const_iterator(nullptr, &_root);
		}

		/**
		 * return a iterator to the end
		 * in fact, it returns past-the-end.
		 */
		iterator end() {
			return iterator(nullptr, &_root, true);
		}

		const_iterator cend() const {
			return const_iterator(nullptr, &_root, true);
		}
		/**
		 * checks whether the container is empty
		 * return true if empty, otherwise false.
		 */
		bool empty() const {
			return _size == 0;
		}
		/**
		 * returns the number of elements.
		 */
		size_t size() const {
			return _size;
		}
		/**
		 * clears the contents
		 */
		void clear() {
			_clear_recursive(_root);
			_root = nullptr;
			_size = 0;
		}
		/**
		 * insert an element.
		 * return a pair, the first of the pair is
		 *   the iterator to the new element (or the element that prevented the insertion),
		 *   the second one is true if insert successfully, or false.
		 */
		pair<iterator, bool> insert(const value_type& value) {
			Node* nd = _root;
			if (_locate(value.first, nd)) {
				return pair<iterator, bool>(
					iterator(nd, &_root),
					false
				);
			}
			return pair<iterator, bool>(
				iterator(_insert(nd, new Node(value, Node::RED, nd)), &_root),
				true
			);
		}

		pair<iterator, bool> insert(value_type&& value) {
			Node* nd = _root;
			if (_locate(value.first, nd)) {
				return pair<iterator, bool>(
					iterator(nd, &_root),
					false
				);
			}
			return pair<iterator, bool>(
				iterator(_insert(nd, new Node(value, Node::RED, nd)), &_root),
				true
			);
		}
		
		/**
		 * erase the element at pos.
		 *
		 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
		 */
		void erase(iterator pos) {
			if (pos._ptr == nullptr ||
				pos._root != &_root) throw sjtu::invalid_iterator();
			_erase(pos._ptr);
		}

		/**
		 * Returns the number of elements with key
		 *   that compares equivalent to the specified argument,
		 *   which is either 1 or 0
		 *     since this container does not allow duplicates.
		 * The default method of check the equivalence is !(a < b || b < a)
		 */
		size_t count(const Key& key) const {
			Node* nd = _root;
			return _locate(key, nd) ? 1 : 0;
		}

		/**
		 * Finds an element with key equivalent to key.
		 * key value of the element to search for.
		 * Iterator to an element with key equivalent to key.
		 *   If no such element is found, past-the-end (see end()) iterator is returned.
		 */
		iterator find(const Key& key) {
			Node* nd = _root;
			if (_locate(key, nd))
				return iterator(nd, &_root);
			return end();
		}

		const_iterator find(const Key& key) const {
			Node* nd = _root;
			if (_locate(key, nd))
				return const_iterator(nd, &_root);
			return cend();
		}
	};

}

#endif