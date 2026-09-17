#pragma once
#include "UICore.h"
namespace ezui {
	//用于描述控件视觉状态(模仿CSS伪类)
	EZUI_SCOPED_ENUM_BEGIN(VisualState, int16_t) {
		None = 0,       // 无状态(未指定)
			Normal = 1,     // 默认状态(无伪类)
			Disabled = 2,   // 禁用状态 :disabled
			Checked = 4,    // 选中状态 :checked
			Hover = 8,      // 鼠标悬浮 :hover
			Active = 16,    // 鼠标按下 :active
			Focus = 32      // 获得焦点 :focus
	} EZUI_SCOPED_ENUM_END(VisualState)
			EZUI_ENUM_OPERATORS(VisualState, int16_t);

		//样式属性集合 允许重复元素
		typedef std::vector<std::pair<UIString, UIString>> StylePropertyMap;

		//样式规则
		struct StyleRule
		{
			VisualState state;// Hover / Pressed / Disabled / Focus
			UIString selector;// "#btnLogin"
			StylePropertyMap properties;
		public:
			StyleRule(const UIString& selectorName, const StylePropertyMap& styleItems);
		};

		/// <summary>
		/// 描述边框的一些信息
		/// </summary>
		struct Border {
		public:
			PropertyValue<int16_t> Left;//左边边框大小
			PropertyValue<int16_t> Top;//顶部边框大小
			PropertyValue<int16_t> Right;//右边边框大小
			PropertyValue<int16_t> Bottom;//底部边框大小
			PropertyValue<int16_t> TopLeftRadius;//左上角半径
			PropertyValue<int16_t> TopRightRadius;//右上角半径
			PropertyValue<int16_t> BottomRightRadius;//右下角半径
			PropertyValue<int16_t> BottomLeftRadius;//左下角半径
			PropertyValue<Color> Color;//边框颜色
			PropertyValue<BorderStyle> Style;//边框样式
		private:
			class Radius_ {
				Border& Border;
			public:
				Radius_(ezui::Border& bd) :Border(bd) {}
				//对四个角度同时设置半径大小
				Radius_& operator=(PropertyValue<int16_t> radius) {
					Border.TopLeftRadius = radius;
					Border.TopRightRadius = radius;
					Border.BottomRightRadius = radius;
					Border.BottomLeftRadius = radius;
					return *this;
				}
			};
		public:
			//同时设置四个角度的半径 
			//也可以单独设置每个角度的半径
			Border::Radius_ Radius;
		public:
			Border() : Style(BorderStyle::None), Radius(*this) {
				this->Style.SetEnabled(false);
			}
			//对四个边设置大小
			Border& operator=(PropertyValue<int16_t> borderWidth) {
				Left = borderWidth;
				Top = borderWidth;
				Right = borderWidth;
				Bottom = borderWidth;
				return *this;
			}
			//缩放边框
			void Scale(float scale) {
				Left.Set(int16_t(Left * scale + 0.5));
				Top.Set(int16_t(Top * scale + 0.5));
				Right.Set(int16_t(Right * scale + 0.5));
				Bottom.Set(int16_t(Bottom * scale + 0.5));
				TopLeftRadius.Set(int16_t(TopLeftRadius * scale + 0.5));
				TopRightRadius.Set(int16_t(TopRightRadius * scale + 0.5));
				BottomRightRadius.Set(int16_t(BottomRightRadius * scale + 0.5));
				BottomLeftRadius.Set(int16_t(BottomLeftRadius * scale + 0.5));
			}
		};

		namespace detail {
			// 为控件样式提供数据。
			class EZUI_API StyleData {
				friend class Control;
			private:
				PropertyValue<SizeMode> BackImageSizeModel;//内部缓存使用
				PropertyValue<Point> BackImagePosition;//内部缓存使用
				PropertyValue<Size> BackImageSize;//内部缓存使用
			public:
				//边框信息
				ezui::Border Border;
				//背景颜色
				PropertyValue<Color> BackColor;
				//背景图片 如果指定的图片被删除 请必须将此置零
				PropertyValue<Image*> BackImage;
				//字体名称 具有父控件继承性
				PropertyValue<std::wstring> FontFamily;
				//字体大小 具有父控件继承性
				PropertyValue<int> FontSize;
				//字体粗度 值范围1~999 如需加粗一般为700即可 具有继承性值
				PropertyValue<int> FontWeight;
				//字体样式 具有父控件继承性
				PropertyValue<FontStyle> FontStyle;
				//前景颜色 (通常应用于字体颜色)具有父控件继承性
				PropertyValue<Color> ForeColor;
				//鼠标样式
				PropertyValue<HCURSOR> Cursor;
				//正数角度(0~ 360) -> 逆时针旋转
				//负数角度(0~ -360) -> 顺时针旋转
				PropertyValue<float> Angle;
				//透明度(0~1.0)
				PropertyValue<float> Opacity;
			public:
				StyleData() {
					Angle.Set(std::numeric_limits<float>::quiet_NaN());
					Opacity.Set(std::numeric_limits<float>::quiet_NaN());
				}
				void Scale(float scale);
				~StyleData() {}
			};
		};


		//样式代理(用于控件样式的中转)
		class EZUI_API StyleProxy EZUI_FINAL {
		private:
			StyleProxy(const StyleProxy&);            // 禁止拷贝构造
			StyleProxy& operator=(const StyleProxy&); // 禁止拷贝赋值
			StyleProxy(StyleProxy&&);                 // 禁止移动构造
			StyleProxy& operator=(StyleProxy&&);      // 禁止移动赋值
			detail::StyleData* m_style;
			detail::StyleData& Ensure() {
				if (!m_style) {
					m_style = new detail::StyleData;
				}
				return *m_style;
			}
		public:
			StyleProxy() : m_style(NULL) {}
			//使用指针方式设置样式
			// m_control->Style->BackColor=Color::Red;
			detail::StyleData* operator->() {
				return &(Ensure());
			}
			~StyleProxy()
			{
				if (m_style)
				{
					delete m_style;
				}
			}
			bool IsEmpty()const {
				return (m_style == NULL);
			}
			operator bool() const {
				return !IsEmpty();
			}
		};

		//样式类
		class UIStyle {
			friend class Control;
			friend class UILoader;
		private:
			//是否是像素单位
			static bool IsPx(const UIString& num, float& outNum);
			//字符串转数值
			static float ToFloat(const UIString& numStr);
			//处理url(d:/imgs/aa.png)路径
			static Image* MakeImage(UIString value);
			//构建边框信息并回调
			static void MakeBorder(const UIString& key, UIString str, Border& border, float scale);
		private:
			static void AnalysisStyle(const UIString& styleStr, std::list<StyleRule>* out);//分析样式
			static void AnalysisStyleItems(const UIString& styleStr, StylePropertyMap* maps);
			static int MatchStyle(Control* ctl, const UIString& selectorName, const std::list<StyleRule>& selectors);//匹配样式 返回匹配成功个数
			static void ApplyStyle(Control* ctl, const UIString& selectName, const std::list<StyleRule>& selectors);//应用样式
			static void ApplyStyle(Control* ctl, const std::list<StyleRule>& selectors);//应用样式
		};
};
