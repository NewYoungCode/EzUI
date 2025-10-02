#include "global.h"

//会话Item
class SessionItem :public Label {
public:
	SessionItem(Object* ownerOnj = NULL) :Label(ownerOnj) {
		this->SetText("自定义加载进来的会话Item");
	};
	virtual void OnPaint(PaintEventArgs& args)override {
		args.Graphics.SetColor(Color::Red);
		args.Graphics.FillRectangle(RectF(0, 0, Width(), Height()), 50);
		__super::OnPaint(args);
	}
	virtual void SetAttribute(const UIString& key, const UIString& value) {
		if (key == "src") {
			//读取文件xxx....
			UIString file = value;
		}
		__super::SetAttribute(key, value);
	}
	virtual ~SessionItem() {};
};

//自定义控件
class SessionFrame :public Frame {
public:
	SessionFrame(Object* ownerObj = NULL) :Frame(ownerObj) {
		this->LoadXml("res/session.htm");
	}
	virtual void OnNotify(Control* sd, EventArgs& args)override {
		//...
		__super::OnNotify(sd, args);
	}
	virtual ~SessionFrame() {};
};

//模仿的qq主窗口界面
class MainForm :public Form {
public:
	MainForm(int width, int height);
	virtual void OnNotify(Control* sd, EventArgs& args)override;
	virtual void OnClose(bool& bClose)override;
	virtual ~MainForm();
};
