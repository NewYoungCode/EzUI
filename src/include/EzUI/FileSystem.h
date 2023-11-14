#pragma once
#include "EString.h"
namespace EzUI {
	struct FileInfo final {
		EString Name;
		DWORD Attr;
		bool IsDirectory() {
			return (Attr & FILE_ATTRIBUTE_DIRECTORY);
		}
	};
	namespace File {
		//�ж��ļ��Ƿ����
		UI_EXPORT bool Exists(const EString& filenNme);
		//�����ļ�
		UI_EXPORT bool Copy(const EString& src, const EString& desc);
		//ɾ���ļ�
		UI_EXPORT bool Delete(const EString& file);
		//�ļ��ƶ����߸���
		UI_EXPORT bool Move(const EString& oldName, const EString& newName);
		//����һ���ļ�(����ļ��Ѵ��������������)
		UI_EXPORT bool Create(const EString& fileName);
		//��ָ�������Զ����Ʒ�ʽд���ļ�(����ļ�����������׷��)
		UI_EXPORT bool Write(const char* fileData, size_t fileSize, const EString& outFileName);
		//��ȡ�ļ����ڴ���
		UI_EXPORT size_t Read(const EString& fileName, std::string* data);
	}
	namespace Path {
		//��ʽ��·��
		UI_EXPORT void Format(std::string* str);
		//��ȡ�ļ���(������Ŀ¼�� ��������չ��) 
		UI_EXPORT EString GetFileNameWithoutExtension(const EString& _filename);
		//��ȡ�ļ�����Ŀ¼
		UI_EXPORT EString GetDirectoryName(const EString& _filename);
		//��ȡ�ļ���չ��
		UI_EXPORT EString GetExtension(const EString& _filename);
		//��ȡ�ļ���(�ļ���+��չ��)
		UI_EXPORT EString GetFileName(const EString& filename);
	}
	namespace Directory {
		//���Ŀ¼�Ƿ����
		UI_EXPORT bool Exists(const EString& directoryNme);
		//����Ŀ¼ �����ڵĶ༶Ŀ¼�����Զ�����
		UI_EXPORT bool Create(const EString& path);
		//��Ŀ¼��Ŀ¼�µ��ļ����Ƶ�ָ����λ��
		UI_EXPORT void Copy(const EString& srcPath, const EString& desPath);
		//ʹ��ͨ��������ļ���Ŀ¼
		UI_EXPORT void Find(const EString& directory, std::vector<FileInfo>* result, const EString& pattern = "*.*", bool loopDir = true);
	}
};
