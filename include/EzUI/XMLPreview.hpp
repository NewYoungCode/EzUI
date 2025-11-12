#include "Window.h"
#include "Application.h"
#include "VLayout.h"
#include "HLayout.h"
#include "LayeredWindow.h"
#include "TextBox.h"
#include "Button.h"
#include "Task.h"
#include "Timer.h"
#include "Frame.h"

namespace ezui {
	namespace detail {
		class PreviewForm;
		static std::vector<PreviewForm*> g_wnds;
		static bool ReadFile(const UIString& filename, std::string* outFileStream) {
			outFileStream->clear();
			std::ifstream ifs(filename.unicode().c_str(), std::ios::binary);
			if (!ifs.is_open()) {
				return false;
			}
			ifs.seekg(0, std::ios::end);
			std::streamsize size = ifs.tellg();
			if (size == std::streamsize(-1)) {
				return false;
			}
			outFileStream->resize(size);
			ifs.seekg(0);
			ifs.read((char*)outFileStream->c_str(), size);
			return ifs.good();
		}
		static bool WriteRegistryString(const UIString& valueName_, const UIString& data_) {

			std::wstring valueName = valueName_.unicode();
			std::wstring data = data_.unicode();

			HKEY hKey;
			LONG lRes = RegCreateKeyExW(
				HKEY_CURRENT_USER,
				L"Software\\EzUI\\EzUI_XmlPreview",   // 你的程序路径
				0,
				NULL,
				REG_OPTION_NON_VOLATILE,
				KEY_WRITE,
				NULL,
				&hKey,
				NULL
			);

			if (lRes != ERROR_SUCCESS) {
				return false;
			}

			lRes = RegSetValueExW(
				hKey,
				valueName.c_str(),
				0,
				REG_SZ,
				reinterpret_cast<const BYTE*>(data.c_str()),
				static_cast<DWORD>((data.size() + 1) * sizeof(wchar_t))
			);

			RegCloseKey(hKey);
			return (lRes == ERROR_SUCCESS);
		}
		static UIString ReadRegistryString(const UIString& valueName_, const UIString& defaultValue_ = "") {

			std::wstring valueName = valueName_.unicode();
			std::wstring defaultValue = defaultValue_.unicode();

			HKEY hKey;
			LONG lRes = RegOpenKeyExW(
				HKEY_CURRENT_USER,
				L"Software\\EzUI\\EzUI_XmlPreview",
				0,
				KEY_READ,
				&hKey
			);

			if (lRes != ERROR_SUCCESS) {
				return defaultValue;
			}

			wchar_t buffer[512]; // 可改成动态分配
			DWORD bufferSize = sizeof(buffer);
			DWORD type = 0;

			lRes = RegGetValueW(
				hKey,
				NULL,
				valueName.c_str(),
				RRF_RT_REG_SZ,
				&type,
				buffer,
				&bufferSize
			);

			RegCloseKey(hKey);

			if (lRes == ERROR_SUCCESS) {
				return std::wstring(buffer);
			}
			else {
				return defaultValue;
			}
		}
		//===========================================================================================================================================
		//主窗口
		class XMLPreview :public Window {
		public:
			Task* task = NULL;
			Label layout;
			Window* previewWnd = NULL;
			XMLPreview(int cx = 1200, int cy = 900);
			virtual void OnSize(const Size& sz)override;
			virtual void OnMove(const Point& sz)override;
			virtual LRESULT WndProc(UINT msg, WPARAM wp, LPARAM lp)override;
			void OnClose(bool& b)override;
			void CreatePreview(const UIString& dir);
			virtual ~XMLPreview();
		};
		//===========================================================================================================================================
		//预览窗口
		class PreviewForm :public Window {
			UIString m_path;
			Task* task = NULL;
			std::string fileData;
			bool exit = false;
			std::string m_workDir;
		public:
			PreviewForm(const UIString& path);
			virtual ~PreviewForm();
			void OnClose(bool& b);
			void OnSize(const Size& sz);
		};
	};
	//xml预览窗口
	using XMLPreview = detail::XMLPreview;
};


namespace ezui {
	namespace detail {
		//===========================================================================================================================================
		PreviewForm::PreviewForm(const UIString& path) :Window(0, 0) {
			LONG style = GetWindowLong(GetWindowId(), GWL_STYLE);
			style &= ~WS_POPUP;      // 去掉 popup 样式
			style |= WS_CHILD;       // 加上 child 样式
			SetWindowLong(GetWindowId(), GWL_STYLE, style);
			this->m_path = path;
			task = new Task([this]() {//创建任务监听文件修改
				while (!exit)
				{
					bool reLoad = false;//是否需要重新加载
					std::string data;
					if (ReadFile(m_path, &data) && data != fileData) {
						fileData = data;//文件被修改了
						reLoad = true;
					}
					if (reLoad) {
						OutputDebugStringW(UIString(L"重新载入: %s\n").args(m_path.c_str()).unicode().c_str());
						Invoke([this]() {
							//this->SetText(m_path);
							this->LoadXml(m_path);
							this->Invalidate();
							});
					}
					Sleep(100);
				}
				});
		}
		PreviewForm::~PreviewForm() {
			exit = true;
			if (task) {
				delete task;
			}
		}
		inline void PreviewForm::OnClose(bool& b) {
			auto itor = std::find(g_wnds.begin(), g_wnds.end(), this);
			if (itor != g_wnds.end()) {
				g_wnds.erase(itor);
			}
			this->DeleteLater();
		}
		inline void PreviewForm::OnSize(const Size& sz) {
			__super::OnSize(sz);
			UIString title = UIString("%s w:%d h:%d").args(this->m_path.c_str(), this->GetClientRect().Width, this->GetClientRect().Height);
			this->SetText(title);
		}
		//===========================================================================================================================================
		inline XMLPreview::XMLPreview(int cx, int cy) :Window(cx, cy) {

			layout.SetText(L"请将xml界面文件拖拽到此处。");
			layout.Style.FontSize = 15;

			this->SetText(L"EzUI_XMLPreview");
			this->SetLayout(&layout);

			this->CenterToScreen();//屏幕居中
			auto rectStr = ReadRegistryString("rect");
			auto rects = rectStr.split(",");
			if (rects.size() == 4) {
				Rect lastRc = Rect(std::atoi(rects[0].c_str()), std::atoi(rects[1].c_str()), std::atoi(rects[2].c_str()), std::atoi(rects[3].c_str()));
				this->SetRect(lastRc);
			}
			//this->SetTopMost(true);
		}
		inline void XMLPreview::CreatePreview(const UIString& xmlFile)
		{
			auto* wnd = new PreviewForm(xmlFile);
			g_wnds.push_back(wnd);
			::SetParent(wnd->GetWindowId(), GetWindowId());
			int width = Width() * 0.5;
			int height = Height() * 0.5;
			int x = (Width() - width) / 2;
			int y = (Height() - height) / 2;
			wnd->SetRect(Rect(x, y, width, height));
			wnd->Show();

		}
		inline LRESULT XMLPreview::WndProc(UINT msg, WPARAM wp, LPARAM lp) {
			if (msg == WM_DROPFILES) {
				HDROP hDrop = (HDROP)wp;
				UINT numFiles = ::DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);  //  获取拖入的文件数量
				TCHAR szFilePath[MAX_PATH]{ 0 };
				::DragQueryFileW(hDrop, 0, szFilePath, sizeof(szFilePath));  //  获取第一个文件路径
				UIString file = szFilePath;
				this->CreatePreview(file);
			}
			return __super::WndProc(msg, wp, lp);
		}
		inline void XMLPreview::OnSize(const Size& sz) {
			__super::OnSize(sz);
			auto rc = this->GetRect();
			UIString rectStr = UIString("%d,%d,%d,%d").args(rc.X, rc.Y, rc.Width, rc.Height);
			WriteRegistryString("rect", rectStr);
		}
		inline void XMLPreview::OnMove(const Point& pt)
		{
			__super::OnMove(pt);
			auto rc = this->GetRect();
			UIString rectStr = UIString("%d,%d,%d,%d").args(rc.X, rc.Y, rc.Width, rc.Height);
			WriteRegistryString("rect", rectStr);
		}
		inline void XMLPreview::OnClose(bool& b) {
			for (auto& it : g_wnds) {
				delete it;
			}
			Application::Exit(0);
		}
		inline XMLPreview::~XMLPreview() {

		}
	};
};