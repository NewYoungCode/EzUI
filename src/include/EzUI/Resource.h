#pragma once
#include "EString.h"
namespace EzUI {
	/// <summary>
	/// ��Դ��
	/// </summary>
	class UI_EXPORT Resource {
	public:
		struct Entry {
			size_t offset = 0;
			size_t size = 0;
			EString name;
		};
		//��Դ�ļ���ȡ��
		class UI_EXPORT ReadStream {
			size_t pos = 0;
			size_t count = 0;
			const char* ptr = NULL;
			std::ifstream* ifs = NULL;
		public:
			ReadStream(HRSRC hRsrc);
			ReadStream(const EString& fileName);
			void seekg(size_t pos);
			void read(char* buf, size_t count);
			size_t tellg();
			const size_t& size();
			virtual ~ReadStream();
		};
	private:
		std::list<Entry> items;
		ReadStream* rStream = NULL;
		void UnPackage();
	public:
		//����ԴĿ¼���д��
		UI_EXPORT static void Package(const EString& dir, const EString& outFile);
	public:
		virtual ~Resource();
		//�ӱ����ļ���������
		Resource(const EString& resFile);
		//ʹ��vs������Դ�ļ���������
		Resource(HRSRC hRsrc);
		//Ѱ����Դ�е��ļ�
		bool GetFile(const EString& fileName, std::string* out);
	};
};