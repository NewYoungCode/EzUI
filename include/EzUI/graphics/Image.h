#pragma once
#include "core/UIDef.h"
#include "core/String.h"

#include "GraphicsTypes.h"
#include "Bitmap.h"

namespace ezui {
	namespace detail {
		//已预乘A通道BGRA的格式图像
		class EZUI_API D2DImage : public detail::IImage {
			friend class D2DGraphics;
			struct GifFrame
			{
				IWICBitmap* wicBitmap; // 存一份完整帧
				UINT delay;
			};
		protected:
			int m_width;
			int m_height;
			IWICBitmap* m_bitMap;//位图
			std::vector<GifFrame>* m_frames;

			IWICBitmap* m_cacheBitmap;//缓存的D2D bitmap
			SizeF m_cacheSize;//缓存的D2D bitmap尺寸
			ImageSizeMode m_cacheSizeMode;//缓存的D2D bitmap尺寸模式
#ifdef DEBUG
			//保存图片路径方便调试
			UIString m_path;
#endif
		private:
			void CreateFromStream(IStream* stream);
			void CreateFromFile(const std::wstring& file);
			void CreateFromHBITMAP(HBITMAP hBitmap);
			void Init(IWICBitmapDecoder* m_bitmapdecoder);
			void CopyFrames(UINT fCount, IWICBitmapDecoder* m_bitmapdecoder);
			HRESULT CreateD2DBitmap(ID2D1RenderTarget* render, ID2D1Bitmap** outD2DBitmap, const SizeF& targetSize);
		public:
			/// 图像可见性
			bool Visible;

			/// 从现有图像拷贝构造
			D2DImage(D2DImage* image);

			/// 从HBITMAP句柄创建图像(注意:颜色值需已预乘Alpha)
			D2DImage(HBITMAP hBitmap);

			/// 从流对象创建图像
			D2DImage(IStream* stream);

			/// 从Bitmap对象创建图像
			D2DImage(Bitmap* bitmap);

			/// 从本地文件创建图像
			D2DImage(const std::wstring& file);

			/// 创建指定尺寸的空白图像(带预乘Alpha的BGRA格式)
			D2DImage(int width, int height);

			/// 从内存数据创建图像
			D2DImage(const void* fileData, size_t fileSize);

			/// 获取底层WIC位图对象
			IWICBitmap* Get();

			/// 获取图像宽度
			int Width();

			/// 获取图像高度
			int Height();

			/// 切换到下一帧(GIF动画),返回帧延迟毫秒数
			virtual int NextFrame()EZUI_OVERRIDE;

			/// 保存图像到文件(支持png/jpg/bmp/gif/tiff格式)
			bool Save(const std::wstring& fileName);

			/// 克隆当前图像对象
			virtual D2DImage* Clone();

			/// 锁定位图并访问像素数据
			/// @param func 回调函数(参数:像素数据指针, 每行字节数)
			bool LockPixels(const std::function<void(uint8_t* pPixels, uint32_t bitmapStride)>& func);

			virtual ~D2DImage();
		public:
			/// 从资源文件或本地文件自动加载图像
			static D2DImage* Make(const UIString& fileOrRes);
		};
	};

	//EzUI图像类型 已预乘A通道BGRA的格式图像
	typedef detail::D2DImage Image;
};
