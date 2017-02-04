#include "stdafx.h"
#include "Base.h"

VOID GetAllPrivilege()
{
	VMProtectBegin(__FUNCTION__);
	for (USHORT i = 0; i < 0x100; i++)
	{
		BOOLEAN Old;
		RtlAdjustPrivilege(i, TRUE, FALSE, &Old);
	}
	VMProtectEnd();
}


