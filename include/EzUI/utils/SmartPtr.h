#pragma once	
#include "core/UIDef.h"
namespace ezui {
	//指针管理
	template <typename T>
	class PtrManager final {
	private:
		std::vector<T>* m_ptrs = NULL;
	private:
		PtrManager(const PtrManager&) = delete;
		PtrManager& operator=(const PtrManager&) = delete;
	public:
		PtrManager() {}
		~PtrManager() {
			this->FreeAll();
			if (m_ptrs) {
				delete m_ptrs;
			}
		}
		const std::vector<T>* GetItems() {
			return m_ptrs;
		}
		void Add(const T& v) {
			if (v) {
				if (!m_ptrs) {
					m_ptrs = new std::vector<T>;
				}
				auto itor = std::find(m_ptrs->begin(), m_ptrs->end(), v);
				if (itor == m_ptrs->end()) {
					m_ptrs->push_back(v);
				}
			}
		}
		bool Contains(const T& v)const {
			if (m_ptrs) {
				auto itor = std::find(m_ptrs->begin(), m_ptrs->end(), v);
				return itor != m_ptrs->end();
			}
			return false;
		}
		void Remove(const T& v) {
			if (m_ptrs) {
				auto it = std::find(m_ptrs->begin(), m_ptrs->end(), v);
				if (it != m_ptrs->end()) {
					m_ptrs->erase(it);
				}
			}
		}
		void FreeAll() {
			if (m_ptrs) {
				auto itor = m_ptrs->begin();
				while (itor != m_ptrs->end())
				{
					T item = *itor;
					itor = m_ptrs->erase(itor); // erase 返回下一个有效迭代器
					delete item;
				}
			}
		}
	};

	//简单的共享智能指针实现 单线程使用
	template<typename T>
	class SharedPtr final {
	private:
		T* m_ptr;
		size_t* m_refCount;
		void Release() {
			if (m_refCount && --(*m_refCount) == 0) {
				delete m_ptr;
				delete m_refCount;
			}
			m_ptr = nullptr;
			m_refCount = nullptr;
		}
	public:
		SharedPtr(T* ptr = nullptr) : m_ptr(ptr), m_refCount(ptr ? new size_t(1) : nullptr) {}
		SharedPtr(const SharedPtr& other) : m_ptr(other.m_ptr), m_refCount(other.m_refCount) {
			if (m_refCount) {
				++(*m_refCount);
			}
		}
		SharedPtr(SharedPtr&& other) noexcept : m_ptr(other.m_ptr), m_refCount(other.m_refCount) {
			other.m_ptr = nullptr;
			other.m_refCount = nullptr;
		}
		~SharedPtr() {
			Release();
		}
		SharedPtr& operator=(const SharedPtr& other) {
			if (this != &other) {
				if (other.m_refCount) ++(*other.m_refCount);  // 先增
				Release();                                      // 再减
				m_ptr = other.m_ptr;
				m_refCount = other.m_refCount;
			}
			return *this;
		}
		SharedPtr& operator=(SharedPtr&& other) noexcept {
			if (this != &other) {
				Release();
				m_ptr = other.m_ptr;
				m_refCount = other.m_refCount;
				other.m_ptr = nullptr;
				other.m_refCount = nullptr;
			}
			return *this;
		}
		T& operator*() const { return *m_ptr; }
		T* operator->() const { return m_ptr; }
		explicit operator bool() const { return m_ptr != nullptr; }
		T* Get() const { return m_ptr; }
		size_t GetRefCount() const { return m_refCount ? *m_refCount : 0; }
	};

	//弱指针代理（用于安全引用 Object 指针，防止野指针）
	template<typename T>
	class WeakPtr {
	private:
		T* m_obj;
		std::shared_ptr<std::atomic<bool>> m_alive;
	public:
		WeakPtr() : m_obj(nullptr), m_alive(nullptr) {
		}
		WeakPtr(T* obj, std::shared_ptr<std::atomic<bool>> alive) : m_obj(obj), m_alive(alive) {
		}
		T* operator->() const {
			EZUI_ASSERT(IsAlive());
			return m_obj;
		}
		T* Get() {
			EZUI_ASSERT(IsAlive());
			return m_obj;
		}
		operator T* () const {
			return m_obj;
		}
		bool IsAlive() const {
			return (m_obj != nullptr && m_alive && m_alive->load() == true/* && dynamic_cast<T*>(m_obj)*/);
		}
		T& operator*() const {
			EZUI_ASSERT(IsAlive());
			return *m_obj;
		}
		explicit operator bool() const {
			return IsAlive();
		}
		bool operator==(T* right)const {
			return (m_obj == right);
		}
		bool operator==(const WeakPtr& right)const {
			return (m_obj == right.m_obj);
		}
		bool operator!=(T* right)const {
			return !(*this == right);
		}
	};
};