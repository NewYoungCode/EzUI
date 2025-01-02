#include "global.h"
#include "LrcControl.h"
#include "VlcPlayer.h"

class DesktopLrcFrm :public LayeredWindow {
private:
	VlcPlayer* _player;
	LrcControl _lrc;
protected:
	void OnPaint(PaintEventArgs& args)override;
public:
	DesktopLrcFrm(VlcPlayer* player);
	void LoadLrc(const EString& lrcData);
	void ChangePostion(int postion);
};