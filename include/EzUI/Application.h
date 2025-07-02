#pragma once
#include "Window.h"
#include "Resource.h"
namespace EzUI {
	class UI_EXPORT Application
	{
		void Init();
	public:
		//退出消息循环
		static void Exit(int_t exitCode = 0);
	public:
		Application();
		//使用本地文件名称或者资源中的名称加载资源包
		//填入vs中的资源ID名称 或者 本地文件名 一个Application只允许有一个资源文件
		bool SetResource(const UIString& localOrResName);
		//启用高DPI适配
		void EnableHighDpi();
		virtual ~Application();
		//执行消息循环
		int_t Exec();
	};
};