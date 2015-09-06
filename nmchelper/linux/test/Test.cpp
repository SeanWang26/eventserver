#include <stdio.h>

int NMC_CALL_TYPE static_NmcStatusCallback(long handle, int type, void* data, int datalen, void *userdata)
{
	printf("static_NmcStatusCallback\n");	
	return 0;
}

int main(int argc, char** argv)
{
	nmc_init(static_NmcStatusCallback, this);
	
	
	
	return 0;
}




















