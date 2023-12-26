#include "mersenne_twister.h"
#include <ctime>
#include <string.h>
#include <stdio.h>

#ifdef BUILD_IOT
namespace mn {
	// MT19937 - 32
	static const uint32_t MT_N = 624;
	static const uint32_t MT_M = 397;
	static const uint32_t MT_R = 31;

	static const uint32_t MT_A = 0x9908B0DF;
	static const uint32_t MT_F = 1812433253;

	static const uint32_t MT_U = 11;
	static const uint32_t MT_D = 0xFFFFFFFF;

	static const uint32_t MT_S = 7;
	static const uint32_t MT_B = 0x9D2C5680;

	static const uint32_t MT_T = 15;
	static const uint32_t MT_C = 0xEFC60000;

	static const uint32_t MT_L = 18;

	static const uint32_t MT_LOWER_MASK = 0x7FFFFFFF;
	static const uint32_t MT_UPPER_MASK = 0x80000000;

	static uint32_t S_Multiple(uint64_t a, uint64_t b) {
		uint64_t c = a * b;
		return c & 0xFFFFFFFF;
	}

	static uint32_t S_Add(uint64_t a, uint64_t b) {
		uint64_t c = a + b;
		return c & 0xFFFFFFFF;
	}

	MTRandom::MTRandom() {
		std::time_t t = std::time(0);
		_seed = t & 0xffffffff;

		Init();
	}

	MTRandom::MTRandom(uint32_t seed) : _seed(seed) {
		Init();
	}

	MTRandom::~MTRandom() {
		delete[] _mt;
	}

	uint32_t MTRandom::Next() {
		if (_index >= MT_N) {
			Twist();
		}
		uint64_t y = _mt[_index];
		y = y ^ ((y >> MT_U) & MT_D);
		y = y ^ ((y << MT_S) & MT_B);
		y = y ^ ((y << MT_T) & MT_C);
		y = y ^ (y >> MT_L);

		++_index;

		return y & 0xFFFFFFFF;
	}

	float MTRandom::NextFloat(float limit) {
		double temp = Next();
		double n = 0xFFFFFFFF;
		temp = temp / n;
		return float(temp);
	}
	
	void MTRandom::Init() {
		_mt = new uint32_t[MT_N];
		_index = MT_N;

		_mt[0] = _seed;
		for (int i = 1; i < MT_N; i++) {
			uint64_t temp = S_Multiple(MT_F, _mt[i - 1] ^ (_mt[i - 1] >> 30));
			temp += i;
			_mt[i] = temp & 0xFFFFFFFF;
		}
	}
	
	void MTRandom::Twist() {
		for (int i = 0; i < MT_N; i++) {
			uint32_t x = S_Add(_mt[i] & MT_UPPER_MASK, _mt[(i + 1) % MT_N] & MT_LOWER_MASK);
			uint32_t xa = x >> 1;
			if ((x % 2) != 0) {
				xa = xa ^ MT_A;
			}
			_mt[i] = _mt[(i + MT_M) % MT_N] ^ xa;
		}
		_index = 0;
	}
}

#endif
