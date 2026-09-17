#pragma once
#include "EzUI.h"

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
			Thread* task;
			Label layout;
			Window* previewWnd;
			XMLPreview(int cx = 1200, int cy = 900);
			virtual void OnSize(const Size& sz)EZUI_OVERRIDE;
			virtual void OnMove(const Point& sz)EZUI_OVERRIDE;
			void OnClose(bool& allowClose)EZUI_OVERRIDE;
			void CreatePreview(const UIString& dir);
			virtual ~XMLPreview();
		};
		//===========================================================================================================================================
		//预览窗口
		class PreviewForm :public BorderlessWindow {
			UIString m_path;
			Thread* task;
			std::string fileData;
			bool exit;
			std::string m_workDir;
		public:
			PreviewForm(const UIString& path);
			virtual ~PreviewForm();
			void OnClose(bool& allowClose);
			void OnSize(const Size& sz);
		};
	};
	//xml预览窗口
	typedef detail::XMLPreview XMLPreview;
};


namespace ezui {
	namespace detail {
		//===========================================================================================================================================
		PreviewForm::PreviewForm(const UIString& path)
			: BorderlessWindow(0, 0), task(NULL), exit(false) {
			LONG style = GetWindowLongPtr(GetWindowHandle(), GWL_STYLE);
			style &= ~WS_POPUP;      // 去掉 popup 样式
			style |= WS_CHILD;       // 加上 child 样式
			SetWindowLongPtr(GetWindowHandle(), GWL_STYLE, style);
			this->m_path = path;
			task = new Thread([this]() {//创建任务监听文件修改
				while (!exit)
				{
					bool reLoad = false;//是否需要重新加载
					std::string data;
					if (ReadFile(m_path, &data) && data != fileData) {
						fileData = data;//文件被修改了
						reLoad = true;
					}
					if (reLoad) {
						UIString out = UIString(L"重新载入: ") + m_path + "\n";
						OutputDebugStringW(out.unicode().c_str());
						Invoke([this]() {
							//this->SetTitle(m_path);
							this->LoadXml(m_path);
							this->Invalidate();
							});
					}
					SleepMs(100);
				}
				});
			this->CloseShadow();//关闭阴影窗口
		}
		PreviewForm::~PreviewForm() {
			exit = true;
			if (task) {
				delete task;
			}
		}
		inline void PreviewForm::OnClose(bool& allowClose) {
			auto itor = std::find(g_wnds.begin(), g_wnds.end(), this);
			if (itor != g_wnds.end()) {
				g_wnds.erase(itor);
			}
			this->DeleteLater();
		}
		inline void PreviewForm::OnSize(const Size& sz) {
			__super::OnSize(sz);
			UIString title = this->m_path +
				" w:" + ezui::ToString(this->GetClientRect().Width) +
				" h:" + ezui::ToString(this->GetClientRect().Height);
			this->SetTitle(title);
		}
		//===========================================================================================================================================
		inline XMLPreview::XMLPreview(int cx, int cy)
			: Window(), task(NULL), previewWnd(NULL) {

			layout.SetText(L"请将xml界面文件拖拽到此处。");
			layout.Style->FontSize = 15;

			this->SetTitle(L"EzUI_XMLPreview");
			this->SetLayout(&layout);
			layout.SetDropEnabled(true);
			layout.AddEventHandler(Event::DragEnter | Event::DragOver | Event::Drop, [this](Control*, EventArgs* e) {
				FileDragEventArgs* args = e->As<FileDragEventArgs>();
				if (!args || args->Files().empty()) {
					return;
				}
				args->Accept();
				if (args->EventType() == Event::Drop) {
					this->CreatePreview(args->Files()[0]);
				}
				});

			auto rectStr = ReadRegistryString("rect");
			auto rects = rectStr.split(",");
			if (rects.size() == 6) {
				Rect lastRc = Rect(std::atoi(rects[0].c_str()), std::atoi(rects[1].c_str()), std::atoi(rects[2].c_str()), std::atoi(rects[3].c_str()));
				float lastScale = std::atof(rects[4].c_str());
				bool lastMaximized = std::atoi(rects[5].c_str());
				this->SetRect(lastRc, lastScale);
				if (lastMaximized) {
					this->ShowMaximized();
				}
			}
			else {
				this->SetSize(Size(cx, cy));
				this->CenterToScreen();//屏幕居中
			}
			//this->SetTopMost(true);
		}
		inline void XMLPreview::CreatePreview(const UIString& xmlFile)
		{
			auto* wnd = new PreviewForm(xmlFile);
			g_wnds.push_back(wnd);
			::SetParent(wnd->GetWindowHandle(), GetWindowHandle());
			int width = Width() * 0.5;
			int height = Height() * 0.5;
			int x = (Width() - width) / 2;
			int y = (Height() - height) / 2;
			wnd->SetRect(Rect(x, y, width, height));
			wnd->Show();

		}
		inline void XMLPreview::OnSize(const Size& sz) {
			__super::OnSize(sz);
			auto rc = this->GetRect();
			UIString rectStr =
				ezui::ToString(rc.X) + "," +
				ezui::ToString(rc.Y) + "," +
				ezui::ToString(rc.Width) + "," +
				ezui::ToString(rc.Height) + "," +
				ezui::ToString(this->GetScale()) + "," +
				(this->IsMaximized() ? "1" : "0");
			WriteRegistryString("rect", rectStr);
		}
		inline void XMLPreview::OnMove(const Point& pt)
		{
			__super::OnMove(pt);
			auto rc = this->GetRect();
			UIString rectStr =
				ezui::ToString(rc.X) + "," +
				ezui::ToString(rc.Y) + "," +
				ezui::ToString(rc.Width) + "," +
				ezui::ToString(rc.Height) + "," +
				ezui::ToString(this->GetScale()) + "," +
				(this->IsMaximized() ? "1" : "0");
			WriteRegistryString("rect", rectStr);
		}
		inline void XMLPreview::OnClose(bool& allowClose) {
			for (auto it = g_wnds.begin(); it != g_wnds.end(); ++it) {
				delete *it;
			}
			Application::Exit(0);
		}
		inline XMLPreview::~XMLPreview() {

		}
	};
};
