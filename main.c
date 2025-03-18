#include <stdio.h>

#include "dppt_searcher.h"
#include "hgss_searcher.h"
#include "input_tool.h"

int main(void) {
	printf("Which describes the version you are playing:\n");
	printf("1) D/P/Pt\n");
	printf("2) HG/SS\n");
	int version = inputReadInt("Enter the number:", 1, 2);
	
	printf("\n");
	
	printf("Which describes your two daycare parents:\n");
	printf("1) Same origin language\n");
	printf("2) Different origin language (Masuda method active)\n");
	int international = (inputReadInt("Enter the number:", 1, 2) == 2);
	
	printf("\n");
	
	printf("What is your target PID?\n");
	uint32_t target_pid = inputReadU32Hex("Enter PID as hex:");
	
	printf("\n");
	
	printf("Running for target PID: %08x\n", target_pid);
	
	if (version == 1) {
		dpptPidSearch(target_pid, international);
	} else {
		hgssPidSearch(target_pid, international);
	}
	
	printf("\n");
	
	printf("Press enter to exit\n");
	getchar();
	
	return 0;
}
