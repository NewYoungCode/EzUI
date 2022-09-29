#include "TileList.h"

namespace EzUI {

	void TileList::Clear(bool freeChilds)
	{
		__super::Clear(freeChilds);
		LocationY.clear();
		_MaxBottom = 0;
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_MaxBottom);
		}
	}

	TileList::TileList()
	{
		this->ScrollBar = new VScrollBar;
		vScrollBar = (VScrollBar*)this->ScrollBar;
		if (vScrollBar) {
			vScrollBar->SetHeight(Height());
			vScrollBar->Parent = this;
			vScrollBar->_controlsLocationY = &LocationY;
		}
	}
	TileList::~TileList()
	{
	}
	bool TileList::OnSize(const Size& sz)
	{
		if (__super::OnSize(sz)) {
			ResumeLayout();
			if (vScrollBar) {
				vScrollBar->SetMaxBottom(_MaxBottom);
			}
			return true;
		}
		return false;
		
	}
	void TileList::OnChildPaint(Controls& controls, PaintEventArgs& args) {
		VisibleControls.clear();
		auto rect = Rect(0, 0, _rect.Width, _rect.Height);
		//�����ӿؼ�
		for (auto i = controls.begin(); i != controls.end(); i++)
		{
			auto& it = **i;
			if (rect.IntersectsWith(it.GetRect())) {
				VisibleControls.push_back(*i);
				it.Rending(args);
			}
			if (it.Y() >= _rect.Height) { //�����б�ؼ��������ٻ��ƺ���Ŀؼ� �Ż�������������
				break;
			}
		}
		//�ӿؼ��������
	}
	void TileList::ResumeLayout()
	{
		if (_rect.IsEmptyArea()) return;
		LocationY.clear();
		int _right = MarginRight;//�ұ߾�
		int _bottom = MarginTop;//�ϱ߾�
		int maxHeight = 0;//ÿ����ߵ��Ǹ�
		int _lineCount = 0;//һ���м���
		int _hCount = 0;//����һ�з��˼���
		_MaxBottom = 0;//����������
		ControlIterator itor = _controls.begin();
		for (; itor != _controls.end(); )
		{
			Control& it = **itor;
			if (_right + it.Width() > Width()) {//����
				if (_hCount > 0) {
					_hCount = 0;
					continue;
				}
				_right = MarginRight;
				_bottom += maxHeight + MarginTop;
				maxHeight = 0;
				_lineCount++;
			}
			it.SetLocation({ _right,_bottom });
			LocationY.insert(std::pair<Control*, int>(&it, it.Y()));
			_right += it.Width() + MarginRight;
			if (maxHeight < it.Height()) {
				maxHeight = it.Height();
			}
			_hCount++;
			itor++;
			_MaxBottom = maxHeight + it.Y();
		}

		_MaxBottom += MarginTop;

		this->PendLayout = false;
	}

};