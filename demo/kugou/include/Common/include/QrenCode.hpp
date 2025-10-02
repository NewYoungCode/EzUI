#pragma once
#include "Common.h"
#include "qrcode/qrencode.h"

#ifdef  _WIN64
#ifdef  _DEBUG
#pragma comment (lib,"X64/libqrencode_d.lib")
#else
#pragma comment (lib,"X64/libqrencode.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib,"libqrencode_d.lib")
#else
#pragma comment(lib,"libqrencode.lib")
#endif // !_DEBUG
#endif

namespace QrenCode {
	//生成二维码 返回位图 用完位图记得释放
	inline HBITMAP Generate(const std::string& str, QRecLevel level = QRecLevel::QR_ECLEVEL_M) {
		unsigned int unWidth, x, y, l, n, unWidthAdjusted;
		byte* pRGBData = NULL, * pSourceData, * pDestData;
		QRcode* pQRC = NULL;
		HBITMAP bmp = NULL;
		do
		{
			if (pQRC = QRcode_encodeString(str.c_str(), 0, level, QR_MODE_8, 1))
			{
				unWidth = pQRC->width;
				unWidthAdjusted = unWidth * 8 * 3;
				if (unWidthAdjusted % 4) {
					unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
				}

				BITMAPINFO bmpInfo;
				memset(&bmpInfo, 0, sizeof(bmpInfo));
				BITMAPINFOHEADER& kInfoHeader = bmpInfo.bmiHeader;
				kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
				kInfoHeader.biWidth = unWidth * 8;
				kInfoHeader.biHeight = -((int)unWidth * 8);
				kInfoHeader.biPlanes = 1;
				kInfoHeader.biBitCount = 24;
				kInfoHeader.biCompression = BI_RGB;
				kInfoHeader.biSizeImage = 0;
				kInfoHeader.biXPelsPerMeter = 0;
				kInfoHeader.biYPelsPerMeter = 0;
				kInfoHeader.biClrUsed = 0;
				kInfoHeader.biClrImportant = 0;
				// Convert QrCode bits to bmp pixels
				pSourceData = pQRC->data;

				bmp = ::CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, (void**)&pRGBData, NULL, 0);

				for (y = 0; y < unWidth; y++)
				{
					pDestData = pRGBData + unWidthAdjusted * y * 8;
					for (x = 0; x < unWidth; x++)
					{
						if (*pSourceData & 1)
						{
							for (l = 0; l < 8; l++)
							{
								for (n = 0; n < 8; n++)
								{
									//以下三行是设置三基色，三基色都设置为0x00，则生成的二维码图片就是黑色的了，要什么颜色自己调整
									*(pDestData + n * 3 + unWidthAdjusted * l) = 0x00;
									*(pDestData + 1 + n * 3 + unWidthAdjusted * l) = 0x00;
									*(pDestData + 2 + n * 3 + unWidthAdjusted * l) = 0x00;
								}
							}
						}
						pDestData += 3 * 8;
						pSourceData++;
					}
				}
			}
		} while (false);
		if (pQRC) {
			QRcode_free(pQRC);
		}
		return bmp;
		////绑定dc
		//HDC dc=::CreateCompatibleDC(NULL);
		//::SelectObject(dc, bmp);
		////用完释放
		//::DeleteDC(dc);
		//::DeleteObject(bmp);
	}
	//生成二维码到内存
	inline bool Generate(const std::string& str, std::string& outFileData, QRecLevel level = QRecLevel::QR_ECLEVEL_M) {
		const char* szSourceSring = str.c_str();
		unsigned int unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
		byte* pSourceData, * pDestData;
		QRcode* pQRC = NULL;
		do
		{
			if (pQRC = QRcode_encodeString(szSourceSring, 0, level, QR_MODE_8, 1))
			{
				unWidth = pQRC->width;
				unWidthAdjusted = unWidth * 8 * 3;
				if (unWidthAdjusted % 4) {
					unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
				}
				unDataBytes = unWidthAdjusted * unWidth * 8;

				// Prepare bmp headers
				BITMAPFILEHEADER kFileHeader;
				kFileHeader.bfType = 0x4d42;
				kFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + unDataBytes;
				kFileHeader.bfReserved1 = 0;
				kFileHeader.bfReserved2 = 0;
				kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
				BITMAPINFOHEADER kInfoHeader;
				kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
				kInfoHeader.biWidth = unWidth * 8;
				kInfoHeader.biHeight = -((int)unWidth * 8);
				kInfoHeader.biPlanes = 1;
				kInfoHeader.biBitCount = 24;
				kInfoHeader.biCompression = BI_RGB;
				kInfoHeader.biSizeImage = 0;
				kInfoHeader.biXPelsPerMeter = 0;
				kInfoHeader.biYPelsPerMeter = 0;
				kInfoHeader.biClrUsed = 0;
				kInfoHeader.biClrImportant = 0;
				// Convert QrCode bits to bmp pixels
				pSourceData = pQRC->data;

				//因为是放在内存 所以简化了一些
				outFileData.resize(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + unDataBytes);
				byte* ptr = (byte*)outFileData.c_str();
				::memcpy(ptr, (char*)&kFileHeader, sizeof(kFileHeader));
				ptr += sizeof(kFileHeader);
				::memcpy(ptr, (char*)&kInfoHeader, sizeof(kInfoHeader));
				ptr += sizeof(kInfoHeader);
				byte* pRGBData = ptr;
				// Preset to white
				::memset(pRGBData, 0xff, unDataBytes);

				for (y = 0; y < unWidth; y++)
				{
					pDestData = pRGBData + unWidthAdjusted * y * 8;
					for (x = 0; x < unWidth; x++)
					{
						if (*pSourceData & 1)
						{
							for (l = 0; l < 8; l++)
							{
								for (n = 0; n < 8; n++)
								{
									//以下三行是设置三基色，三基色都设置为0x00，则生成的二维码图片就是黑色的了，要什么颜色自己调整
									*(pDestData + n * 3 + unWidthAdjusted * l) = 0x00;
									*(pDestData + 1 + n * 3 + unWidthAdjusted * l) = 0x00;
									*(pDestData + 2 + n * 3 + unWidthAdjusted * l) = 0x00;
								}
							}
						}
						pDestData += 3 * 8;
						pSourceData++;
					}
				}
			}
		} while (false);
		if (pQRC) {
			QRcode_free(pQRC);
		}
		return !outFileData.empty();
	}
};