#include "global.h"
#include "lrcPanel.h"
#include "VlcPlayer.h"

class DesktopLrcFrm :public LayeredWindow {
private:
	VlcPlayer* _player;
	LrcPanel _lrc;
protected:
	void OnPaint(PaintEventArgs& args)override;
public:
	DesktopLrcFrm(VlcPlayer* player);
	void LoadLrc(const UIString& lrcData);
	void ChangePostion(int postion);
};