#include "mt_tool.h"

#define N  624		
#define M  397

const unsigned int mtLen = N;

#define UPPER_MASK  0x80000000
#define LOWER_MASK  0x7FFFFFFF

static uint32_t mt[N];
static uint32_t mag01[2] = { 0, 0x9908B0DF };

static void mtScramble() {
	int kk = 0;
	while (kk < N-M) {
		uint32_t y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
		mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 1];
		kk++;
	}
	
	while (kk < N-1) {
		uint32_t y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
		mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 1];
		kk++;
	}
	
	uint32_t y = (mt[N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
	mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 1];
}

void mtSrand(uint32_t s) {
	mt[0]= s;
	for (int i = 1; i < N; i++) {
		mt[i] = (0x6C078965 * (mt[i-1] ^ (mt[i-1] >> 30)) + i); 
	}
	
	mtScramble();
}

uint32_t mtTemper(uint32_t y) {
	y ^= (y >> 11);
	y ^= (y <<  7) & 0x9D2C5680;
	y ^= (y << 15) & 0xEFC60000;
	y ^= (y >> 18);
	return y;
}

static uint32_t unshr(uint32_t x, int shift) {
	uint32_t r = x;
	for (int i = 0; i < 32; i++) {
		r = x ^ (r >> shift);
	}
	return r;
}

static uint32_t unshl(uint32_t x, int shift, uint32_t mask) {
	uint32_t r = x;
	for (int i = 0; i < 32; i++) {
		r = x ^ ((r << shift) & mask);
	}
	return r;
}

uint32_t mtUntemper(uint32_t v) {
	v = unshr(v, 18);
	v = unshl(v, 15, 0xEFC60000);
	v = unshl(v, 7,  0x9D2C5680);
	v = unshr(v, 11);
	return v;
}

uint32_t* mtGetBuf() {
	return &mt[0];
}
