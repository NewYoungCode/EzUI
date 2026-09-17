#pragma once	
#include "UIDef.h"
namespace ezui {
	// 属性值包装类
	// 用于表示一个可选启用状态的属性值。
	// 当 enabled=false 时，表示该属性未参与计算/使用默认值。
	template<typename T>
	class PropertyValue {
		bool m_enable;
		T m_value;
	public:
		PropertyValue() : m_enable(false), m_value() {}
		PropertyValue(const T& value) : m_enable(true), m_value(value) {}
		PropertyValue& operator=(const T& value) {
			m_value = value;
			m_enable = true;
			return *this;
		}
		PropertyValue(const PropertyValue& rightValue)
			: m_enable(rightValue.m_enable), m_value(rightValue.m_value) {
		}
		PropertyValue& operator=(const PropertyValue& rightValue) {
			if (this != &rightValue) {
				m_value = rightValue.m_value;
				m_enable = rightValue.m_enable;
			}
			return *this;
		}
		void Set(const T& value) { m_value = value; }
		void SetEnabled(bool bEnable) { m_enable = bEnable; }
		bool IsEnabled() const { return m_enable; }
		operator T() const { return m_enable ? m_value : T(); }
		T& operator->() { return m_value; }
		T Get() { return m_value; }
	};
};
