#pragma once
#include "EzUI.h"
#include "Painter.h"

typedef std::function< void(UINT uMsg, WPARAM wParam, LPARAM lParam)> UserMessageProc;

enum class Event {
	OnPaint,

	OnMouseWheel,
	OnMouseEnter,
	OnMouseMove,
	OnMouseLeave,
	OnMouseClick,
	OnMouseDoubleClick,
	OnMouseDown,
	OnMouseUp,

	OnKeyDown,
	OnKeyUp
};
enum class ControlState {
	None,
	Hover,
	Active,
	Disable
};

//ժҪ: ������ǰ�ؼ�����
enum ControlType :BYTE {
	ControlBase,//�����ؼ�
	ControlEdit,//�����
	ControlEditWin32,//win32�����
	ControlContainer,//����
	ControlSpacer,//����
	ControlScrollBar, //������
	ControlWindow
};

#define DynamicCast(_ControlPtr,_ControlClass,_ControlTypeEnum) ( ##_ControlPtr->GetType()== ##_ControlTypeEnum?(( ##_ControlClass*)( ##_ControlPtr)):NULL)

enum AnchorStyle : int
{
	//
  // ժҪ:
  //     �ÿؼ���ê�������������κα�Ե��
	None = 0,
	//
	// ժҪ:
	//     �ÿؼ���λ���������ϱ�Ե��
	Top = 1,
	//
	// ժҪ:
	//     �ÿؼ���λ�����������±�Ե��
	Bottom = 2,
	//
	// ժҪ:
	//     �ÿؼ���λ�����������Ե��
	Left = 4,
	//
	// ժҪ:
	//     �ؼ���λ�����������ұ�Ե��
	Right = 8
};
enum class DockStyle {
	// ժҪ:
	//δ����
	None,
	// ժҪ:
	//�ڸ��ؼ��� ���ұ���
	Horizontal,
	// ժҪ:
	//�ڸ��ؼ��� ���±���
	Vertical,
	// ժҪ:
	// �����������ؼ�
	Fill
};


enum class MouseButton {
	// ժҪ: 
	  //     δ��������갴ť��
	None = 0,
	//
	// ժҪ: 
	//     �����ť�����¡�
	Left = 1048576,
	//
	// ժҪ: 
	//     ����Ұ�ť�����¡�
	Right = 2097152,
	//
	// ժҪ: 
	//     ����а�ť�����¡�
	Middle = 4194304,
	//
	// ժҪ: 
	//     �� 1 �� XButton �����¡�
	XButton1 = 8388608,
	//
	// ժҪ: 
	//     �� 2 �� XButton �����¡�
	XButton2 = 16777216,
};



// ժҪ: 
//�����¼�
struct EventArgs {
	Event EventType;
};

// ժҪ: 
//Ϊ����¼��ṩ��������
struct MouseEventArgs :public EventArgs {
	MouseButton Button;
	short Delta;
	Point Location;
};

// ժҪ: 
// Ϊ OnPaint �¼��ṩ���ݡ�
struct PaintEventArgs :public EventArgs {
	using _Painter_ = Painter;//Ԥ��������
	Painter &Painter;//����
	Rect InvalidRectangle;//WM_PAINT�������Ч����
	HWND HWnd;//�����ھ��
	PaintEventArgs(_Painter_&painter) :Painter(painter) {}
};

//�����߿�
struct  Border {
	UI_UInt left;
	UI_UInt top;
	UI_UInt right;
	UI_UInt bottom;
	Color color;
	Border() {

	}
	Border(UI_UInt width, const Color& color) {
		this->left = width;
		this->top = width;
		this->right = width;
		this->bottom = width;
		this->color = color;
	}
	Border(UI_UInt left, UI_UInt top, UI_UInt right, UI_UInt bottom, const Color& color) {
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
		this->color = color;
	}
};

struct Background {
	Color color;
	HImage image;
	Background() {}
	Background(const Color& color, Image *image = NULL) {
		this->color = color;
		this->image = image;
	}
	Background(Image *image) {
		this->image = image;
	}
};
struct Fore {
	Color color;
	HImage image;
	Fore() {}
	Fore(const Color& color, Image *image = NULL) {
		this->color = color;
		this->image = image;
	}
	Fore(Image *image) {
		this->image = image;
	}
};

//�ؼ���ʽ
struct ControlStyle {
	UI_Int Radius;//Բ��ϵ��
	UI_Int BorderLeft;//��߱߿�
	UI_Int BorderTop;//�����߿�
	UI_Int BorderRight;//�ұ߱߿�
	UI_Int BorderBottom;//�ײ��߿�
	Color BorderColor;//�߿���ɫ
	Color BackgroundColor;//������ɫ
	HImage BackgroundImage;//����ͼƬ
	HImage ForeImage;//ǰ��ͼƬ

	EString FontFamily;//��������   ���м̳���
	UI_Float FontSize;//�����С       ���м̳���
	Color ForeColor;//ǰ����ɫ      ���м̳���
private:
	ControlStyle& operator=(const ControlStyle&right) { //��ֱֹ�Ӹ�ֵ ��Ϊ�����ᵼ�� Colorִ�п���ʹ��Color��ò��Ϸ�����Ч
		return *this;
	}
	ControlStyle(const ControlStyle&right) { //��ֹ���� ��Ϊ�����ᵼ�� Colorִ�п���ʹ��Color��ò��Ϸ�����Ч
	}
public:
	ControlStyle() {}
	void SetBorder(const Color&color, int width) { //������border��Ч
		BorderColor = color;
		BorderLeft = width;//��߱߿�
		BorderTop = width;//�����߿�
		BorderRight = width;//�ұ߱߿�
		BorderBottom = width;//�ײ��߿�
	}
	bool IsValid() {
		return Radius.valid ||
			BorderLeft.valid || BorderTop.valid || BorderRight.valid || BorderBottom.valid || BorderColor.valid || BackgroundColor.valid ||
			BackgroundImage.valid || ForeImage.valid ||
			!FontFamily.empty() || FontSize.valid || ForeColor.valid;
	}
};
//�ؼ���Ϊ
enum class ControlAction {
	None,
	MoveWindow,//�ƶ�����
	Mini,//��С��
	Max,//���|�ָ�
	Close//�ر�
};

class  UI_EXPORT IControl {
protected:
	bool _load = false;
	bool _mouseIn = false;
	bool _mouseDown = false;//����Ƿ��Ѿ�����
	bool _hasTimer = false;
public:
	ControlType _Type = ControlType::ControlBase;//�ؼ�����
	UINT_PTR Tag = NULL;
	HWND _hWnd = NULL;//if son is control,the hwnd is parent window handle
public:
	IControl();
	virtual ~IControl();
public:
	virtual void OnMouseMove(const Point&point) = 0;
	virtual void OnMouseLeave() = 0;
	virtual void OnMouseWheel(short zDelta, const Point&point) = 0;
	virtual void OnMouseDoubleClick(MouseButton mbtn, const Point&point) = 0;
	virtual void OnMouseDown(MouseButton mbtn, const Point&point) = 0;
	virtual void OnMouseUp(MouseButton mbtn, const Point&point) = 0;
	virtual void OnSize(const Size&size) = 0;
	virtual void OnLoad() = 0;
	virtual void OnChar(WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnKeyDown(WPARAM wParam) = 0;
	virtual void OnTimer();//��ʱ������
public:
	virtual UINT_PTR SetTimer(size_t interval);
	virtual void KillTimer();
};