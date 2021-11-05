#include "TileLayout.h"

void TileLayout::Sort()
{
	if (_rect.IsEmptyArea()) return;
	_controlsLocationY.clear();
	int _right = MarginRight;//�ұ߾�
	int _bottom = MarginTop;//�ϱ߾�
	int maxHeight = 0;//ÿ����ߵ��Ǹ�
	int _lineCount = 0;//һ���м���
	int _hCount = 0;//����һ�з��˼���
	_MaxBottom = 0;//����������
	std::list<Control*>::iterator itor = _controls.begin();
	for (; itor != _controls.end(); )
	{
		Control &it = **itor;
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
		it.OnLayout({ Width(),Height() },false);//����ԭʼ��������
		it.Move({ _right,_bottom });
		_controlsLocationY.insert(std::pair<Control*, int>(&it, it.Y()));
		_right += it.Width() + MarginRight;
		if (maxHeight < it.Height()) {
			maxHeight = it.Height();
		}
		_hCount++;
		itor++;
		_MaxBottom = maxHeight + it.Y();
	}

	_MaxBottom += MarginTop;
}

TileLayout::TileLayout()
{
	this->ScrollBar = new VScrollBar;
	verticalScrollBar = (VScrollBar*)this->ScrollBar;
	if (verticalScrollBar) {
		verticalScrollBar->ReSize({ 10,Height() });//���������
		verticalScrollBar->Parent = this;
		verticalScrollBar->_controlsLocationY = &_controlsLocationY;
	}
}

void TileLayout::AddControl(Control * ctl)
{
	__super::AddControl(ctl);
	Sort();
	if (verticalScrollBar) {
		verticalScrollBar->SetMaxBottom(_MaxBottom);
	}
}

void TileLayout::OnSize(const Size & sz)
{
	Sort();
	if (verticalScrollBar) {
		verticalScrollBar->OnLayout(sz);
		verticalScrollBar->SetMaxBottom(_MaxBottom);
	}
}

void TileLayout::OnChildPaint(Controls &controls, PaintEventArgs &args) {
	VisibleControls.clear();
	auto rect = Rect(0, 0, _rect.Width, _rect.Height);
	//�����ӿؼ�
	for (auto i = controls.begin(); i != controls.end(); i++)
	{
		auto &it = **i;
		if (rect.IntersectsWith(it.GetRect())) {
			VisibleControls.push_back(*i);
			it.OnEvent(Event::OnPaint, &args);
		}
		if (it.Y() >= _rect.Height) { //�����б�ؼ��������ٻ��ƺ���Ŀؼ� �Ż�������������
			break;
		}
	}
	//�ӿؼ��������
}

void TileLayout::RefreshLayout()
{
	Sort();
	Refresh();
}
