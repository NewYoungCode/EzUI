#pragma once
#include "EzUI.h"
namespace EzUI {
#define UI_GetClassName() public: virtual EString GetTypeName(){return typeid(this).name();} private:
	class UI_EXPORT Control :public IControl
	{
		UI_GetClassName()
	private:
		bool _stateRepaint = false;//״̬�����ı��ʱ�����
		bool _mouseIn = false;//����Ƿ��ڿؼ���
		bool _load = false;//�Ƿ�load
		Controls _controls;//�ӿؼ�
		Controls _spacer;//�洢�ؼ��²��ֵĵĵ��ɼ���
		//����״̬AddControlؼRemoveControlؼOnSizeʱ��˱�־Ϊ���� ����ResumeLayout��־Ϊ������ ������OnLayout()֮��˱�־ΪNone
		EzUI::LayoutState _layoutState = EzUI::LayoutState::None;
		std::wstring _tipsText;//�����������ʾ����
		HCURSOR _hCursor = NULL;//�����ʽ
		Point _lastLocation;//��һ�δ�С
		Size _lastSize;//��һ�δ�С
		Rect _lastRect;
		Rect _lastDrawRect;//���һ����ʾ��λ��
		int _eventNotify = Event::OnMouseClick | Event::OnMouseDoubleClick | Event::OnMouseWheel | Event::OnMouseEnter | Event::OnMouseMove | Event::OnMouseDown | Event::OnMouseUp | Event::OnMouseLeave | Event::OnChar | Event::OnKeyDown | Event::OnKeyUp;//Ĭ����ӵ�������֪ͨ�����п�����
		std::mutex _rePaintMtx;
		Control(const Control&);
		Control& operator=(const Control&);
	private:
		bool CheckEventPassThrough(const Event& eventType);
		bool CheckEventNotify(const Event& eventType);
	protected:
		int _fixedWidth = 0;//���Կ��
		int _fixedHeight = 0;//���Ը߶�
		Rect _rect;//�ؼ���������(���ڸ��ؼ�)
	public:
		EzUI::Margin Margin;//��߾� ��������ռһ�� �� һ�е������ ���ñ߾��ʹ�ؼ���С ��������Ϊ����
		WindowData* PublicData = NULL;//�����ϵĹ�������
		EzUI::Cursor Cursor = EzUI::Cursor::None;//�����ʽ
		int MousePassThrough = 0;//���Ե������Ϣ
		int ShadowWidth = 0;//�ؼ���Ӱ���
		const bool IsXmlControl = false;//�Ƿ���xml���ؽ�����
		EString Name;//�ؼ���ObjectName ID
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
		const Rect ClipRect;//�ؼ��ڴ����еĿɼ�����
	public:
		EventMouseMove MouseMove;//�ƶ��¼�
		EventMouseEnter MouseEnter;//�����¼�
		EventMouseWheel MouseWheel;//�����¼�
		EventMouseLeave MouseLeave;//����뿪�¼�
		EventMouseDown MouseDown;//��갴���¼�
		EventMouseUp MouseUp;//���̧��
		EventMouseClick MouseClick;//��굥��
		EventMouseDoubleClick MouseDoubleClick;//���˫��
		EventPaint Painting;//�����¼�
	protected:
		ControlStyle& GetStyle(ControlState& _state);//��ȡ��ǰ�ؼ�״̬�µ���ʽ��Ϣ
		virtual void OnPaint(PaintEventArgs& args);//���� 
		virtual void ChildPainting(Controls& controls, PaintEventArgs& args);//�ӿؼ����� �������ش˺����Ż�����������
		virtual void OnBackgroundPaint(PaintEventArgs& painter);//��������
		virtual void OnForePaint(PaintEventArgs& e);//ǰ������
		virtual void OnBorderPaint(PaintEventArgs& painter);//�߿����
		virtual void OnLoad();//�ؼ���һ�μ��� ���� �˺�����LayerWindow���治�����ں�������ӿؼ� �����������ÿؼ�����  
		virtual void OnLocation(const Point& pt);//���귢���ı�
		virtual void OnSize(const Size& size) override;//��С�����ı�
		virtual void OnLayout();//���ִ����ڴ� �������֮��PendLayout���ó�false
		virtual void OnKeyBoardEvent(const KeyboardEventArgs& _args);//�����¼���Ϣ
		virtual void OnMouseMove(const Point& point);//����ڿؼ����ƶ�
		virtual void OnMouseLeave();//����뿪�ؼ�
		virtual void OnMouseWheel(short zDelta, const Point& point);//������
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);//��갴��
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);//��굯��
		virtual void OnMouseClick(MouseButton mbtn, const Point& point);//��굥��
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);//���˫��
		virtual void OnMouseEnter(const Point& point);//�������
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;//WM_CAHR��Ϣ
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) override;//WM_CAHR��Ϣ
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);//���̵���
	public:
		virtual void OnMouseEvent(const MouseEventArgs& args);//����¼���Ϣ
		//���º����뱣֤�ڸ��ؼ���������ɵ������ʹ�� ʹ��ResumeLayout()ִ�в���
		const int& X();
		const int& Y();
		const int& Width();
		const int& Height();
		void SetX(const int& X);
		void SetY(const int& Y);
		void SetLocation(const Point& pt);//�ƶ�����븸�ؼ���λ��
		void SetSize(const Size& size); //���ػ�ؼ�ʱ��������ʹ�� Ӱ������(�����SetRect����)
		void SetFixedSize(const Size& size); //���þ��Կ��
		void SetWidth(const int& width);//���ػ�ؼ�ʱ��������ʹ�� Ӱ������(�����SetRect����)
		void SetHeight(const int& height);//���ػ�ؼ�ʱ��������ʹ�� Ӱ������(�����SetRect����)
		void SetFixedWidth(const int& fixedWidth);//���þ��Կ��
		void SetFixedHeight(const int& fixedHeight);//���þ��Ը߶�
		const int& GetFixedWidth();//��ȡ���Կ��
		const int& GetFixedHeight();//��ȡ���Ը߶�
		virtual const Rect& GetRect();//��ȡ����븸�ؼ����� ���ּ����
		Rect GetClientRect();//��ȡ���ڿͻ��˵ľ���
		bool IsPendLayout();//�Ƿ��й���Ĳ���
		void TryPendLayout();//���Թ��𲼾�
		void EndLayout();//��������
		void SetRect(const Rect& rect);//������Ը��ؼ�����
		virtual void ResumeLayout();//ֱ�ӽ��в���
		virtual void SetTips(const EString& text);
		virtual void OnKillFocus();//ʧȥ�����ʱ����
		virtual void OnRemove();//���Ƴ�����������
		void Trigger(const MouseEventArgs& args);//������������Ϣ
		void Trigger(const KeyboardEventArgs& args);//�������������Ϣ
		void AddEventNotify(int eventType);//��ӵ�������ontify�����п�����
		void RemoveEventNotify(int eventType);
		void SetCursor(const EString& fileName);
		HCURSOR GetCursor();
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
		virtual void SetStyleSheet(const EString& styleStr, ControlState _state = ControlState::None);//
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
		bool IsVisible();//��ǰ�Ƿ���ʾ�ڴ�����
		virtual bool Invalidate();// ʹ��ǰ�ؼ�������Ϊ��Ч����
		virtual void Refresh();// ʹ��ǰ�ؼ�����Ϊ��Ч��������������ȫ������Ч����
		void ComputeClipRect();//������ڸ��ؼ��Ĳü�����
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
		UI_GetClassName()
	public:
		ScrollBar() {
			Style.ForeColor = { 205,205,205 };//the bar backgroundcolor
			Style.BackgroundColor = { 240,240,240 };//the bar backgroundcolor
			ActiveStyle.ForeColor = { 166,166,166 };
			SetWidth(10);//�������߶�
			SetHeight(10);//���������
			Style.Radius = 10;
		}
		virtual ~ScrollBar() {}
	};
};