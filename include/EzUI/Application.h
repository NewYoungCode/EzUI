#pragma once
#include "Window.h"
#include "Resource.h"

namespace ezui {
	class UI_EXPORT Application
	{
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