#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include <winnt.h>

// Cache L3 is 6MB with 64 bytes per line = 98304 lines (0x18000)
// Visual Studio does not accept an array bigger than 2GB.
// Windows does not allow running a program that uses 2GB of data section
#define NB_ROW  0x4000		// square matrix with more rows than the L3 can accomodate
#define NB_COL  0x4000

char buf[NB_ROW][NB_COL];


// Avoid function being inlined to ease identification in disassembly view
__declspec(noinline) char testFnctByRow(char res)
{
	for (int row = 0; row < NB_ROW; row++)
	{
		for (int col = 0; col < NB_COL; col++)
		{
			res = (res ^ buf[row][col]) << 1;
		}
	}
	return res;
}

__declspec(noinline) char testFnctByCol(char res)
{
	for (int col = 0; col < NB_COL; col++)
	{
		for (int row = 0; row < NB_ROW; row++)
		{
			res = (res ^ buf[row][col]) << 1;
		}
	}
	return res;
}

__declspec(noinline) char testFnctWrByRow(char res)
{
	for (int row = 0; row < NB_ROW; row++)
	{
		for (int col = 0; col < NB_COL; col++)
		{
			buf[row][col] = res++;
		}
	}
	return res;
}

__declspec(noinline) char testFnctWrByCol(char res)
{
	for (int col = 0; col < NB_COL; col++)
	{
		for (int row = 0; row < NB_ROW; row++)
		{
			buf[row][col] = res++;
		}
	}
	return res;
}

int main(int argc, const char *argv[])
{
	int affMask = 4;
	int byCol = 0;
	int runWrite = 0;
	int nbIter = 5;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--mask") == 0)					affMask = strtol(argv[++i], NULL, 0);
		else if (strcmp(argv[i], "--byCol") == 0)			byCol = 1;
		else if (strcmp(argv[i], "--wr") == 0)				runWrite = 1;
		else if (strcmp(argv[i], "--iter") == 0)			nbIter = strtol(argv[++i], NULL, 0);
		else
		{
			printf("Unknown argument %s\n", argv[i]);
			printf("Valid arguments:\n");
			printf("\t--mask <mask>\n");
			printf("\t--iter <nbIter>\n");
			printf("\t--byCol\n");
			printf("\t--wr\n");
			exit(1);
		}
	}

	printf("Using %d rows, %d columns\n", NB_ROW, NB_COL);

	memset(buf, 0, sizeof(buf));		// Make sure memory is no more virtual

	// Make this executable the highest possible priority
	BOOL errBool = SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	assert(errBool != 0);
	HANDLE hdl = GetCurrentThread();
	errBool = SetThreadPriority(hdl, THREAD_PRIORITY_TIME_CRITICAL);
	assert(errBool != 0);
	DWORD_PTR errAffMsk = SetThreadAffinityMask(hdl, affMask);		// Set which CPU to use
	assert(errAffMsk != 0);

	while (nbIter--)
	{
		char res = 0;
		DWORD entryTime = GetTickCount();
		if (runWrite)
		{
			if (byCol)			res = testFnctWrByCol(res);
			else                res = testFnctWrByRow(res);
		}
		else
		{
			if (byCol)			res = testFnctByCol(res);
			else                res = testFnctByRow(res);
		}

		DWORD exitTime = GetTickCount();
		printf("%d ms (%d)\n", exitTime - entryTime, res);
	}

	return 0;
}
