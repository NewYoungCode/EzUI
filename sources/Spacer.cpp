#include "Spacer.h"
#include "Control.h"
namespace ezui {

	Spacer::Spacer() {
		this->SetHitTestVisible(false);
	}
	bool Spacer::IsSpacer()
	{
		return true;
	}
	Spacer::~Spacer() {};

	VSpacer::~VSpacer() {};
	VSpacer::VSpacer(int fixedHeight) {
		SetFixedHeight(fixedHeight);
	}

	HSpacer::~HSpacer() {};
	HSpacer::HSpacer(int fixedWidth) {
		SetFixedWidth(fixedWidth);
	}
};