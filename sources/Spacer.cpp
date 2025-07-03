#include "Spacer.h"
#include "Control.h"
namespace ezui {

	Spacer::~Spacer() {};

	VSpacer::VSpacer() {};
	VSpacer::~VSpacer() {};
	VSpacer::VSpacer(int_t fixedHeight) {
		SetFixedHeight(fixedHeight);
	}

	HSpacer::HSpacer() {};
	HSpacer::~HSpacer() {};
	HSpacer::HSpacer(int_t fixedWidth) {
		SetFixedWidth(fixedWidth);
	}
};