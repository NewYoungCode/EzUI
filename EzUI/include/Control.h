#pragma once
#include "EzUI.h"
namespace EzUI {
	class UI_EXPORT Control :public IControl
	{
	private:
		bool* _isRemove = NULL;//�ؼ��Ƿ��Ѿ����Ƴ����ͷ�
		bool _visible = true;//�ؼ��Ƿ�ɼ� �˱�־Ϊtrue��ʱ�� ����ʵ���в�����ɼ� 
		float _scale = 1.0f;//��ǰ�ؼ���dpi����
		std::list<Control*> _controls;//�ӿؼ�
		std::list<Control*> _spacers;//�洢���ɿؼ�
		//����״̬AddControlؼInsertControlؼRemoveControlؼOnSizeʱ��˱�־Ϊ���� ����ResumeLayout��־Ϊ������ ������OnLayout()֮��˱�־ΪNone
		EzUI::LayoutState _layoutState = EzUI::LayoutState::None;
		EString _tipsText;//�����������ʾ����
		Point _lastLocation;//��һ��λ��
		Size _lastSize;//��һ�δ�С
		bool _autoWidth = false;//�Ƿ���������Զ����
		bool _autoHeight = false;//�������ݵĸ߶��Զ��仯
		Size _contentSize;//�ؼ����ݿ��
		Size _fixedSize;//����Size
		Rect _rect;//�ؼ���������(���ڸ��ؼ�)
		DockStyle _dock = DockStyle::None;//dock��ʽ
	private:
		Control(const Control&) = delete;
		Control& operator=(const Control&) = delete;
		void ComputeClipRect();//������ڸ��ؼ��Ĳü�����
	public:
		Distance Margin;//��߾� ��������ռһ�� �� һ�е������ ���ñ߾��ʹ�ؼ���С ��������Ϊ����
		WindowData* PublicData = NULL;//�����ϵĹ�������
		//��ӵ�������֪ͨ�����п�����
		int EventNotify = Event::OnMouseClick | Event::OnMouseDoubleClick | Event::OnMouseWheel | Event::OnMouseEnter | Event::OnMouseMove | Event::OnMouseDown | Event::OnMouseUp | Event::OnMouseLeave | Event::OnKeyChar | Event::OnKeyDown | Event::OnKeyUp;
		int EventPassThrough = 0;//�ؼ��ɱ���͸���¼�
		bool Enable = true;//�ؼ������� ��ֹ״̬����������Ϣ��������
		EString Name;//�ؼ���ObjectName ID
		ControlState State = ControlState::Static;//�ؼ�״̬
		ControlAction Action = ControlAction::None;//�ؼ���Ϊ
		ControlStyle Style;//Ĭ����ʽ
		ControlStyle HoverStyle;//���������ʽ
		ControlStyle ActiveStyle;//��갴����ʽ
		Control* Parent = NULL;//���ؼ�
		std::list<Control*> VisibleControls;//���ڿؼ��еĿɼ��ؼ�
		const Rect ClipRect;//�ؼ��ڴ����еĿɼ�����
		std::function<void(Control*, const EventArgs&)> EventHandler = NULL;//�¼�������
	protected:
		//��������ʹ��
		virtual void SetContentWidth(const int& width);//
		virtual void SetContentHeight(const int& height);//
		virtual void SetContentSize(const Size& size);//
		virtual bool OnEvent(const EventArgs& arg);//�����¼��Ƚ�����
		virtual void OnPaintBefore(PaintEventArgs& args, bool paintSelf = true);//����֮ǰ
		virtual void OnPaint(PaintEventArgs& args);//���� 
		virtual void OnChildPaint(PaintEventArgs& args);//�ӿؼ����� �������ش˺����Ż�����������
		virtual void OnBackgroundPaint(PaintEventArgs& painter);//��������
		virtual void OnForePaint(PaintEventArgs& e);//ǰ������
		virtual void OnBorderPaint(PaintEventArgs& painter, const Border& border);//�߿����
		virtual void OnLocation(const LocationEventArgs& arg);//���귢���ı�
		virtual void OnSize(const SizeEventArgs& arg);//��С�����ı�
		virtual void OnRect(const RectEventArgs& arg);
		virtual void OnDpiChange(const DpiChangeEventArgs& arg);
		//���ִ����ڴ� ��Ҫ��д��������д�˺��� 
		virtual void OnLayout();
		//����¼�
		virtual void OnMouseMove(const MouseEventArgs& arg);//����ڿؼ����ƶ�
		virtual void OnMouseLeave(const MouseEventArgs& args);//����뿪�ؼ�
		virtual void OnMouseWheel(const MouseEventArgs& arg);//������
		virtual void OnMouseDown(const MouseEventArgs& arg);//��갴��
		virtual void OnMouseUp(const MouseEventArgs& arg);//��굯��
		virtual void OnMouseClick(const MouseEventArgs& arg);//��굥��
		virtual void OnMouseDoubleClick(const MouseEventArgs& arg);//���˫��
		virtual void OnMouseEnter(const MouseEventArgs& arg);//�������
		virtual void OnMouseEvent(const MouseEventArgs& args);//����¼���Ϣ
		//�����¼�
		virtual void OnKeyBoardEvent(const KeyboardEventArgs& _args);//�����¼���Ϣ
		virtual void OnKeyChar(const KeyboardEventArgs& _args);//WM_CAHR��Ϣ
		virtual void OnKeyDown(const KeyboardEventArgs& _args);//WM_CAHR��Ϣ
		virtual void OnKeyUp(const KeyboardEventArgs& _args);//���̵���
		//ʧȥ����
		virtual void OnKillFocus(const KillFocusEventArgs& _args);//ʧȥ�����ʱ����
		virtual void OnRemove();//���Ƴ�����������
	public:
		virtual ControlStyle& GetStyle(const ControlState& _state);//��ȡ��ǰ�ؼ�״̬�µ���ʽ��Ϣ
		virtual ControlStyle& GetDefaultStyle();//���ڻ�ȡ��ͬ�ؼ���ǰĬ�ϵ�
		//��ͨ��ʽ
		int GetBorderTopLeftRadius(ControlState _state = ControlState::None);
		int GetBorderTopRightRadius(ControlState _state = ControlState::None);
		int GetBorderBottomRightRadius(ControlState _state = ControlState::None);
		int GetBorderBottomLeftRadius(ControlState _state = ControlState::None);
		int GetBorderLeft(ControlState _state = ControlState::None);
		int GetBorderTop(ControlState _state = ControlState::None);
		int GetBorderRight(ControlState _state = ControlState::None);
		int GetBorderBottom(ControlState _state = ControlState::None);
		Color GetBorderColor(ControlState _state = ControlState::None);
		Image* GetForeImage(ControlState _state = ControlState::None);
		Image* GetBackImage(ControlState _state = ControlState::None);
		Color GetBackColor(ControlState _state = ControlState::None);
		float GetAngle(ControlState _state = ControlState::None);
		//���м̳�����ʽ
		virtual HCURSOR GetHCursor();
		Color GetForeColor(ControlState _state = ControlState::None);//��ȡĬ�Ͽؼ�״̬��ǰ��ɫ
		std::wstring GetFontFamily(ControlState _state = ControlState::None);//��ȡĬ�Ͽؼ�״̬������Family
		int GetFontSize(ControlState _state = ControlState::None);//��ȡĬ�Ͽؼ�״̬�������С��ʽ
	public:
		Control();
		virtual ~Control();
		void DestroySpacers();//���ٿؼ������е���
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
		virtual Rect GetCareRect();//��ȡ���λ��
		virtual bool IsAutoWidth();//�Ƿ��Զ��߶�
		virtual bool IsAutoHeight();//�Ƿ��Զ��߶�
		virtual void SetAutoWidth(bool flag);//�����Զ����
		virtual void SetAutoHeight(bool flag);//�����Զ��߶�
		virtual void SetAutoSize(bool flag);
		virtual const Size& GetContentSize();
		Size GetSize();
		Point GetLocation();
		virtual const Rect& GetRect();//��ȡ����븸�ؼ����� ���ּ����
		Rect GetClientRect();//��ȡ���ڿͻ��˵ľ���
		const DockStyle& GetDockStyle();//��ȡdock��־
		void SetDockStyle(const DockStyle& dockStyle);
		const float& GetScale();
		bool IsPendLayout();//�Ƿ��й���Ĳ���
		const LayoutState& TryPendLayout();//���Թ��𲼾� ���ص�ǰ����״̬
		void EndLayout();//��������
		const Rect& SetRect(const Rect& rect);//������Ը��ؼ����� ����ʵ�ʵ�rect
		virtual void RefreshLayout();//ˢ�²���
		void SetTips(const EString& text);//����tips����
		const EString& GetTips();//��ȡtips����
		virtual ScrollBar* GetScrollBar();//��ȡ�ؼ��Ĺ�����
		bool DispatchEvent(const EventArgs& arg);//�ɷ�ʧȥ�����¼�
		virtual void SetStyleSheet(const EString& styleStr, ControlState _state = ControlState::Static);//������ʽ����qt����
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//�����ؼ���������
		const std::list<Control*>& GetControls();//��ȡ��ǰ�����ӿؼ� const��������Ϊ������ֱ���޸��ӿؼ�����
		Control* GetControl(size_t pos);//ʹ���±��ȡ�ؼ� ���Զ�����spacer(����)����Ŀؼ�
		bool Contains(Control* ctl);//��ݹ�ѭȫ�������Ŀؼ��Ƿ����
		size_t IndexOf(Control* childCtl);//��ȡ�ӿؼ��ڴ������е�����
		Control* FindControl(const EString& ctlName);//ʹ��nameѰ�������пؼ�,��������
		std::vector<Control*> FindControl(const EString& attrName, const EString& attrValue);//ʹ������Ѱ�������з��������Ŀؼ�,��������
		Control* FindSingleControl(const EString& attrName, const EString& attrValue);//ʹ������Ѱ�������е�һ�����������Ŀؼ�,��������
		Control* FindChild(const EString& ctlName);//Ѱ�ҿؼ�,�����Ӽ�
		std::vector<Control*> FindChild(const EString& attrName, const EString& attrValue);//Ѱ�ҿؼ�,�����Ӽ�
		Control* FindSingleChild(const EString& attrName, const EString& attrValue);//Ѱ�ҿؼ�,�����Ӽ�
		virtual bool SwapChild(Control* childCtl, Control* childCt2);//���ӿؼ��������ؼ�����λ�ý���
		virtual void Insert(size_t pos, Control* childCtl);//ѡ���Բ���ؼ�
		virtual void Add(Control* childCtl);//��ӿؼ���ĩβ
		virtual void Remove(Control* childCtl);//ɾ���ؼ� ������һ��������
		virtual void SetParent(Control* parentCtl);//���ø��ؼ�
		virtual void Clear(bool freeControls = false);//��յ�ǰ�����ӿؼ�, freeControls�Ƿ��ͷ������ӿؼ�
		virtual void SetVisible(bool flag);//����Visible��־
		virtual bool IsVisible();//��ȡVisible��־
		virtual bool Invalidate();// ʹ��ǰ�ؼ�������Ϊ��Ч����
		virtual void Refresh();// ʹ��ǰ�ؼ�����Ϊ��Ч��������������ȫ������Ч����
	};

	//��ӵ��������û��ֶ��ͷ�,
	class UI_EXPORT Spacer :public Control {
	public:
		virtual ~Spacer() {};
	};
	//���о��Ը߶ȵ� �ĵ���
	class UI_EXPORT VSpacer :public Spacer {
	private:
		VSpacer() {};
	public:
		virtual ~VSpacer() {};
		VSpacer(int fixedHeight) {
			SetFixedHeight(fixedHeight);
		}
	};
	//���о��Կ�ȵ� �ĵ���
	class UI_EXPORT HSpacer :public Spacer {
	private:
		HSpacer() {};
	public:
		virtual ~HSpacer() {};
		HSpacer(int fixedWidth) {
			SetFixedWidth(fixedWidth);
		}
	};

	class UI_EXPORT ScrollBar :public Control {
	protected:
		//����Ƿ��Ѿ�����
		bool _mouseDown = false;
		//��һ��������е�����
		int _lastPoint = 0;
		//��������ǰ������
		double _sliderPos = 0;
		//�������ĳ���
		int _sliderLength = 0;
		//������ÿ����һ�εı���
		double _rollRate = 0;
		//�������ڵ�����ƫ��
		int _offset = 0;
		//�����������ݳ���
		int _contentLength = 0;
		//�������ɼ�����(����������)
		int _viewLength = 0;
		//��������ĳ���
		int _overflowLength = 0;
	public:
		//�����������ƫ��֮��Ļص�����
		std::function<void(int)> OffsetCallback = NULL;
		//�����¼� arg1:������ arg2:�����ٷֱ� arg3:��������
		std::function<void(ScrollBar*, float, Event)> Scroll = NULL;
	protected:
		virtual void OnBackgroundPaint(PaintEventArgs& arg)override;
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnMouseUp(const MouseEventArgs& arg)override;
		virtual void OnMouseLeave(const MouseEventArgs& arg) override;
		virtual void OnMouseWheel(const MouseEventArgs& arg)override;
		virtual void GetInfo(int* viewLength, int* contentLength, int* scrollBarLength) = 0;
		void ScrollTo(int offset, const Event& type);
	public:
		//������ָ���ؼ��ɼ�λ��
		virtual void ScrollTo(Control* ctl) = 0;
		//���հٷֱȹ��� 0.0f~1.0f
		void ScrollTo(const float& scrollRate);
		//��ȡ��ǰ��������λ�� ���ȵİٷֱ�
		float ScrollPos();
		//��ȡ����ľ���
		virtual Rect GetSliderRect() = 0;//
		virtual void ParentSize(const Size& parentSize) = 0;
		//�������Ƿ��Ѿ���������ʾ
		bool IsDraw();
		//�������Ƿ��ܹ�����
		bool Scrollable();
		//�����ؼ��������ݷ����ı� �����ˢ�¹�����
		void RefreshScroll();
		ScrollBar();
		virtual ~ScrollBar();
	};
};