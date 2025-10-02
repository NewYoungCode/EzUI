#pragma once
#include <unordered_map>

template<class T1, class T2>
class Map :protected std::unordered_map<T1, T2> {
	using __base = std::unordered_map<T1, T2>;
	T2 defaultValue = T2();//当获取不到value的时候返回的默认值
public:
	typename __base::iterator begin() {
		return __base::begin();
	}
	typename __base::iterator end() {
		return __base::end();
	}
	bool insert(const T1& key, const T2& value) {
		auto itor = __base::insert(std::pair<T1, T2>(key, value));
		return itor.second;
	}
	bool empty() {
		return __base::empty();
	}
	size_t size() {
		return __base::size();
	}
	const T2& operator[](const T1& key) const {
		auto itor = __base::find(key);
		if (itor != __base::end()) {
			return itor->second;
		}
		return defaultValue;
	}
	typename __base::iterator find(const T1& key) {
		return __base::find(key);
	}
	void erase(const typename __base::iterator& itor) {
		__base::erase(itor);
	}
	bool erase(const T1& key) {
		auto itor = __base::find(key);
		if (itor != __base::end()) {
			__base::erase(itor);
			return true;
		}
		return false;
	}
};
