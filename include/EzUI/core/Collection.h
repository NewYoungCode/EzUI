#pragma once	
#include "UIDef.h"
namespace ezui {
	//通用集合类 允许重复元素
	template<typename T>
	class Collection EZUI_FINAL : public std::vector<T> {
	public:
		 Collection() { }
		~Collection() { }
		//返回集合中的第一个元素
		T First()const {
			return this->empty() ? T() : this->front();
		}
		//返回集合中的最后一个元素
		T Last()const {
			return this->empty() ? T() : this->back();
		}
		//移除元素
		bool Remove(T item) {
			auto& vec = *this; // 继承自 vector
			auto new_end = std::remove(vec.begin(), vec.end(), item); // 将所有等于 item 的元素移动到尾部
			bool removed = (new_end != vec.end());
			vec.erase(new_end, vec.end()); // 删除尾部元素
			return removed;
		}
		//检查元素是否存在
		bool Contains(T item)const {
			auto itor = std::find(this->begin(), this->end(), item);
			return itor != this->end();
		}
		//查找索引
		int IndexOf(T item)const
		{
			int pos = 0;
			for (auto itor = this->begin(); itor != this->end(); ++itor)
			{
				if ((*itor) == item) {
					return pos;
				}
				++pos;
			}
			return -1;
		}
		//添加元素
		void Add(T item) {
			this->push_back(item);
		}
		//插入元素
		void Insert(int pos, T item) {
			size_t i = 0;
			auto itor = this->begin();
			for (; itor != this->end(); ++itor) {
				if (i == pos) {
					break;
				}
				++i;
			}
			if (itor == this->end()) {
				this->push_back(item);
			}
			else {
				this->insert(itor, item);
			}
		}
		//判断两个集合是否相等 大小相同 内容相同 顺序相同
		bool operator==(const Collection<T>& right) const {
			if (this->size() != right.size()) {
				return false;
			}
			auto itor1 = this->begin();
			auto itor2 = right.begin();
			for (; itor1 != this->end() && itor2 != right.end(); ++itor1, ++itor2) {
				if (!(*itor1 == *itor2)) {
					return false;
				}
			}
			return true;
		}
		bool operator!=(const Collection<T>& right) const {
			return !(*this == right);
		}
	};
};