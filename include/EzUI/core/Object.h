#pragma once
#include "UIDef.h"
#include "String.h"
#include "SmartPtr.h"

namespace ezui {
	namespace detail {
		extern EZUI_API int AddObjectCount();
		extern EZUI_API int SubtractObjectCount();
		extern EZUI_API int GetObjectCount();
	}

	//常用对象基类
	class EZUI_API Object {
	private:
		//对象是否存活的标志(智能指针包装的原子布尔值)
		std::shared_ptr<atomic<bool>>* m_alive;
		//属性集合
		std::map<UIString, UIString>* m_attrs;
		//用户自定义数据
		std::map<int, void*>* m_userDatas;
		// 管理子对象的释放
		PtrManager<Object*> m_childObjects;
#ifdef DEBUG
		Object* m_ownerObject; // 拥有者对象（如果有）
#endif // DEBUG
	private:
		Object(const Object&);            // 禁止拷贝构造
		Object& operator=(const Object&); // 禁止拷贝赋值
		Object(Object&&);                 // 禁止移动构造
		Object& operator=(Object&&);      // 禁止移动赋值
	public:
		// 如果传入 ownerObject，则该对象的生命周期由 ownerObject 管理
		// ownerObject 析构时会自动销毁当前对象
		// 若为 NULL，则该对象具有独立生命周期
		explicit Object(Object* ownerObject = NULL);
		virtual ~Object();
	public:
		//获取对象弱指针(用于安全引用对象指针，防止野指针)
#ifdef EZUI_COMPILER_VS2010
		template<typename T>
#else
		template<typename T = Object>
#endif
		WeakPtr<T> GetWeakPtr() {
			WeakPtr<T> ptr((T*)this, GetAlive());
			return ptr;
		}

#ifdef EZUI_COMPILER_VS2010
		WeakPtr<Object> GetWeakPtr() {
			return this->GetWeakPtr<Object>();
		}
#endif
		//获取对象是否存活的共享指针(用于查看对象生命周期)
		std::shared_ptr<atomic<bool>> GetAlive();
		//设置用户自定义数据
		virtual void SetUserData(void* userData, int dataIndex = 0);
		//获取用户自定义数据
		virtual void* GetUserData(int dataIndex = 0)const;
		//删除用户数据
		virtual bool EraseUserData(int dataIndex);
		//设置属性
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue);
		//获取属性
		virtual const UIString& GetAttribute(const UIString& attrName)const;
		//获取全部属性
		virtual const std::map<UIString, UIString>& GetAttributes();
		//移除某个属性
		virtual void RemoveAttribute(const UIString& attrName);
		/// 取得对象所有权，使其跟随当前对象销毁；不改变 Control 的控件树关系。
		virtual Object* Attach(Object* obj);
		/// 解除对象所有权；不改变 Control 的控件树关系。
		virtual void Detach(Object* obj);
		//延迟删除
		void DeleteLater();
		//尝试转为指定类型
		template<typename T>
		T* As() {
			T* out = dynamic_cast<T*>(this);
			return out;
		}
	};
}
