#include "Control.h"
namespace EzUI {
	class UI_EXPORT HostWnd :public Control {
	private:
		HWND _hWnd = NULL;
	protected:
		virtual void OnSize(const Size& sz) override;
	public:
		HostWnd(Control* parent);
		HostWnd(HWND hwnd);
		void OnLoad() override;
		virtual ~HostWnd();
		virtual void OnPaint(PaintEventArgs& args)override;
	};
};