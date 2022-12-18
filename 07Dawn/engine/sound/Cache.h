#pragma once
#undef max
#include <deque>
#include <iterator>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <algorithm>


//https://codereview.stackexchange.com/questions/128560/caches-implementation-in-c
template <typename Key, typename Value>
class CacheFiFo {
public:
	using value_type = typename std::pair<Key, Value*>;
	using value_it = typename std::deque<value_type>::iterator;
	//using operation_guard = typename std::lock_guard<std::mutex>;

	CacheFiFo(size_t max_size) : max_cache_size{ max_size } {
		if (max_size == 0) {
			max_cache_size = std::numeric_limits<size_t>::max();
		}
	}

	void Put(const Key& key, const Value* value) {
		operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			if (cache_items_map.size() + 1 > max_cache_size) {
				// remove the last element from cache
				auto last = cache_items_deque.rbegin();
				delete last->second;
				cache_items_map.erase(last->first);
				cache_items_deque.pop_back();
			}

			cache_items_deque.push_front(std::make_pair(key, value));
			cache_items_map[key] = cache_items_deque.begin();
		}else {
			// just update value
			it->second->second = value;
		}
	}

	void Put(const Key& key) {
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			if (cache_items_map.size() + 1 > max_cache_size) {
				// remove the last element from cache
				auto last = cache_items_deque.rbegin();
				delete last->second;
				cache_items_map.erase(last->first);
				cache_items_deque.pop_back();
			}

			cache_items_deque.push_front(std::make_pair(key, new Value(key)));
			cache_items_map[key] = cache_items_deque.begin();
		}
	}

	const Value* Get(const Key& key) const {
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			throw std::range_error("No such key in the cache");
		}

		return it->second->second;
	}

	bool Exists(const Key& key) const noexcept {
		//operation_guard og{ safe_op };

		return cache_items_map.find(key) != cache_items_map.end();
	}

	size_t Size() const noexcept {
		//operation_guard og{ safe_op };

		return cache_items_map.size();
	}

private:
	std::deque<value_type> cache_items_deque;
	std::unordered_map<Key, value_it> cache_items_map;
	size_t max_cache_size;
	//mutable std::mutex safe_op;
};

template <typename Key, typename Value>
class CacheLRUP {
public:
	using value_type = typename std::pair<Key, Value*>;
	using value_it = typename std::list<value_type>::iterator;
	//using operation_guard = typename std::lock_guard<std::mutex>;

	CacheLRUP(size_t max_size) : max_cache_size{ max_size } {
		if (max_size == 0) {
			max_cache_size = std::numeric_limits<size_t>::max();
		}
	}

	void Put(const Key& key, const Value* value) {
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			if (cache_items_map.size() + 1 > max_cache_size) {
				// remove the last element from cache
				auto last = cache_items_list.crbegin();
				delete last->second;
				cache_items_map.erase(last->first);
				cache_items_list.pop_back();
			}

			cache_items_list.push_front(std::make_pair(key, value));
			cache_items_map[key] = cache_items_list.begin();
		}
		else {
			it->second->second = value;
			cache_items_list.splice(cache_items_list.cbegin(), cache_items_list, it->second);
		}
	}

	void Put(const Key& key) {
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			if (cache_items_map.size() + 1 > max_cache_size) {
				// remove the last element from cache
				auto last = cache_items_list.crbegin();
				delete last->second;
				cache_items_map.erase(last->first);
				cache_items_list.pop_back();
			}

			cache_items_list.push_front(std::make_pair(key, new Value(key)));
			cache_items_map[key] = cache_items_list.begin();

		}
		else {
			//it->second->second = value;
			cache_items_list.splice(cache_items_list.cbegin(), cache_items_list, it->second);
		}
	}

	const Value* Get(const Key& key) const {
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			throw std::range_error("No such key in the cache");
		}
		else {
			cache_items_list.splice(cache_items_list.begin(), cache_items_list,
				it->second);

			return it->second->second;
		}
	}

	bool Exists(const Key& key) const noexcept {
		//operation_guard og{ safe_op };
		return cache_items_map.find(key) != cache_items_map.end();
	}

	size_t Size() const noexcept {
		//operation_guard og{ safe_op };
		return cache_items_map.size();
	}

private:
	mutable std::list<value_type> cache_items_list;
	std::unordered_map<Key, value_it> cache_items_map;
	size_t max_cache_size;
	//mutable std::mutex safe_op;
};

template <typename Key, typename Value>
class CacheLRU {
public:
	using value_type = typename std::pair<Key, Value>;
	using value_it = typename std::list<value_type>::iterator;
	//using operation_guard = typename std::lock_guard<std::mutex>;

	CacheLRU(size_t max_size) : max_cache_size{ max_size } {
		if (max_size == 0) {
			max_cache_size = std::numeric_limits<size_t>::max();
		}
	}

	void Put(const Key& key, const Value& value) {
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			if (cache_items_map.size() + 1 > max_cache_size) {
				// remove the last element from cache
				auto last = cache_items_list.crbegin();
				delete last->second;
				cache_items_map.erase(last->first);
				cache_items_list.pop_back();
			}

			cache_items_list.push_front(std::make_pair(key, value));
			cache_items_map[key] = cache_items_list.begin();
		}
		else {
			it->second->second = value;
			cache_items_list.splice(cache_items_list.cbegin(), cache_items_list, it->second);
		}
	}

	void Put(const Key& key) {
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			if (cache_items_map.size() + 1 > max_cache_size) {
				// remove the last element from cache
				auto last = cache_items_list.crbegin();

				cache_items_map.erase(last->first);
				cache_items_list.pop_back();
			}

			cache_items_list.push_front(std::make_pair(key, Value(key, std::distance(cache_items_map.begin(), it))));
			cache_items_map[key] = cache_items_list.begin();

		} else {
			//it->second->second = value;
			cache_items_list.splice(cache_items_list.cbegin(), cache_items_list, it->second);
		}
	}

	const Value& Get(const Key& key) const {
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			throw std::range_error("No such key in the cache");
		}
		else {
			cache_items_list.splice(cache_items_list.begin(), cache_items_list,
				it->second);

			return it->second->second;
		}
	}

	bool Exists(const Key& key) const noexcept {
		//operation_guard og{ safe_op };
		return cache_items_map.find(key) != cache_items_map.end();
	}

	size_t Size() const noexcept {
		//operation_guard og{ safe_op };

		std::cout << "Size1: " << cache_items_map.size() << std::endl;

		return cache_items_map.size();
	}

private:
	mutable std::list<value_type> cache_items_list;
	std::unordered_map<Key, value_it> cache_items_map;
	size_t max_cache_size;
	//mutable std::mutex safe_op;
};

template <typename Key, typename Value>
class CacheLFU {
public:
	using freq_type = unsigned;
	using value_type = typename std::tuple<Key, Value*, freq_type>;
	using value_it = typename std::list<value_type>::iterator;
	//using operation_guard = typename std::lock_guard<std::mutex>;

	enum VTFields { key_f = 0, value_f = 1, frequency_f = 2 };

	CacheLFU(size_t max_size) : max_cache_size{ max_size } {
		if (max_size == 0) {
			max_cache_size = std::numeric_limits<size_t>::max();
		}
	}

	void Put(const Key& key, const Value* value) {
		constexpr unsigned INIT_FREQ = 1;
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			if (cache_items_map.size() + 1 > max_cache_size) {
				// look for the element with the smallest frequency value
				auto least_fr =
					std::min_element(cache_items_list.cbegin(), cache_items_list.cend(),
						[](const value_type& a, const value_type& b) {
					return std::get<frequency_f>(a) <
						std::get<frequency_f>(b);
				});
				delete least_fr._Ptr;
				cache_items_map.erase(std::get<key_f>(*least_fr));
				cache_items_list.erase(least_fr);
			}

			cache_items_list.emplace_front(std::make_tuple(key, value, INIT_FREQ));
			cache_items_map[key] = cache_items_list.begin();
		}else {
			// increase frequency of the existing value "key" and assigne new value
			
			++(std::get<frequency_f>(*it->second));
		}
	}

	void Put(const Key& key) {
		constexpr unsigned INIT_FREQ = 1;
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			if (cache_items_map.size() + 1 > max_cache_size) {
				// look for the element with the smallest frequency value
				auto least_fr =
					std::min_element(cache_items_list.cbegin(), cache_items_list.cend(),
						[](const value_type& a, const value_type& b) {
					return std::get<frequency_f>(a) <
						std::get<frequency_f>(b);
				});
				delete least_fr._Ptr;
				cache_items_map.erase(std::get<key_f>(*least_fr));
				cache_items_list.erase(least_fr);
			}

			cache_items_list.emplace_front(std::make_tuple(key, new Value(key), INIT_FREQ));
			cache_items_map[key] = cache_items_list.begin();
		}else {
			// increase frequency of the existing value "key" and assigne new value
			++(std::get<frequency_f>(*it->second));
		}
	}

	const Value* Get(const Key& key) const {
		//operation_guard og{ safe_op };
		auto it = cache_items_map.find(key);

		if (it == cache_items_map.end()) {
			throw std::range_error("No such key in the cache");
		}
		else {
			// increment the frequency of the "key"-element
			++(std::get<frequency_f>(*it->second));

			return std::get<value_f>(*it->second);
		}
	}

	bool Exists(const Key& key) const noexcept {
		//operation_guard og{ safe_op };
		return cache_items_map.find(key) != cache_items_map.end();
	}

	size_t Size() const noexcept {
		//operation_guard og{ safe_op };
		return cache_items_map.size();
	}

private:
	mutable std::list<value_type> cache_items_list;
	std::unordered_map<Key, value_it> cache_items_map;
	size_t max_cache_size;
	mutable std::mutex safe_op;
};