#pragma once
#include "core/UIDef.h"
#include "GraphicsTypes.h"

//字体装载管理相关
namespace ezui {
	namespace detail {
		class MemoryFontLoader;
		class CustomFontCollectionLoader;
		class FontManager;

		class EZUI_API FontInfo EZUI_FINAL {
			friend class FontManager;
		public:
			std::vector<uint8_t> fontData;
			IDWriteFontFile* fontFile;
			IDWriteFontCollection* fontCollection;
			std::wstring fontName;
		private:
			FontInfo() : fontFile(NULL), fontCollection(NULL) {};
		public:
			~FontInfo() {
				fontFile->Release();
				fontCollection->Release();
			}
		};
		//管理自定义字体
		class EZUI_API FontManager EZUI_FINAL {
		private:
			std::vector<FontInfo*> fonts;
			IDWriteFactory* m_factory; // 外部传入(无需释放)
			MemoryFontLoader* fontLoader;
			CustomFontCollectionLoader* collectionLoader;
		private:
			FontInfo* MakeFontInfo() { return new FontInfo; };
			// 获取字体名称
			HRESULT GetFontName(IDWriteFontFace* fontFace, std::wstring& outName);
		public:
			FontManager(IDWriteFactory* factory);
			~FontManager();
			// 添加字体并创建字体集合
			bool InstallFont(const char* data, uint32_t size, std::wstring* outFontName = NULL);
			// 根据字体名称查找字体集合
			IDWriteFontCollection* FindFont(const std::wstring& fontName);
		};
	};
};


//字体绘制相关
namespace ezui {
	//字体类
	class EZUI_API Font {
	private:
		Font();
		bool m_ref;
		FontStyle m_fontStyle;
		float m_fontSize;
		int m_fontWeight;
		IDWriteTextFormat* m_value;
		std::wstring m_fontFamily;
		void Copy(const Font& _copy);
	public:
		Font(const Font& _copy);
		Font(const std::wstring& fontFamily, float fontSize, int fontweight = 0, FontStyle fontStyle = FontStyle::Normal);
		float GetFontSize()const;
		float GetFontWeight()const;
		FontStyle GetFontStyle() const;
		const std::wstring& GetFontFamily()const;
		IDWriteTextFormat* Get() const;
		bool operator==(const Font& _right);
		virtual ~Font();
	};

	//文本命中测试数据
	struct HitTestMetrics {
	public:
		bool IsTrailingHit;//命中位置是否在尾部
		int Length;
		int TextPos;//命中的下标
		RectF FontBox;//文字的矩形位置
	public:
		Rect GetCaretRect() {
			float x = FontBox.X;
			if (IsTrailingHit) {
				x += FontBox.Width;
			}
			float y = FontBox.Y;
			return Rect((int)x, (int)y, 1, (int)(FontBox.Height + 0.5));
		}
		//获取字体高度
		int GetFontHeight() {
			return int(FontBox.Height + 0.5);
		}
	};

	//文本布局类
	class EZUI_API TextLayout {
	private:
		TextLayout(const TextLayout& rightValue);
		int m_unicodeSize;
		float m_fontSize;
		IDWriteTextLayout* m_textLayout;
		DWRITE_TEXT_METRICS m_textMetrics;
		std::vector<RectF> m_lineRects;
		std::wstring m_fontFamily;
		void GetMetrics();
	public:
		TextLayout(const std::wstring& text, const Font& font, const SizeF& maxSize = SizeF(EZUI_FLOAT_MAX, EZUI_FLOAT_MAX), TextAlign textAlgin = TextAlign::TopLeft);
		//根据坐标执行命中测试
		Point HitTestPoint(const Point& pt, int* outTextPos, BOOL* outIsTrailingHit, int* fontHeight);
		//根据坐标执行命中测试
		void HitTestPoint(const Point& pt, HitTestMetrics* hitTestMetrics);
		//根据文字下标执行命中测试
		Point HitTestTextPosition(int textPos, BOOL isTrailingHit);
		//获取字体名称
		const std::wstring& GetFontFamily();
		//获取字体大小
		float GetFontSize();
		//获取文本宽度
		int Width();
		//获取文本高度
		int Height();
		//获取文本整体的占用空间
		Size GetFontBox();
		//获取每行文字的矩形位置
		const std::vector<RectF>& GetLineRects();
		//获取字体高度
		int GetFontHeight();
		//获取一共有多少行
		int GetLineCount();
		//获取IDWriteTextLayout指针
		IDWriteTextLayout* Get() const;
		// 设置文本对齐方式
		void SetTextAlign(TextAlign textAlign);
		// 设置下划线 count为-1则表示从pos开始到结尾全部设置下划线
		void SetUnderline(int pos = 0, int count = 0);
		//绘制删除线
		void SetStrikeThrough(int pos, int count);
		//设置行高
		void SetLineHeight(int lineHeight);
		virtual ~TextLayout();
	};
};
