#include "Spacer.h"
#include "Control.h"
namespace ezui {

	Spacer::Spacer() {
		this->SetHitTestVisible(false);
	};
	Spacer::~Spacer() {};

	VSpacer::VSpacer() {};
	VSpacer::~VSpacer() {};
	VSpacer::VSpacer(int fixedHeight) {
		SetFixedHeight(fixedHeight);
	}

	HSpacer::HSpacer() {};
	HSpacer::~HSpacer() {};
	HSpacer::HSpacer(int fixedWidth) {
		SetFixedWidth(fixedWidth);
	}
};