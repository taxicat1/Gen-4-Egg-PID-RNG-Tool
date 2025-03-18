#include "mt_tool.h"
#include "pid_tool.h"

#include <stdio.h>
#include <stdint.h>


typedef struct {
	unsigned int seed        :32;
	unsigned int coin_flips  :5;
	unsigned int double_taps :7;
	float effort;
} dppt_manip_details;


static void calcManipEffort(dppt_manip_details* manip, int delay) {
	double effort = 0.0;
	
	// Assumption: it takes you ~4 attempts on average to hit a seed
	double seed_attempts = 4.0;
	
	// Must always wait the full delay to hit a seed
	double delay_per_second = 60.0;
	double delay_effort_weight = 0.8; // Arbitrary: waiting for delay is passive time, lower effort cost than doing flips or taps
	double delay_seconds = (double)delay / delay_per_second;
	effort += delay_seconds * seed_attempts * delay_effort_weight;
	
	// Must use ~10 flips to verify the seed
	double coin_flip_seconds = 2.8;
	double verify_seconds = 10 * coin_flip_seconds;
	effort += verify_seconds * seed_attempts;
	
	// Remaining coin flips
	if (manip->coin_flips > 10) {
		effort += (manip->coin_flips - 10) * coin_flip_seconds;
	}
	
	// Double taps
	double double_tap_seconds = 1.4;
	effort += manip->double_taps * double_tap_seconds;
	
	manip->effort = effort;
}

static void printManip(const dppt_manip_details* manip) {
	printf("Initial seed:       %08x\n", manip->seed);
	printf("Coin flips:         %u\n",   manip->coin_flips);
	
	switch (manip->double_taps) {
		case 0:
			printf("Happiness checker:  %s\n", "Avoid");
			break;
		
		case 1:
			printf("Happiness checker:  %s\n", "Open");
			break;
		
		default:
			printf("Happiness checker:  %u\n", manip->double_taps - 1);
			break;
	}
}

static void printInstructions() {
	printf("Manip instructions:\n");
	printf(" - Hit the initial seed using any Gen 4 RNG reporter\n");
	printf(" - Verify the seed using the specified number of coin flips\n");
	printf(" - Double-tap the Friendship Checker the specified number of times\n");
	printf("   (or, switch to it and immediately switch away, or avoid it entirely)\n");
	printf(" - Bike around until an egg is generated\n");
}

int dpptPidSearch(uint32_t target_pid, int international) {
	printf("Searching, please wait!\n\n\n");
	fflush(stdout);
	
	// Minimum 10 for seed verification, max equal to one extra double tap
	int min_coin_flips  = 10;
	int max_coin_flips  = 22;
	
	// Arbitrary
	int max_double_taps = 40;
	
	// At each middle stage, quit if we found a result. Only keep searching if we have nothing
	int delay_steps[4] = { 600, 7000, 12000, 24000 };
	
	dppt_manip_details match_found = { 0, 0, 0, 0.0 };
	
	uint32_t target_mtout = pidPrepare(target_pid, international);
	
	uint32_t* mt = mtGetBuf();
	dppt_manip_details this_manip;
	for (uint32_t vsync_year = delay_steps[0]; vsync_year < delay_steps[3]; vsync_year++) {
		for (uint32_t hour = 0x00000000; hour < (24*0x00010000); hour += 0x00010000) {
			for (uint32_t month_day_minute_second = 0x01000000; month_day_minute_second != 0x00000000; month_day_minute_second += 0x01000000) {
				
				this_manip.seed = month_day_minute_second + hour + vsync_year;
				mtSrand(this_manip.seed);
				
				for (this_manip.double_taps = 0; this_manip.double_taps < max_double_taps; this_manip.double_taps++) {
					for (this_manip.coin_flips = min_coin_flips; this_manip.coin_flips < max_coin_flips; this_manip.coin_flips++) {
						
						int frame = this_manip.coin_flips + (this_manip.double_taps * 12);
						if (frame >= mtLen) {
							break;
						}
						
						if (mt[frame] == target_mtout) {
							// Fill in the effort field
							calcManipEffort(&this_manip, vsync_year);
							
							// If this is better than previous best, or there was no previous best (0.0 effort), then overwrite
							if (match_found.effort == 0.0f || this_manip.effort < match_found.effort) {
								match_found = this_manip;
							}
						}
					}
				}
			}
		}
		
		// Quit early if we have a match within the return delay bound
		if (vsync_year == delay_steps[1] && match_found.effort != 0.0) {
			break;
		}
		
		if (vsync_year == delay_steps[2] && match_found.effort != 0.0) {
			break;
		}
	}
	
	
	if (match_found.effort == 0.0) {
		printf("COULD NOT FIND THIS PID!\n\n");
		return 1;
	}
	
	printManip(&match_found);
	printf("\n");
	printInstructions();
	
	return 0;
}
