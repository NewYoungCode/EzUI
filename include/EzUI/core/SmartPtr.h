#pragma once	
#include "UIDef.h"
#ifdef EZUI_COMPILER_VS2010
#include "platform/window/atomic.h"
#else
#include <atomic>
#endif

namespace ezui {
#ifndef EZUI_COMPILER_VS2010
	using std::atomic;
#endif

	//指针管理
	template <typename T>
	class PtrManager EZUI_FINAL {
	private:
		std::vector<T>* m_ptrs;
	private:
		PtrManager(const PtrManager&);
		PtrManager& operator=(const PtrManager&);
	public:
		PtrManager() : m_ptrs(NULL) {}
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
	class SharedPtr EZUI_FINAL {
	private:
		T* m_ptr;
		size_t* m_refCount;
		void Release() {
			if (m_refCount && --(*m_refCount) == 0) {
				delete m_ptr;
				delete m_refCount;
			}
			m_ptr = (T*)(NULL);
			m_refCount = NULL;
		}
	public:
		SharedPtr(T* ptr = (T*)(NULL)) : m_ptr(ptr), m_refCount(ptr ? new size_t(1) : NULL) {}
		SharedPtr(const SharedPtr& other) : m_ptr(other.m_ptr), m_refCount(other.m_refCount) {
			if (m_refCount) {
				++(*m_refCount);
			}
		}
#ifndef EZUI_COMPILER_VS2010
		SharedPtr(SharedPtr&& other) EZUI_NOEXCEPT : m_ptr(other.m_ptr), m_refCount(other.m_refCount) {
			other.m_ptr = (T*)(NULL);
			other.m_refCount = NULL;
		}
#endif // !EZUI_COMPILER_VS2010
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
#ifndef EZUI_COMPILER_VS2010
		SharedPtr& operator=(SharedPtr&& other) EZUI_NOEXCEPT {
			if (this != &other) {
				Release();
				m_ptr = other.m_ptr;
				m_refCount = other.m_refCount;
				other.m_ptr = (T*)(NULL);
				other.m_refCount = NULL;
			}
			return *this;
		}
#endif // !EZUI_COMPILER_VS2010
		T& operator*() const { return *m_ptr; }
		T* operator->() const { return m_ptr; }
		operator bool() const { return m_ptr != (T*)(NULL); }
		T* Get() const { return m_ptr; }
		size_t GetRefCount() const { return m_refCount ? *m_refCount : 0; }
	};

	//弱指针代理（用于安全引用 Object 指针，防止野指针）
	template<typename T>
	class WeakPtr {
	private:
		T* m_obj;
		std::shared_ptr<atomic<bool>> m_alive;
	public:
		WeakPtr() : m_obj((T*)(NULL)) {
		}
		WeakPtr(T* obj, std::shared_ptr<atomic<bool>> alive) : m_obj(obj), m_alive(alive) {
		}
		T* operator->() const {
			EZUI_ASSERT(IsAlive(), L"WeakPtr target is no longer alive");
			return m_obj;
		}
		T* Get() {
			EZUI_ASSERT(IsAlive(), L"WeakPtr target is no longer alive");
			return m_obj;
		}
		operator T* () const {
			return IsAlive() ? m_obj : NULL;
		}
		bool IsAlive() const {
			return (m_obj != (T*)(NULL) && m_alive && m_alive->load() == true/* && dynamic_cast<T*>(m_obj)*/);
		}
		T& operator*() const {
			EZUI_ASSERT(IsAlive(), L"WeakPtr target is no longer alive");
			return *m_obj;
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
