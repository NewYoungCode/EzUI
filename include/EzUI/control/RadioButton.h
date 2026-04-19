#pragma once
#include "CheckBox.h"

namespace ezui {
	//单选按钮 同一组中只能有一个被选中
	class EZUI_API RadioButton :
		public CheckBox
	{
	protected:
		virtual void OnMouseDown(MouseEventArgs* arg)override;
	public:
		RadioButton(Object* ownerObject = NULL);
		virtual~RadioButton();
	};
};