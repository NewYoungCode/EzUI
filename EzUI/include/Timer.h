#pragma once
#include "EzUI.h"
#include "Task.h"
namespace EzUI {

	//ʹ���̵߳ļ�ʱ�� ����������ͬ��(������ʱ���ֱ�ӿ�ʼִ�лص�����)
	class UI_EXPORT ThreadTimer :public IControl {
		bool bExit = false;
		bool bStop = true;
		Task* task = NULL;
		std::mutex mtx;
		std::condition_variable condv;
	public:
		std::function<void(ThreadTimer*)> Tick = NULL;
		size_t Interval = -1;
	public:
		ThreadTimer();
		void Start();
		void Stop();
		virtual ~ThreadTimer();
	};

	//�ڲ���UI��ʱ����ʹ�ô�Timer�� �����߳�ͬ��(������ʱ���һ�λ����Ż�����)
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