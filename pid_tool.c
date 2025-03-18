#include "pid_tool.h"
#include "mt_tool.h"

static uint32_t arngInv(uint32_t x) {
	return (x * 2520285293) + 1774682003;
}

uint32_t pidPrepare(uint32_t pid, int international) {
	uint32_t target_mtout = pid;
	
	// Masuda rerolls
	if (international) {
		target_mtout = arngInv(arngInv(arngInv(arngInv(target_mtout))));
	}
	
	target_mtout = mtUntemper(target_mtout);
	
	return target_mtout;
}
