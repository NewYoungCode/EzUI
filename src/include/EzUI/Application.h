#pragma once
#include "Window.h"
#include "Resource.h"
namespace EzUI {
	class UI_EXPORT Application
	{
		void Init();
	public:
		static void Exit(int exitCode = 0) {
			::PostQuitMessage(exitCode);
		}
	public:
		Application();
		//ʹ��vs�е���ԴID������Դ�� resID��ԴID custResType�Զ�����Դ�������� ����
		Application(int resID, const EString& custResType);
		//ʹ�ñ����ļ����Ƽ�����Դ��
		Application(const EString& fileName);
		//���ø�DPI����
		void EnableHighDpi();
		virtual ~Application();
		int Exec();
	};
};