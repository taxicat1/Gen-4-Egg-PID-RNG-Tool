#include "mt_tool.h"
#include "voltorb_flip_tool.h"
#include "pid_tool.h"

#include <stdio.h>
#include <stdint.h>


typedef struct {
	unsigned int seed          :32;
	unsigned int voltorb_flips :5;
	unsigned int rematch_calls :5;
	unsigned int extra_egg     :2;
	float effort;
} hgss_manip_details;


static void calcManipEffort(hgss_manip_details* manip, int delay) {
	double effort = 0.0;
	
	// Assumption: it takes you ~4 attempts on average to hit a seed
	double seed_attempts = 4.0;
	
	// Must always wait the full delay to hit a seed
	double delay_per_second = 60.0;
	double delay_effort_weight = 0.8; // Arbitrary: waiting for delay is passive time, lower effort cost than doing flips or calls
	double delay_seconds = (double)delay / delay_per_second;
	effort += delay_seconds * seed_attempts * delay_effort_weight;
	
	double voltorb_seconds = 5.4;
	effort += manip->voltorb_flips * voltorb_seconds;
	
	double call_seconds = 7.7;
	effort += manip->rematch_calls * call_seconds;
	
	double extra_egg_seconds = 10.0;
	if (manip->extra_egg) {
		effort += extra_egg_seconds;
	}
	
	manip->effort = effort;
}

static void printManip(const hgss_manip_details* manip) {
	const char* extra_egg_result[] = { "No", "Yes" };
	
	printf("Initial seed:      %08x\n", manip->seed);
	printf("Voltorb Flips:     %u\n",   manip->voltorb_flips);
	printf("Rematch calls:     %u\n",   manip->rematch_calls);
	printf("Reject first egg:  %s\n",   extra_egg_result[ manip->extra_egg ]);
}

static void printInstructions() {
	printf("Manip instructions:\n");
	printf(" - Hit the initial seed using any Gen 4 RNG reporter\n");
	printf(" - Verify the seed using any number of Elm/Irwin calls\n");
	printf(" - Open a Voltorb Flip game and immediately quit the specified number of times\n");
	printf(" - Call any rematchable trainer that is not currently accepting a rematch\n");
	printf("   the specified number of times (Youngster Joey is the usual pick)\n");
	printf(" - Bike around until an egg is generated\n");
	printf(" - If 'Reject first egg' is 'Yes', reject the first egg, bike around\n");
	printf("   until another one is generated, and accept the second one\n");
}

int hgssPidSearch(uint32_t target_pid, int international) {
	printf("Searching, please wait!\n\n\n");
	fflush(stdout);
	
	// Arbitrary
	int max_voltorb_flips = 35;
	int max_rematch_calls = 25;
	
	// At each middle stage, quit if we found a result. Only keep searching if we have nothing
	int delay_steps[4] = { 600, 7000, 12000, 24000 };
	
	hgss_manip_details match_found = { 0, 0, 0, 0, 0.0 };
	
	uint32_t target_mtout = pidPrepare(target_pid, international);
	if (target_mtout == 0x00000000) {
		printf("This PID is invalid!\n");
		printf("An initial egg PID of 00000000 is interpreted by the game as no egg available\n");
		return 1;
	}
	
	uint32_t* mt = mtGetBuf();
	hgss_manip_details this_manip;
	for (uint32_t vsync_year = delay_steps[0]; vsync_year < delay_steps[3]; vsync_year++) {
		for (uint32_t hour = 0x00000000; hour < (24*0x00010000); hour += 0x00010000) {
			for (uint32_t month_day_minute_second = 0x01000000; month_day_minute_second != 0x00000000; month_day_minute_second += 0x01000000) {
				
				this_manip.seed = month_day_minute_second + hour + vsync_year;
				mtSrand(this_manip.seed);
				
				// Look if the target even exists in the upcoming MT outputs
				// The voltorb flip thing is way slower than this sequential search
				int target_found = 0;
				for (int i = 0; i < mtLen; i++) {
					if (mt[i] == target_mtout) {
						target_found = 1;
						break;
					}
				}
				
				if (!target_found) {
					continue;
				}
				
				// Now check if we can actually hit this target with voltorb flipping and Joey calling
				// Note: We don't save the idx of the target and then try to hit it, because it could happen that multiple
				//       idxes within the mt buffer all contain the target. This is unlikely, but I don't want to skip this case
				int voltorb_frame = 0;
				for (this_manip.voltorb_flips = 0; this_manip.voltorb_flips < max_voltorb_flips; this_manip.voltorb_flips++) {
					for (this_manip.rematch_calls = 0; this_manip.rematch_calls < max_rematch_calls; this_manip.rematch_calls++) {
						for (this_manip.extra_egg = 0; this_manip.extra_egg <= 1; this_manip.extra_egg++) {
							
							int frame = voltorb_frame + (this_manip.rematch_calls * 2) + this_manip.extra_egg;
							if (frame >= mtLen) {
								goto multiLoopBreak;
							}
							
							if (mt[frame] == target_mtout) {
								// Fill in the effort field
								calcManipEffort(&this_manip, vsync_year);
								
								// If this is better than previous best, or there was no previous best (0.0 effort), then overwrite
								if (match_found.effort == 0.0 || this_manip.effort < match_found.effort) {
									match_found = this_manip;
								}
							}
						}
					}
					
					// Next voltorb flip advances
					voltorb_frame = vfGetNext(voltorb_frame);
					if (voltorb_frame == -1) {
						break;
					}
				}
				
			multiLoopBreak:
				(void)0;
			}
		}
		
		// Quit early if we have a match within the return delay bound
		if (vsync_year == delay_steps[1] && match_found.effort != 0.0) {
			break;
		}
		
		if (vsync_year >= delay_steps[2] && match_found.effort != 0.0) {
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
