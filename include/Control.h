#pragma once
#include "IType.h"
namespace EzUI {
	class UI_EXPORT Control :public IControl
	{
	private:
		Control(const Control&);
		Control& operator=(const Control&);
	protected:
		Controls _controls;//�ӿؼ�
		Controls _spacer;//�洢�ؼ��²��ֵĵĵ��ɼ���
		Rect _rect;//�ؼ���������(���ڸ��ؼ�)
		Rect _lastDrawRect;//���һ����ʾ��λ��
		int _fixedWidth = 0;//���Կ��
		int _fixedHeight = 0;//���Ը߶�
		bool _pendLayout = true;//�����Ƿ񱻹��� ��Addcontrol����RemoveControl��ʱ��˱�־Ϊtrue ������ResumeLayout()֮��˱�־Ϊfalse
		Tuple<LPTSTR> _LastCursor;//��һ��������ʽ
	public:
		EzUI::Cursor Cursor = EzUI::Cursor::None;//�����ʽ
		int MousePassThrough = 0;//���Ե������Ϣ
		const Rect ClipRect;//�ؼ��ڴ����еĿɼ�����
		int ShadowWidth = 0;//�ؼ���Ӱ���
		bool IsXmlControl = false;//�Ƿ���xml���ؽ�����
		EString Name;//�ؼ���ObjectName
		ScrollBar* ScrollBar = NULL;//��ֱ����������ˮƽ������ һ���ؼ�ֻ������һ��
		ControlState State = ControlState::None;//�ؼ�״̬
		ControlAction Action = ControlAction::None;//�ؼ���Ϊ
		ControlStyle Style;//Ĭ����ʽ
		ControlStyle HoverStyle;//���������ʽ
		ControlStyle ActiveStyle;//��갴����ʽ
		bool Visible = true;//�ؼ��Ƿ�ɼ�
		Control* Parent = NULL;//���ؼ�
		Controls VisibleControls;//���ڿؼ��еĿɼ��ؼ�
		DockStyle Dock = DockStyle::None;//dock��ʽ
	public:
		EventMouseMove MouseMove;//�ƶ��¼�
		EventMouseEnter MouseEnter;//�����¼�
		EventMouseWheel MouseWheel;//�����¼�
		EventMouseLeave MouseLeave;//����뿪�¼�
		EventMouseDown MouseDown;//��갴���¼�
		EventMouseUp MouseUp;//���̧��
		EventMouseClick MouseClick;//��굥��
		EventMouseDoubleClick MouseDoubleClick;//���˫��
		EventPaint Painting = NULL;
		EventBackgroundPaint  BackgroundPainting = NULL;
		EventForePaint  ForePainting = NULL;
	public:
		const int& X();
		const int& Y();
		const int& Width();
		const int& Height();
		void SetX(const int& X);
		void SetY(const int& Y);
		void SetLocation(const Point& pt);//�ƶ�����븸�ؼ���λ��
		void SetSize(const Size& size); //���ػ�ؼ�ʱ��������ʹ�� Ӱ������(�����SetRect����)
		void SetWidth(const int& width);//���ػ�ؼ�ʱ��������ʹ�� Ӱ������(�����SetRect����)
		void SetHeight(const int& height);//���ػ�ؼ�ʱ��������ʹ�� Ӱ������(�����SetRect����)
		void SetFixedWidth(const int& fixedWidth);//���þ��Կ��
		void SetFixedHeight(const int& fixedHeight);//���þ��Ը߶�
		const int& GetFixedWidth();//��ȡ���Կ��
		const int& GetFixedHeight();//��ȡ���Ը߶�
		const Rect& GetRect();//��ȡ����븸�ؼ�����
		virtual void OnLayout(const Size& parentRect, bool instantly = true);//���ؼ���С�ı��¼�  instantly������Ч
		Rect GetClientRect();//��ȡ���ڿͻ��˵ľ���
		virtual void ComputeClipRect();//������ڸ��ؼ��Ĳü�����
		bool CheckEventPassThrough(Event eventType);
		virtual void SetRect(const Rect& rect, bool rePaint = false);//������Ը��ؼ�����
	public:
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;//WM_CAHR��Ϣ
		virtual void OnKeyDown(WPARAM wParam) override;//WM_CAHR��Ϣ
		virtual void OnPaint(PaintEventArgs& args);//���� 
		virtual void OnChildPaint(Controls& controls, PaintEventArgs& args);//�ӿؼ����� �������ش˺����Ż�����������
		virtual void OnBackgroundPaint(PaintEventArgs& painter);//��������
		virtual void OnForePaint(PaintEventArgs& e);//ǰ������
		virtual void OnBorderPaint(PaintEventArgs& painter);//�߿����
		virtual void OnLoad();//�ؼ���һ�μ��� ���� �˺�����LayerWindow���治�����ں�������ӿؼ� �����������ÿؼ�����  
		virtual void OnSize(const Size& size);//��С�����ı�
		virtual void OnKillFocus();//ʧȥ�����ʱ����
	protected:
		virtual void OnMouseEvent(const MouseEventArgs& args);//����¼���Ϣ
		virtual void OnMouseMove(const Point& point);//����ڿؼ����ƶ�
		virtual void OnMouseLeave();//����뿪�ؼ�
		virtual void OnMouseWheel(short zDelta, const Point& point);//������
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);//��갴��
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);//��굯��
		virtual void OnMouseClick(MouseButton mbtn, const Point& point);//��굥��
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);//���˫��
		virtual void OnMouseEnter(const Point& point);//�������
	public:
		void Trigger(const MouseEventArgs& args);//������������Ϣ
	protected:
		ControlStyle& GetStyle(ControlState& _state);//��ȡ��ǰ�ؼ�״̬�µ���ʽ��Ϣ
	public:
		Control();
		virtual ~Control();
		void DestroySpacers();
		//��ͨ��ʽ
		HImage  GetForeImage();
		HImage  GetBackgroundImage();
		UI_Int GetRadius();
		UI_Int GetBorderLeft();
		UI_Int GetBorderTop();
		UI_Int GetBorderRight();
		UI_Int GetBorderBottom();
		Color GetBorderColor();
		Color GetBackgroundColor();
		virtual void SetStyleSheet(const EString& styleStr);//
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//�����ؼ���������
		//���м̳�����ʽ
		EString GetFontFamily(ControlState _state = ControlState::None);//��ȡĬ�Ͽؼ�״̬������Family
		UI_Int GetFontSize(ControlState _state = ControlState::None);//��ȡĬ�Ͽؼ�״̬�������С��ʽ
		Color GetForeColor(ControlState _state = ControlState::None);//��ȡĬ�Ͽؼ�״̬��ǰ��ɫ
		Controls& GetControls();//��ȡ��ǰ�����ӿؼ�
		Control* GetControl(size_t pos);//ʹ���±��ȡ�ؼ�
		Control* FindControl(const EString& objectName);//Ѱ���ӿؼ� �������� ���� �ȵ�
		Controls FindControl(const EString& attr, const EString& attrValue);//ʹ�����Բ���
		size_t Index();
		virtual void AddControl(Control* ctl);//��ӿؼ�
		virtual ControlIterator RemoveControl(Control* ctl);//ɾ���ؼ� ������һ��������
		virtual void Clear(bool freeControls = false);//��յ�ǰ�����ӿؼ�, freeControls�Ƿ��ͷ������ӿؼ�
		virtual void Rending(PaintEventArgs& args);//���ƺ���
		virtual bool Invalidate();// ʹ��ǰ�ؼ�������Ϊ��Ч����
		virtual void Refresh();// ʹ��ǰ�ؼ�����Ϊ��Ч��������������ȫ������Ч����
	};

	//��ӵ��������û��ֶ��ͷ�,
	class Spacer :public Control {
	public:
		virtual ~Spacer() {};
	};
	//���о��Ը߶ȵ� �ĵ���
	class VSpacer :public Spacer {
	private:
		VSpacer() {};
	public:
		virtual ~VSpacer() {};
		VSpacer(int fixedHeight) {
			SetFixedHeight(fixedHeight);
		}
	};
	//���о��Կ�ȵ� �ĵ���
	class HSpacer :public Spacer {
	private:
		HSpacer() {};
	public:
		virtual ~HSpacer() {};
		HSpacer(int fixedWidth) {
			SetFixedWidth(fixedWidth);
		}
	};

	class  ScrollBar :public Control, public IScrollBar {
	public:
		ScrollBar() {
			Style.ForeColor = { 200,255,255,255 };//the bar backgroundcolor
			Style.BackgroundColor = { 200,200,200,200 };//the bar backgroundcolor
			SetWidth(7);//���������
			Style.Radius = 7;
		}
		virtual ~ScrollBar() {}
	};
};