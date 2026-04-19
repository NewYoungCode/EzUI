#pragma once	
#include "core/UIDef.h"
namespace ezui {
	// 属性值包装类
	// 用于表示一个可选启用状态的属性值。
	// 当 enabled=false 时，表示该属性未参与计算/使用默认值。
	template<typename T>
	class PropertyValue {
		bool m_enable = false;
		T m_value{};
	public:
		PropertyValue() = default;
		PropertyValue(const T& value) : m_value(value), m_enable(true) {}
		PropertyValue& operator=(const T& value) {
			m_value = value;
			m_enable = true;
			return *this;
		}
		PropertyValue(const PropertyValue& rightValue)
			: m_value(rightValue.m_value), m_enable(rightValue.m_enable) {
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
		explicit operator bool() const { return IsEnabled(); }
		operator T() const { return m_enable ? m_value : T{}; }
		T& operator->() { return m_value; }
		T Get() { return m_value; }
	};
};