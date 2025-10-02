#pragma once
#include <random>
class Random {
	std::mt19937 randomer;
public:
	Random() :randomer(std::random_device()()) {
	}
	int Next(int lower, int upper) {
		std::uniform_int_distribution<int> distribution(lower, upper);
		return distribution(randomer);
	}
	float Next(float lower, float upper) {
		std::uniform_real_distribution<float> distribution(lower, upper);
		return distribution(randomer);
	}
};
