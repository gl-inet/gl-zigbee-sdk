#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "lib/libglzbapi.h"
#include "glzb_type.h"
#include "glzb_errno.h"

int main()
{

	glzb_init();

	glzb_nwk_status_para_s status;
	memset(&status, 0, sizeof(glzb_nwk_status_para_s));

	GL_RET ret = glzb_get_nwk_status(&status);
	if(ret != GL_SUCCESS)
	{
		printf("error ret: %d\n");
		return -1;
	}

	printf("{\n");
	bool joined_newk = false;
	printf("  NWK Status: ");
	switch (status.nwk_status)
	{
		case 0:
			printf("no network!\n");
			break;
		case 1:
			printf("joining network!\n");
			break;
		case 2:
			joined_newk = true;
			printf("joined network!\n");
			break;
		case 3:
			printf("joined network no parent!\n");
			break;
		case 4:
			printf("leaving network!\n");
			break;
	}
	if(!joined_newk)
	{
		printf("}\n");
		return 0;
	}

	printf("  Node Type: ");
	switch (status.node_type)
	{
		case 0:
			printf("Unknow Device\n");
			break;
		case 1:
			printf("Coordinator\n");
			break;
		case 2:
			printf("Router\n");
			break;
		case 3:
			printf("End Device\n");
			break;
		case 4:
			printf("Sleep End Device\n");
			break;
	}

	printf("  Extended PAN ID: %s\n", status.extended_pan_id);

	printf("  PAN ID: 0x%04x\n", status.pan_id);

	printf("  Tx Power: %d\n", status.radio_tx_power);

	printf("  Radio Channel: %d\n", status.radio_channel);

	// printf("  Channels: %d\n", status.channels);

	printf("  Join Method: ");
	switch (status.node_type)
	{
		case 0:
			printf("Use MAC association\n");
			break;
		case 1:
			printf("Use NWK rejoin\n");
			break;
		case 2:
			printf("Use NWK rejoin have NWK key\n");
			break;
		case 3:
			printf("Use configured NWK state\n");
			break;
	}

	printf("  NWK Manager ID: 0x%04x\n", status.nwk_manager_id);

	printf("  NWK Update ID: 0x%02x\n", status.nwk_update_id);

	printf("}\n");

	glzb_free();

	return 0;
}