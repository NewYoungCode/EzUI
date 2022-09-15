#pragma once
#include "Window.h"
#include "Painter.h"

namespace EzUI {
	class BoxShadow {
	public:
		EBitmap* _bufBitmap = NULL;
	public:
		int _shadowWidth = 15;
		void setA(int x, int y, BYTE a) {
			DWORD* point = (DWORD*)_bufBitmap->point + (x + y * _bufBitmap->Width);//起始地址+坐标偏移
			((BYTE*)point)[3] = a;//修改A通道数值
			((BYTE*)point)[2] = 0;//修改R通道数值
			((BYTE*)point)[1] = 0;//修改G通道数值
			((BYTE*)point)[0] = 0;//修改B通道数值
		}
		bool SetShadow(int m_Width, int m_Height, size_t iSize) {
			int width = m_Width < m_Height ? m_Width : m_Height;
			int radius = 3;//半径
			int max_size = width / 2 - radius;
			if (max_size <= 0) {
				radius = 0;
				max_size = width / 2;
			}
			iSize = (int)iSize < max_size ? iSize : max_size;
			double piAngle = 3.1415926;
			int iSizeB = 4 * iSize;
			double fN = piAngle / iSize / 5;//设置四条边外模糊度
			double lN = 1.0 / iSize;
			int iAplpha = 0;
			int Left = iSize + radius,
				Top = iSize + radius,
				Right = m_Width - iSize - radius,
				Bottom = m_Height - iSize - radius;
			int x = 0, y = 0;
			for (size_t i = 0; i < iSize; i++)
			{
				iAplpha = int(255 - cos(fN * (i)) * 255);
				for (x = Left; x <= Right; x++)
				{
					setA(x, i, iAplpha);
					setA(x, m_Height - i - 1, iAplpha);
				}
				for (y = Top; y <= Bottom; y++)
				{
					setA(i, y, iAplpha);
					setA(m_Width - i - 1, y, iAplpha);
				}
			}
			double fL = 0;
			int iSizeR = iSize + radius;
			for (int y = 0; y < iSizeR; y++)
			{
				for (int x = y; x < iSizeR; x++)
				{
					fL = sqrt((iSizeR - x) * (iSizeR - x) + (iSizeR - y) * (iSizeR - y));
					if (fL <= radius) {
						break;
					}
					if (fL > radius && fL < iSizeR) {
						iAplpha = int(255 - cos(fN * (iSizeR - fL)) * 255);
					}
					else {
						iAplpha = 0;
					}
					setA(x, y, iAplpha);
					setA(y, x, iAplpha);

					setA(m_Width - x - 1, y, iAplpha);
					setA(m_Width - y - 1, x, iAplpha);

					setA(x, m_Height - y - 1, iAplpha);
					setA(y, m_Height - x - 1, iAplpha);

					setA(m_Width - x - 1, m_Height - y - 1, iAplpha);
					setA(m_Width - y - 1, m_Height - x - 1, iAplpha);
				}
			}
			return true;
		}
		virtual ~BoxShadow() {
			if (_bufBitmap) {
				delete _bufBitmap;
			}
		}
		BoxShadow(int _width, int _height,int ShadowWidth) {
			_shadowWidth = ShadowWidth;
			int width = _width + _shadowWidth * 2;
			int height = _height + _shadowWidth * 2;
			_bufBitmap = new EBitmap(width, height, EBitmap::PixelFormat::PixelFormatARGB);

			::SetBkMode(_bufBitmap->GetDC(), TRANSPARENT);
			SetShadow(width, height, _shadowWidth);
		}
		Size GetSize() {
			return Size{ _bufBitmap->Width ,_bufBitmap->Height };
		}
	};
};
