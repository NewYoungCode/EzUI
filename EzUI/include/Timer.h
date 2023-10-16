#pragma once
#include "EzUI.h"
#include <TimeAPI.h>
namespace EzUI {

	//ʹ��window�ļ�ʱ��(���Ƚϸ�) ����������ͬ��
	class UI_EXPORT ThreadTimer :public IControl {
		MMRESULT timer = 0;
	public:
		std::function<void(ThreadTimer*)> Tick = NULL;
		size_t Interval = -1;
	public:
		ThreadTimer();
		void Start();
		void Stop();
		virtual ~ThreadTimer();
	};

	//�ڲ���UI��ʱ����ʹ�ô�Timer�� �����߳�ͬ��
	class UI_EXPORT Timer :public IControl {
	public:
		std::function<void(Timer*)> Tick = NULL;
		size_t Interval = -1;
		UINT_PTR TimerId = NULL;
	public:
		void Stop();
		void Start();
		Timer() {}
		virtual ~Timer();
	};
};