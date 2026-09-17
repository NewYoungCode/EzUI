#pragma once
#include "core/UIDef.h"
#include "GraphicsTypes.h"

namespace ezui {

	//BGRA 32位图 通常用于框架内部的分层窗口绘制
	class EZUI_API Bitmap {
	private:
		int m_width;
		int m_height;
		HBITMAP m_bmp;
		HDC m_hdc;
		uint8_t* m_point;
		BITMAPINFO& m_bmpInfo;
	private:
		Bitmap(const Bitmap&);            // 禁止拷贝构造
		Bitmap& operator=(const Bitmap&); // 禁止拷贝赋值
		Bitmap(Bitmap&&);                 // 禁止移动构造
		Bitmap& operator=(Bitmap&&);      // 禁止移动赋值
	protected:
		void Create(int width, int height, bool zeroFill = false);
	public:
		//位图宽
		int Width()const;

		//位图高
		int Height()const;

		/// <summary>
		/// //BGRA 32位图
		/// </summary>
		/// <param name="width">位图的宽</param>
		/// <param name="height">位图的高</param>
		Bitmap(int width, int height);

		//从HDC中创建位图 (注意: 只会复制位图区域内容 不会复制整个HDC内容)
		Bitmap(HDC dc, const Rect& rect);

		//设置某个像素的颜色
		void SetPixel(int x, int y, const Color& color);

		//获取某个像素的颜色
		Color GetPixel(int x, int y)const;

		//获取位图数据指针 BGRA格式
		uint8_t* GetPixelData();

		//抹除矩形内容 填充透明色
		void Erase(const Rect& rect);

		//获取HBITMAP句柄
		HBITMAP GetHBITMAP();

		//获取HDC
		HDC GetDC();

		//保存为文件 支持bmp/png/jpg格式
		bool Save(const std::wstring& fileName);

		//克隆一个Bitmap对象
		Bitmap* Clone()const;

		virtual ~Bitmap();
	};
};
