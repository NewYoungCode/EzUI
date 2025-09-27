#pragma once
#include "Control.h"
#include "Timer.h"

namespace ezui {
	class UI_EXPORT PictureBox : public Control {
	private:
		Timer m_timer;
		std::shared_ptr<std::atomic<bool>> m_alive;
	private:
		void Init();
	protected:
		virtual void OnForePaint(PaintEventArgs& arg)override;
	public:
		//图片(支持gif图自动播放)
		Image* Image = NULL;
		PictureBox(Object* ownerObject = NULL);
		virtual ~PictureBox();
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
	};
};
