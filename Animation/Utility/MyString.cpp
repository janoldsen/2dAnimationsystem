#include "MyString.h"

void eraseString(char* string, int start, int end)
{
	int length = strlen(string);
	for (int i = start; i < length; i++)
	{
		string[i] = string[i + end - start];
	}
}

void insertString(char* dst, int pos, char* src)
{
	int dstLength = strlen(dst);
	int srcLength = strlen(src);


	for (int i = dstLength + srcLength; i >= pos + srcLength; --i)
	{
		dst[i] = dst[i - srcLength];
	}
	memcpy(dst + pos, src, srcLength);
}