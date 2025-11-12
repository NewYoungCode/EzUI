#pragma once
#include "Window.h"
#include "Resource.h"

namespace ezui {
	//app类所有代码必须在Application实例化完成之后才可以创建窗口/控件...
	class UI_EXPORT Application
	{
	private:
		Application(const Application&) = delete;            // 禁止拷贝构造
		Application& operator=(const Application&) = delete; // 禁止拷贝赋值
		Application(Application&&) = delete;                 // 禁止移动构造
		Application& operator=(Application&&) = delete;      // 禁止移动赋值
	public:
		//退出消息循环
		static void Exit(int exitCode = 0);
		//获取程序启动路径
		static UIString StartPath();
	public:
		Application(HINSTANCE hInstance = NULL);
		//使用本地文件名称或者资源中的名称加载资源包
		//填入vs中的资源ID名称 或者 本地文件名 一个Application只允许有一个资源文件
		bool SetResource(const UIString& localOrResName);
		//启用高DPI适配
		void EnableHighDpi();
		virtual ~Application();
		//执行消息循环
		int Exec();
	};
};