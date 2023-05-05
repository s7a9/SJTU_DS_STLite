/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_LINKEDHASHMAP_HPP
#define SJTU_LINKEDHASHMAP_HPP

 // only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"
#include "list.hpp"

namespace sjtu {
	/**
	 * In linked_hashmap, iteration ordering is differ from map,
	 * which is the order in which keys were inserted into the map.
	 * You should maintain a doubly-linked list running through all
	 * of its entries to keep the correct iteration order.
	 *
	 * Note that insertion order is not affected if a key is re-inserted
	 * into the map.
	 */

	template<class T>
	class _hashnode_t {
	public:
		T data;
		_hashnode_t* prev, * next, * hash_next;

		explicit _hashnode_t(const T& data) :
			data(data), prev(nullptr), next(nullptr), hash_next(nullptr) {}

		explicit _hashnode_t(T&& data) :
			data(data), prev(nullptr), next(nullptr), hash_next(nullptr) {}
	};

	template<
		class Key,
		class T,
		class Hash = std::hash<Key>,
		class Equal = std::equal_to<Key>
	> class linked_hashmap : protected list<pair<Key, T>, _hashnode_t<pair<Key, T>>> {
	public:
		/**
		 * the internal type of data.
		 * it should have a default constructor, a copy constructor.
		 * You can use sjtu::linked_hashmap as value_type by typedef.
		 */
		using value_type = pair<const Key, T>;

	private:
		static const size_t INITAL_CAPACITY = 129, LOAD_FACTOR = 75;

		using list_t = list<pair<Key, T>, _hashnode_t<pair<Key, T>>>;

		using linknode_t = typename list_t::node;

		linknode_t** _table;

		size_t _table_size;

		Hash _hash;

		Equal _equal;

		// iterate through list and initalize hash table
		void _rehash_list() {
			std::fill(_table, _table + _table_size, nullptr);
			for (linknode_t* nd = this->_begin; nd; nd = nd->next) {
				size_t index = _hash(nd->data.first) % _table_size;
				nd->hash_next = _table[index], _table[index] = nd;
			}
		}

		// Get the reference of *hash_next*, which has the target key
		// Will get a nullptr if not found, dirctly put value in it
		linknode_t*& _find(const Key& key) const {
			size_t pos = _hash(key) % _table_size;
			if (_table[pos] == nullptr || _equal(_table[pos]->data.first, key))
				return _table[pos];
			linknode_t* node = _table[pos];
			while (node->hash_next && !_equal(node->hash_next->data.first, key))
				node = node->hash_next;
			return node->hash_next;
		}

		inline void _expand() {
			delete[] _table;
			_table_size = _table_size * 2 + 3;
			_table = new linknode_t * [_table_size];
			_rehash_list();
		}

	public:

		using iterator = typename list_t::iterator;

		using const_iterator = typename list_t::const_iterator;

		/**
		 * TODO two constructors
		 */
		linked_hashmap() : list_t::list(), _table_size(INITAL_CAPACITY) {
			_table = new linknode_t * [_table_size];
			std::fill(_table, _table + _table_size, nullptr);
		}

		linked_hashmap(const linked_hashmap& other) :
			list_t::list(other), _table_size(other._table_size) {
			_table = new linknode_t * [_table_size];
			_rehash_list();
		}

		/**
		 * TODO assignment operator
		 */
		linked_hashmap& operator=(const linked_hashmap& other) {
			if (this == &other) return *this;
			static_cast<list_t*>(this)->operator=(other);
			delete[] _table;
			_table_size = other._table_size;
			_table = new linknode_t * [_table_size];
			_rehash_list();
			return *this;
		}

		/**
		 * TODO Destructors
		 */
		~linked_hashmap() {
			delete[] _table;
		}

		/**
		 * TODO
		 * access specified element with bounds checking
		 * Returns a reference to the mapped value of the element with key equivalent to key.
		 * If no such element exists, an exception of type `index_out_of_bound'
		 */
		T& at(const Key& key) {
			linknode_t* nd = _find(key);
			if (nd) return nd->data.second;
			throw sjtu::index_out_of_bound();
		}

		const T& at(const Key& key) const {
			linknode_t* nd = _find(key);
			if (nd) return nd->data.second;
			throw sjtu::index_out_of_bound();
		}

		/**
		 * TODO
		 * access specified element
		 * Returns a reference to the value that is mapped to a key equivalent to key,
		 *   performing an insertion if such key does not already exist.
		 */
		inline T& operator[](const Key& key) {
			if (this->_size >= _table_size * LOAD_FACTOR / 100)
				_expand();
			linknode_t*& node = _find(key);
			if (node) return node->data.second;
			node = this->_insert(nullptr, new linknode_t(pair<Key, T>(key, T())));
			return node->data.second;
		}

		/**
		 * behave like at() throw index_out_of_bound if such key does not exist.
		 */
		inline const T& operator[](const Key& key) const {
			return at(key);
		}

		/**
		 * return a iterator to the beginning
		 */
		inline iterator begin() {
			return iterator(this->_begin, this->_end);
		}

		inline const_iterator cbegin() const {
			return const_iterator(this->_begin, this->_end);
		}

		/**
		 * return a iterator to the end
		 * in fact, it returns past-the-end.
		 */
		inline iterator end() {
			return iterator(nullptr, this->_end);
		}

		inline const_iterator cend() const {
			return const_iterator(nullptr, this->_end);
		}

		/**
		 * checks whether the container is empty
		 * return true if empty, otherwise false.
		 */
		inline bool empty() const {
			return this->_size == 0;
		}

		/**
		 * returns the number of elements.
		 */
		inline size_t size() const {
			return this->_size;
		}

		/**
		 * clears the contents
		 */
		void clear() {
			this->_clear();
			std::fill(_table, _table + _table_size, nullptr);
		}

		/**
		 * insert an element.
		 * return a pair, the first of the pair is
		 *   the iterator to the new element (or the element that prevented the insertion),
		 *   the second one is true if insert successfully, or false.
		 */
		inline pair<iterator, bool> insert(const value_type& value) {
			return insert(std::move(value_type(value)));
		}

		pair<iterator, bool> insert(value_type&& value) {
			if (this->_size >= _table_size * LOAD_FACTOR / 100)
				_expand();
			linknode_t*& pos = _find(value.first);
			if (pos && _equal(value.first, pos->data.first)) {
				return sjtu::pair<iterator, bool>(iterator(pos, this->_end), false);
			}
			pos = this->_insert(nullptr, new linknode_t(value));
			return sjtu::pair<iterator, bool>(iterator(pos, this->_end), true);
		}

		/**
		 * erase the element at pos.
		 *
		 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
		 */
		iterator erase(iterator pos) {
			if (!this->_check_iterator(pos) || pos == end()) {
				throw sjtu::invalid_iterator();
			}
			if (this->_size < _table_size / 4 && _table_size > INITAL_CAPACITY) {
				_table_size = (_table_size - 3) / 2;
				delete[] _table;
				_table = new linknode_t * [_table_size];
				_rehash_list();
			}
			linknode_t*& node = _find(pos->first), * tmp_node = node;
			if (node == nullptr) return end();
			iterator ret_iter(this->_erase(node), this->_end);
			node = node->hash_next;
			delete tmp_node;
			return ret_iter;
		}

		/**
		 * Returns the number of elements with key
		 *   that compares equivalent to the specified argument,
		 *   which is either 1 or 0
		 *     since this container does not allow duplicates.
		 */
		inline size_t count(const Key& key) const {
			return _find(key) ? 1 : 0;
		}

		/**
		 * Finds an element with key equivalent to key.
		 * key value of the element to search for.
		 * Iterator to an element with key equivalent to key.
		 *   If no such element is found, past-the-end (see end()) iterator is returned.
		 */
		inline iterator find(const Key& key) {
			return iterator(_find(key), this->_end);
		}

		inline const_iterator find(const Key& key) const {
			return const_iterator(_find(key), this->_end);
		}
	};

}

#endif
