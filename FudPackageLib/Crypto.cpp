#include "stdafx.h"
#include "Base.h"

void CryptoFunction(UCHAR* key, int enc, UCHAR* buf, int buflen)
{
	int index;
	// key size is always equal to 8
	for (index = 0; index < buflen; index++)
	{
		if (enc == 1)
			buf[index] = buf[index] ^ index;

		buf[index] = buf[index] ^ key[index % 8];
		if (enc == 0)
			buf[index] = buf[index] ^ index;
	}
}