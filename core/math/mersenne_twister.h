#ifndef MATH_MERSENNE_TWISTER_H_
#define MATH_MERSENNE_TWISTER_H_

#include <stdint.h>

#ifdef BUILD_IOT
namespace mn {
	// https://en.wikipedia.org/wiki/Mersenne_Twister
	class MTRandom {
	public:
		MTRandom();

		MTRandom(uint32_t seed);

		~MTRandom();

		uint32_t Next();

		float NextFloat(float limit = 1);

	private:
		void Init();

		void Twist();

	private:
		uint32_t _seed;

		uint32_t* _mt = nullptr;
		int _index = 0;

	private:
		MTRandom(const MTRandom&) = delete;
		const MTRandom& operator=(const MTRandom&) = delete;
	};
}

#endif

#endif // !MATH_MERSENNE_TWISTER_H_