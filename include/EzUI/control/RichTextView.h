#pragma once
#include "VLayout.h"
#include "HLayout.h"

namespace ezui {
	//富文本显示控件
	class EZUI_API RichTextView :
		public VLayout
	{
	private:
		// 解析XML元素并添加到父控件中
		void ParseElement(void* element, Control* parentControl);
	public:
		RichTextView(Object* ownerObject = NULL);

		//设置富文本内容(支持简单的HTML标签,如<p>、<a>、<span>等)
		void SetText(const UIString& htmlText);

		virtual ~RichTextView();
	};
};