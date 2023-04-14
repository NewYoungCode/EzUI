#include "FileSystem.h"
//定义....................................................................................................................
namespace FileSystem {
	void ReadFileInfoWin32(const std::string& directory, WIN32_FIND_DATAA&pNextInfo, std::vector<FileSystem::FileInfo>&result) {
		std::string filename;
		filename.append(directory);
		filename.append("\\");
		filename.append(pNextInfo.cFileName);
		Text::Replace(filename, "\\", "/");
		Text::Replace(filename, "//", "/");
		struct FileSystem::FileInfo fileInfo;
		if (pNextInfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) { //目录
			fileInfo.FileType = FileType::Directory;
			fileInfo.FullName = filename;
			fileInfo.FileName = filename;
		}
		else if (FILE_ATTRIBUTE_ARCHIVE& pNextInfo.dwFileAttributes) {
			fileInfo.FileType = FileType::File;
			fileInfo.FileName = pNextInfo.cFileName;
			fileInfo.FullName = filename;
			fileInfo.Extension = Path::GetExtension(filename);
		}
		if (pNextInfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
			fileInfo.ReadOnly = true;
		}
		if (fileInfo.FileType != FileType::None) {
			result.push_back(fileInfo);
		}
	}
	size_t  Find(const std::string& directory, std::vector<FileSystem::FileInfo>&result, const std::string& pattern) {
		HANDLE hFile = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATAA pNextInfo;
		hFile = FindFirstFileA((directory + "\\" + pattern).c_str(), &pNextInfo);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			return 0;
		}
		if (pNextInfo.cFileName[0] != '.') {
			ReadFileInfoWin32(directory, pNextInfo, result);
		}
		while (FindNextFileA(hFile, &pNextInfo))
		{
			if (pNextInfo.cFileName[0] != '.') {
				ReadFileInfoWin32(directory, pNextInfo, result);
			}
		}
		FindClose(hFile);//避免内存泄漏
		return result.size();
	};
};
namespace File {
	bool Exists(const std::string&filename) {
#ifdef _WINDEF_
		DWORD dwAttr = GetFileAttributesA(filename.c_str());
		if (dwAttr == DWORD(-1)) {
			return false;
		}
		if (dwAttr & FILE_ATTRIBUTE_ARCHIVE) {
			return true;
		}
		return false;
#else
		//用stat函数在windows环境下可能会出现问题
		struct stat buf;
		int status = stat(filename.c_str(), &buf);
		if (status == 0 && (buf.st_mode & S_IFREG) == S_IFREG) {
			return true;
		}
		else {
			return false;
		}
#endif
	}
	bool Create(const std::string &filename) {
		File::Delete(filename);
		std::ofstream ofs(filename.c_str(), std::ios::app);
		ofs.close();
		return true;
	}
	bool Delete(const std::string&filename) {
		::remove(filename.c_str());
		return !File::Exists(filename);
	}
	bool Move(const std::string &oldname, const std::string &newname) {
		if (!File::Delete(newname)) {
			printf("Move Faild !\n");
			return false;
		}
		::rename(oldname.c_str(), newname.c_str());
		if (File::Exists(oldname)) {
			return false;
		}
		return true;
	}
	void ReadFile(const  std::string &filename, std::string&outData) {
		outData.clear();
		std::ifstream *ifs = new std::ifstream(filename.c_str(), std::ios::binary);
		std::stringstream ss;
		ss << ifs->rdbuf();
		ifs->close();
		outData = ss.str();
		delete ifs;
	}
	void WriteFile(const std::stringstream & data, const std::string & filename)
	{
		std::string buf = data.str();
		File::Delete(filename);
		std::ofstream *ofs = new std::ofstream(filename, std::ios::binary);
		ofs->write(buf.c_str(), buf.size());
		ofs->flush();
		ofs->close();
		delete ofs;
	}
	void Copy(const std::string & src_filename, const std::string & des_filename, bool cover)
	{
		std::string fileData;
		File::ReadFile(src_filename, fileData);//读取源文件

		if (cover) {
			File::Delete(des_filename);
		}

		std::ofstream ofs(des_filename, std::ios::binary | std::ios::app);//写入到新的文件
		ofs.write(fileData.c_str(), fileData.size());
		ofs.flush();
		ofs.close();
	}
	std::string CreateTempFile(const std::string&filename)
	{
		char buf[513]{ 0 };
		GetTempPathA(512, buf);
		std::string path = std::string(buf) + "\\" + Path::GetFileNameWithoutExtension(Path::StartFileName());
		Path::Create(path);

		std::string file = path + "\\" + filename;
		File::Create(file);

		return file;
	}
};
namespace Path {
	void FileWatcher::TaskFunc()
	{
		std::vector<std::string> files;//启动加载当前文件
		//std::vector<std::string> files = Path::SearchFiles(path, math.c_str());
		for (; exit; )
		{
			//移除不存在的文件
			for (size_t i = 0; i < files.size(); i++)
			{
				if (!File::Exists(files[i]))
				{
					std::vector<std::string>::iterator it = std::find(files.begin(), files.end(), files[i]);
					if (it != files.end()) {
						files.erase(it);
					}
				}
			}
			//判断是否新增的文件s
			std::vector<std::string> tmp = Path::SearchFiles(path, math.c_str());
			for (auto& item : tmp)
			{
				if (find(files.begin(), files.end(), item) == files.end())
				{
					files.push_back(item);
					if (callback) {
						callback(item);
					}
				}
			}
			//值越小越精准
			Sleep(sleep);
		}
	}
	FileWatcher::FileWatcher(const std::string& path, const std::string& math, const std::function<void(const std::string& filename)>& callback, size_t sleep)
	{
		this->sleep = sleep;
		this->callback = callback;
		this->path = path;
		this->math = math;
		TaskFunc();
	}
	FileWatcher::~FileWatcher()
	{
		ctn = false;
	}
	bool Create(const std::string &path) {
		_mkdir(path.c_str());
		if (Path::Exists(path)) {
			return true;
		}
		//创建多级目录
		if (path.find(":") != size_t(-1)) {
			std::string dir = path + "/";
			Text::Replace(dir, "\\", "/");
			Text::Replace(dir, "//", "/");
			std::vector<std::string> arr = Text::Split(dir, "/");
			std::string root;
			if (arr.size() > 0) {
				root += arr[0] + "/";
				for (size_t i = 1; i < arr.size(); i++)
				{
					if (arr[i].empty()) {
						continue;
					}
					root += arr[i] + "/";
					if (!Path::Exists(root)) {
						_mkdir(root.c_str());
					}
				}
			}
		}
		return Path::Exists(path);
	}
	bool Delete(const std::string& directoryName) {
		std::vector<FileSystem::FileInfo>result;
		FileSystem::Find(directoryName, result);
		for (auto&it : result) {
			if (it.FileType == FileSystem::FileType::File) {
				File::Delete(it.FullName);
			}
			if (it.FileType == FileSystem::FileType::Directory) {
				Path::Delete(it.FullName);
			}
		}
		::_rmdir(directoryName.c_str());
		return !Path::Exists(directoryName);
	}
	std::vector<std::string> SearchFiles(const std::string &path, const std::string &pattern)
	{
		std::vector<std::string> files;
		HANDLE hFile = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATAA pNextInfo;
		std::string dir;
		dir.append(path);
		dir.append("\\");
		dir.append(pattern);
		hFile = FindFirstFileA(dir.c_str(), &pNextInfo);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			return files;
		}
		WCHAR infPath[MAX_PATH] = { 0 };
		if (pNextInfo.cFileName[0] != '.')
		{
			std::string filename;
			filename.append(path);
			filename.append("\\");
			filename.append(pNextInfo.cFileName);
			Text::Replace(filename, "\\", "/");
			Text::Replace(filename, "//", "/");
			files.push_back(filename);
		}
		while (FindNextFileA(hFile, &pNextInfo))
		{
			if (pNextInfo.cFileName[0] == '.')
			{
				continue;
			}
			if (pNextInfo.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE) { //如果是文件才要
				std::string filename;
				filename.append(path);
				filename.append("\\");
				filename.append(pNextInfo.cFileName);
				Text::Replace(filename, "\\", "/");
				Text::Replace(filename, "//", "/");

				files.push_back(filename);

			}
		}
		FindClose(hFile);//避免内存泄漏
		return files;
	}
#ifdef _WINDEF_
	bool Exists(const std::string&path) {
		DWORD dwAttr = GetFileAttributesA(path.c_str());
		if (dwAttr == DWORD(-1)) {
			return false;
		}
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{
			return true;
		}

		return false;
	}
#else
	//XP系统下判断可能会有问题
	bool Exists(const std::string & path)
	{
		struct stat buf;
		int status = stat(path.c_str(), &buf);
		if (status == 0) {
			return true;
		}
		return false;
	}
#endif
	std::string GetFileNameWithoutExtension(const std::string &_filename) {
		std::string str = _filename;
		std::string &newStr = str;
		Text::Replace(newStr, "\\", "/");
		int bPos = newStr.rfind("/");
		int ePos = newStr.rfind(".");
		newStr = newStr.substr(bPos + 1, ePos - bPos - 1);
		return newStr;
	}
	std::string GetDirectoryName(const std::string &_filename) {
		std::string str = _filename;
		std::string &newStr = str;
		Text::Replace(newStr, "\\", "/");
		int pos = newStr.rfind("/");
		return _filename.substr(0, pos);
	}
	std::string GetExtension(const std::string &_filename) {
		size_t pos = _filename.rfind(".");
		return pos == size_t(-1) ? "" : _filename.substr(pos);
	}
	std::string GetFileName(const std::string &filename) {
		return Path::GetFileNameWithoutExtension(filename) + Path::GetExtension(filename);
	}
	std::string StartPath() {
		return Path::GetDirectoryName(StartFileName());
	}
	std::string StartFileName() {
		char exeFullPath[MAX_PATH - 1]{ 0 };
		::GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
		return std::string(exeFullPath);
	}
};