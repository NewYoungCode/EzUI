#pragma once
#include "Control.h"
#include "core/Timer.h"

namespace ezui {
	//图片显示控件(支持gif图自动播放)
	class EZUI_API PictureBox : public Control {
	private:
		Timer* m_timer;
		//图片(支持gif图自动播放)
		Image* m_image = NULL;
	private:
		void Init();
	protected:
		virtual void OnForePaint(PaintEventArgs* args)override;
	public:
		PictureBox(Object* ownerObject = NULL);

		//设置图片
		// attach = true 表示将图片的生命周期绑定到当前控件(控件销毁时会自动销毁图片)
		void SetImage(Image* image, bool attach = false);

		//获取图片
		Image* GetImage()const;

		// Supported attributes:
		// src: image file path or resource
		// All Control attributes are also supported
		virtual void SetAttribute(const UIString& key, const UIString& value)override;

		virtual ~PictureBox();
	};
};
