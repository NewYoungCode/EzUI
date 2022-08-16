#include "Layout.h"

namespace EzUI {

	Layout::Layout()
	{

	}

	Layout::~Layout()
	{
	}

	void Layout::Sort()
	{
	}

	void Layout::RefreshLayout()
	{
		Sort();
		Refresh();
	}
};