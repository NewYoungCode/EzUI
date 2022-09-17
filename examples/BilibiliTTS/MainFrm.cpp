#include "MainFrm.h"
MainFrm::MainFrm(int width, int height) :Form(width, height) {
	layout.Style.BackgroundColor = Color::Blue;
	//标题
	title.SetText(utf8("哔哩哔哩弹幕助手"));
	title.Action = ControlAction::MoveWindow;
	title.Dock = DockStyle::Horizontal;
	title.SetFixedHeight(40);
	title.Style.BackgroundColor = Color::White;
	//弹幕列表
	//list.SetFixedHeight(400);
	//list.SetFixedWidth(500);
	list.Style.BackgroundColor = Color::Gray;

	layout.AddControl(&title);
	layout.AddControl(&list);
	//设置布局
	SetLayout(&layout);
}

void MainFrm::OnLoad() {
	SetTimer(2000);
}

//去重
bool MainFrm::test1(const EString& timeconst, const EString& userName, const EString& text) {
	auto iter = maps.find(timeconst);
	EString value = userName + text;
	if (iter != maps.end()) {
		if ((*iter).second == value) {
			return true;
		}
	}
	maps.insert(std::pair<EString, EString>(timeconst, value));
	return false;
}

void MainFrm::OnTimer()
{
	WebClient wc;
	wc.AddHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/103.0.5060.66 Safari/537.36 Edg/103.0.1264.44");
	std::string resp;
	wc.HttpGet("https://api.live.bilibili.com/ajax/msg?roomid=6121065", resp);
	JObject json(resp);
	for (auto& item : json["data"]["room"]) {
		EString ts = item["check_info"]["ts"].toStyledString();//时间戳
		EString uid = item["uid"].toStyledString();//用户ID
		EString nickName = item["nickname"].asString();//用户昵称
		EString text = item["text"].asString();//用户发送的文字
		bool b = test1(ts+uid, nickName, text);//
		if (!b) {

			Label* lb = new Label();
			lb->Style.ForeColor = Color::White;
			lb->Style.BorderColor = Color::Black;
			lb->Style.BorderBottom = 1;

			lb->TextAlign=(TextAlign::MiddleLeft);
			lb->SetFixedHeight(30);
			lb->Dock = DockStyle::Horizontal;
			lb->SetText(nickName + " : " + text);
			list.AddControl(lb);
			list.ScrollBar->Move(list.ScrollBar->RollingTotal());
			if (frist) continue;
			TTS::Speak(nickName + utf8("说,") + text);
		}
	}
	frist = false;
}
void MainFrm::OnClose(bool& close) {
	Application::exit(0);
}

