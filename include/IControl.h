#pragma once
#include "EzUI.h"
#include "Painter.h"

enum  Event :int {
	OnMouseWheel = 1,
	OnMouseEnter = 2,
	OnMouseMove = 4,
	OnMouseLeave = 8,
	OnMouseClick = 16,
	OnMouseDoubleClick = 32,
	OnMouseDown = 64,
	OnMouseUp = 128,

	OnKeyDown = 256,
	OnKeyUp = 512,

	OnPaint = 1024
};
enum class ControlState {
	None,
	Hover,
	Active,
	Disable
};

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
	Painter& Painter;//����
	Rect InvalidRectangle;//WM_PAINT�������Ч����
	HWND HWnd;//�����ھ��
	PaintEventArgs(_Painter_& painter) :Painter(painter) {}
};

//�ؼ���ʽ
class UI_EXPORT ControlStyle {
public:
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
	void operator=(const ControlStyle& right) {} //��ֱֹ�Ӹ�ֵ ��Ϊ�����ᵼ�� Colorִ�п���ʹ��Color��ò��Ϸ�����Ч
	ControlStyle(const ControlStyle& right) {} //��ֹ���� ��Ϊ�����ᵼ�� Colorִ�п���ʹ��Color��ò��Ϸ�����Ч
public:
	ControlStyle() {}
	virtual ~ControlStyle() {}
	void SetBorder(const Color& color, int width);
	bool IsValid();
	void SetStyleSheet(const EString& styleStr);
	void SetStyle(const EString& key,const EString&value);
};
//�ؼ���Ϊ
enum class ControlAction {
	None,
	MoveWindow,//�ƶ�����
	Mini,//��С��
	Max,//���|�ָ�
	Close//�ر�
};
typedef std::map<EString, EString> Attributes;
typedef std::map<EString, EString>::iterator AttributeIterator;
class  UI_EXPORT IControl {
protected:
	bool _load = false;
	bool _mouseIn = false;
	bool _mouseDown = false;//����Ƿ��Ѿ�����
	bool _hasTimer = false;
	Attributes _attrs;
public:
	UINT_PTR Tag = NULL;
	HWND _hWnd = NULL;//if son is control,the hwnd is parent window handle
public:
	IControl();
	virtual ~IControl();
public:
	virtual void OnMouseMove(const Point& point) = 0;
	virtual void OnMouseLeave() = 0;
	virtual void OnMouseWheel(short zDelta, const Point& point) = 0;
	virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point) = 0;
	virtual void OnMouseDown(MouseButton mbtn, const Point& point) = 0;
	virtual void OnMouseUp(MouseButton mbtn, const Point& point) = 0;
	virtual void OnSize(const Size& size) = 0;
	virtual void OnLoad() = 0;
	virtual void OnChar(WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnKeyDown(WPARAM wParam) = 0;
	virtual void OnTimer();//��ʱ������
public:
	virtual void SetStyleSheet(const EString& styleStr);//����style
	virtual void SetAttribute(const EString& attrName, const EString& attrValue);//��������
	virtual const EString GetAttribute(const EString& attrName);//��ȡ����
	virtual UINT_PTR SetTimer(size_t interval);
	virtual void KillTimer();
};