#include "init.h"

void GetLockCode(void)
{
	ST_CpuID = *(uint32_t*)(0x1ffff7e8);
}

