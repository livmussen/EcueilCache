#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

// Avoid function being inlined to ease identification in disassembly view
__declspec(noinline) void testWrite(volatile uint64_t* addr, volatile uint64_t* dummy)
{
	uint64_t nb = 0;

	while (1)
	{
		*addr = nb++;
		*dummy += nb;		// Add extra processing to let previous write go deep enough in CPU write buffer
							// without this line, the cache collision phenomenon seems hidden.
	}
}

__declspec(noinline) void testReadWrite(volatile uint64_t* rdAddr, volatile uint64_t* wrAddr)
{
	uint64_t nb = 0;

	while (1)
	{
		nb += *rdAddr;		// should read value 0
		*wrAddr = nb++;
	}
}

HANDLE startEvent = NULL;		// Event to synchronize the test starting point amonst all threads
uint64_t* buf = NULL;

int threadWrEntryFnct(void* param)
{
	uint64_t dummy = 0;
	int offset = (int)(size_t)param;
	printf("Dummy = 0x%p\n", &dummy);
	WaitForSingleObject(startEvent, INFINITE);	// All threads starts at the same time here
	testWrite(buf + offset, &dummy);
	return 0;
}

int threadRdWrEntryFnct(void* param)
{
	int offset = (int)(size_t)param;
	WaitForSingleObject(startEvent, INFINITE);	// All threads starts at the same time here
	testReadWrite(buf, buf + offset);
	return 0;
}

#define MAX_THREADS		20
struct
{
	int offset;
	DWORD threadId;
	int affinity;
} threadList[MAX_THREADS];

int main(int argc, const char* argv[])
{
	int affMask = 1;
	int dataOffset = 0;
	int worstOffset = 0;
	int readWrite = 0;		// if not 0, we also do read operation
	int listIx = 0;			// index in threadList

	// Make this executable the highest possible priority
	BOOL errBool = SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	assert(errBool != 0);
	errBool = SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	assert(errBool != 0);
	DWORD_PTR errAffMsk = SetThreadAffinityMask(GetCurrentThread(), affMask);
	assert(errAffMsk != 0);

	startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// Manual reset to unblock all threads at the same time

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--rw") == 0)					readWrite = 1;
		else if (strcmp(argv[i], "--mask") == 0)			affMask = strtol(argv[++i], NULL, 0);
		else if (strcmp(argv[i], "--off") == 0)
		{
			dataOffset = strtol(argv[++i], NULL, 0);
			if (dataOffset > worstOffset)	worstOffset = dataOffset;

			DWORD tid;
			HANDLE hdl;
			if (readWrite)
			{
				hdl = CreateThread(NULL, 1000 /*stack size*/, threadRdWrEntryFnct, (void*)(size_t)dataOffset, 0, &tid);
			}
			else
			{
				hdl = CreateThread(NULL, 1000 /*stack size*/, threadWrEntryFnct, (void*)(size_t)dataOffset, 0, &tid);
			}
			assert(hdl != NULL);

			errBool = SetThreadPriority(hdl, THREAD_PRIORITY_TIME_CRITICAL);
			assert(errBool != 0);

			DWORD_PTR errAffMsk = SetThreadAffinityMask(hdl, affMask);
			assert(errAffMsk != 0);

			threadList[listIx].threadId = tid;
			threadList[listIx].offset = dataOffset;
			threadList[listIx].affinity = affMask;
			printf("Launched TID %d with offset %d, affinity = 0x%x\n", threadList[listIx].threadId,
				threadList[listIx].offset,
				threadList[listIx].affinity);
			listIx++;
			assert(listIx < MAX_THREADS);
		}
		else
		{
			printf("Unknown argument %s\n", argv[i]);
			printf("Valid arguments:\n");
			printf("\t--rw\n");
			printf("\t--mask <mask>\n");
			printf("\t--off <wrOffset>\n");
			exit(1);
		}
	}
	buf = (uint64_t*)malloc((worstOffset + 1) * sizeof(uint64_t));
	assert(buf != NULL);
	assert((uint64_t)buf % sizeof(uint64_t) == 0);				// Make sure memory is properly aligned
	memset(buf, 0, (worstOffset + 1) * sizeof(uint64_t));		// Make sure memory is no more virtual

	uint64_t prevCnt[MAX_THREADS] = { 0 };
	uint64_t cnt[MAX_THREADS] = { 0 };
	int lastTime = GetTickCount();

	SetEvent(startEvent);								// Launch test

	for (int maxTime = 0; maxTime < 3; maxTime++)		// Avoid infinite execution in case all threads taking 100% CPU
	{
		Sleep(2000);

		for (int i = 0; i < listIx; i++)
		{
			cnt[i] = buf[threadList[i].offset];	// take last number written by all threads
		}
		int curTime = GetTickCount();			// This function calls OS. Call it after data fetched
		uint64_t sum = 0;
		for (int i = 0; i < listIx; i++)
		{
			uint64_t diff = cnt[i] - prevCnt[i];	// Keep data during the last test loop
			printf("%4d TID %5d, offset %3d (0x%p), affinity = 0x%03x: val = %llu,%03llu,%03llu,%03llu\n", curTime - lastTime,
				threadList[i].threadId, threadList[i].offset, &(buf[threadList[i].offset]), threadList[i].affinity,
				diff / 1000000000, (diff / 1000000) % 1000, (diff / 1000) % 1000, (diff % 1000));
			prevCnt[i] = cnt[i];
			sum += diff;
		}
		lastTime = curTime;
		printf("\t\t\tsum:  %llu,%03llu,%03llu,%03llu\n", sum / 1000000000, (sum / 1000000) % 1000, (sum / 1000) % 1000, (sum % 1000));
	}

	return 0;
}
