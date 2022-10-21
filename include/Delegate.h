#pragma once
#include "UIDef.h"
#include "IType.h"
namespace EzUI {
	//�о�c#��ί�кܺ��� ���Գ�Ϯһ��
	template<class out, class...in>
	class Delegate {
		using TFunc = std::function<out(in...)>;
		std::map<size_t, TFunc> callbacks;
	public:
		auto operator+=(const TFunc& callback) {
			auto pair = std::pair<size_t, TFunc>(size_t(&callback), callback);
			callbacks.emplace(pair);
			return size_t(&callback);
		}
		void operator-=(size_t eventid) {
			callbacks.erase(eventid);
		}
		void operator()(in... args) {
			for (auto& it : callbacks) {
				it.second(std::forward<in>(args)...);
			}
		}
	};
};