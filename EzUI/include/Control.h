#pragma once
#include "EzUI.h"
namespace EzUI {
	class UI_EXPORT Control :public IControl
	{
	private:
		bool* _isRemove = NULL;//�ؼ��Ƿ��Ѿ����Ƴ����ͷ�
		bool _visible = true;//�ؼ��Ƿ�ɼ� �˱�־Ϊtrue��ʱ�� ����ʵ���в�����ɼ� 
		Controls _controls;//�ӿؼ�
		//����״̬AddControlؼInsertControlؼRemoveControlؼOnSizeʱ��˱�־Ϊ���� ����ResumeLayout��־Ϊ������ ������OnLayout()֮��˱�־ΪNone
		EzUI::LayoutState _layoutState = EzUI::LayoutState::None;
		std::wstring _tipsText;//�����������ʾ����
		Point _lastLocation;//��һ��λ��
		Size _lastSize;//��һ�δ�С
		Rect _lastDrawRect;//���һ����ʾ��λ��
		bool _autoWidth = false;//�Ƿ���������Զ����
		bool _autoHeight = false;//�������ݵĸ߶��Զ��仯
		Size _contentSize;
		std::mutex _paintMtx;//������߳��е���Invalidate()������

		Size _fixedSize{ 0,0 };//����Size
		Rect _rect;//�ؼ���������(���ڸ��ؼ�)
		DockStyle _dock = DockStyle::None;//dock��ʽ
		int _eventNotify = Event::OnMouseClick | Event::OnMouseDoubleClick | Event::OnMouseWheel | Event::OnMouseEnter | Event::OnMouseMove | Event::OnMouseDown | Event::OnMouseUp | Event::OnMouseLeave | Event::OnKeyChar | Event::OnKeyDown | Event::OnKeyUp;//Ĭ����ӵ�������֪ͨ�����п�����
	private:
		Control(const Control&) = delete;
		Control& operator=(const Control&) = delete;
		bool CheckEventPassThrough(const Event& eventType);//����¼��Ƿ��Ѿ�����
		void ComputeClipRect();//������ڸ��ؼ��Ĳü�����
	public:
		EzUI::Margin Margin;//��߾� ��������ռһ�� �� һ�е������ ���ñ߾��ʹ�ؼ���С ��������Ϊ����
		WindowData* PublicData = NULL;//�����ϵĹ�������
		int MousePassThrough = 0;//���Ե������Ϣ
		bool Enable = true;//�ؼ������� ��ֹ״̬����������Ϣ��������
		EString Name;//�ؼ���ObjectName ID
		ControlState State = ControlState::Static;//�ؼ�״̬
		ControlAction Action = ControlAction::None;//�ؼ���Ϊ
		ControlStyle Style;//Ĭ����ʽ
		ControlStyle HoverStyle;//���������ʽ
		ControlStyle ActiveStyle;//��갴����ʽ
		Control* Parent = NULL;//���ؼ�
		Controls VisibleControls;//���ڿؼ��еĿɼ��ؼ�
		const Rect ClipRect;//�ؼ��ڴ����еĿɼ�����
		std::function<void(Control*, const EventArgs&)> EventNotify = NULL;
	protected:
		virtual bool OnEvent(const EventArgs& arg);//�����¼��Ƚ�����
		virtual void DoPaint(PaintEventArgs& args, bool paintSelf = true);//���ƺ���
		virtual void OnPaint(PaintEventArgs& args);//���� 
		virtual void OnChildPaint(PaintEventArgs& args);//�ӿؼ����� �������ش˺����Ż�����������
		virtual void OnBackgroundPaint(PaintEventArgs& painter);//��������
		virtual void OnForePaint(PaintEventArgs& e);//ǰ������
		virtual void OnBorderPaint(PaintEventArgs& painter, const Border& border);//�߿����
		virtual void OnLocation(const LocationEventArgs& arg);//���귢���ı�
		virtual void OnSize(const SizeEventArgs& arg);//��С�����ı�
		virtual void OnRect(const RectEventArgs& arg);
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
		//���м̳�����ʽ
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
		void SetContentWidth(const int& width);//
		void SetContentHeight(const int& height);//
		void SetContentSize(const Size& size);//
		virtual const Size& GetContentSize();
		Size GetSize();
		Point GetLocation();
		virtual const Rect& GetRect();//��ȡ����븸�ؼ����� ���ּ����
		Rect GetClientRect();//��ȡ���ڿͻ��˵ľ���
		const DockStyle& GetDockStyle();//��ȡdock��־
		void SetDockStyle(const DockStyle& dockStyle);
		bool IsPendLayout();//�Ƿ��й���Ĳ���
		const LayoutState& TryPendLayout();//���Թ��𲼾� ���ص�ǰ����״̬
		void EndLayout();//��������
		const Rect& SetRect(const Rect& rect);//������Ը��ؼ����� ����ʵ�ʵ�rect
		virtual void ResumeLayout();//ֱ�ӽ��в���
		virtual void SetTips(const EString& text);//����tips
		virtual ScrollBar* GetScrollBar();//��ȡ�ؼ��Ĺ�����
		bool DispatchEvent(const EventArgs& arg);//�ɷ�ʧȥ�����¼�
		void AddEventNotify(int eventType);//��ӵ�������Ontify�����п�����
		void RemoveEventNotify(int eventType);//�Ƴ�һ�������ڵ�Ontify��Ϣ
		virtual void SetStyleSheet(const EString& styleStr, ControlState _state = ControlState::Static);//
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//�����ؼ���������
		const Controls& GetControls();//��ȡ��ǰ�����ӿؼ� const��������Ϊ������ֱ���޸��ӿؼ�����
		bool Contains(Control* ctl);//��ݹ�ѭȫ�������Ŀؼ��Ƿ����
		Control* FindControl(size_t pos);//ʹ���±��ȡ�ؼ�
		Control* FindControl(const EString& objectName);//Ѱ���ӿؼ� �������� ���� �ȵ�
		Controls FindControl(const EString& attr, const EString& attrValue);//ʹ�����Բ���
		bool SwapControl(Control* childCtl, Control* childCt2);//���ӿؼ��������ؼ�����λ�ý���
		size_t Index();//��ȡ��ǰ�ؼ��ڸ������µ�����
		virtual void InsertControl(size_t pos, Control* childCtl);//ѡ���Բ���ؼ�
		virtual void AddControl(Control* childCtl);//��ӿؼ���ĩβ
		virtual void RemoveControl(Control* childCtl);//ɾ���ؼ� ������һ��������
		virtual void SetParent(Control* parentCtl);//���ø��ؼ�
		virtual void Clear(bool freeControls = false);//��յ�ǰ�����ӿؼ�, freeControls�Ƿ��ͷ������ӿؼ�
		virtual void SetVisible(bool flag);//����Visible��־
		bool IsVisible();//��ȡVisible��־
		virtual bool IsInWindow();//��ǰ�Ƿ���ʾ�ڴ����� ����ʵ������Ƿ���ʾ
		virtual bool Invalidate();// ʹ��ǰ�ؼ�������Ϊ��Ч����
		virtual void Refresh();// ʹ��ǰ�ؼ�����Ϊ��Ч��������������ȫ������Ч����
	};

	//��ӵ��������û��ֶ��ͷ�,
	class Spacer :public Control {
	private:
		//���ɲ������ٳ����ӿؼ�
		void AddControl(Control* ctl) {};
		ControlIterator RemoveControl() {};
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
	//
	// ժҪ:
	// Ϊ֧���Զ�������Ϊ�Ŀؼ�����һ�����ࡣ
	class ScrollableControl :public Control {
	public:
		ScrollableControl() {};
		virtual ~ScrollableControl() {};
	};
	class ScrollBar :public Control {
	private:
		//�������������ٳ����ӿؼ�
		void AddControl(Control* ctl) {};
		ControlIterator RemoveControl() {};
	protected:
		//����Ƿ��Ѿ�����
		bool _mouseDown = false;
		//��һ��������е�����
		int  _lastPoint = 0;
		// ��������ǰ������
		double _sliderPos = 0;
		// �������ĳ���
		int _sliderLength = 0;
	public:
		//������������ �������߱��Ƴ����߱��ͷŵ� ��ע�⽫��ָ������
		ScrollableControl* OWner = NULL;
		//�����������ƫ��֮��Ļص�����
		std::function<void(int)> OffsetCallback = NULL;
		EventScrollRolling Rolling = NULL;//�����¼�
	protected:
		virtual void OnMouseUp(const MouseEventArgs& arg)override
		{
			__super::OnMouseUp(arg);
			_mouseDown = false;
		}
		virtual void OnMouseLeave(const MouseEventArgs& arg) override
		{
			__super::OnMouseLeave(arg);
			_mouseDown = false;
		}
	public:
		virtual void Move(double pos) = 0;
		virtual Rect GetSliderRect() = 0;//
		virtual void OWnerSize(const Size& parentSize) = 0;
		//�������Ƿ��Ѿ���������ʾ
		virtual bool IsDraw() = 0;
		//�������Ƿ��ܹ�����
		virtual bool Scrollable() = 0;
		//��OWner�������ݷ����ı� �����ˢ�¹�����
		void RefreshScroll() {
			Move(_sliderPos);
		};
		ScrollBar() {
			Style.ForeColor = Color(205, 205, 205);//the bar default backgroundcolor
			SetSize({ 10,10 });
		}
		virtual ~ScrollBar() {}
	};
};