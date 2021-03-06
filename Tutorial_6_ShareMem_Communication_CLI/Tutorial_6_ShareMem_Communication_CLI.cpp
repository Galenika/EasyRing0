#include <Windows.h>
#include <iostream>

static const std::string	gsc_szSymLink = "\\\\.\\ShMem_Test";

int main()
{
	auto szMessage		= std::string("");
	auto dwWriteCount	= 0UL;

	auto hDriver = CreateFileA(gsc_szSymLink.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (!hDriver || hDriver == INVALID_HANDLE_VALUE)
	{
		printf("CreateFileA(read) fail! Error: %u\n", GetLastError());
		return 1;
	}

	auto hMapFile = OpenFileMappingA(FILE_MAP_READ, FALSE, "Global\\SharedMemoryTest");
	if (!hMapFile || hMapFile == INVALID_HANDLE_VALUE)
	{
		printf("OpenFileMappingA(read) fail! Error: %u\n", GetLastError());
		return 0;
	}

	auto pBuf = (char*)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 4096);
	if (!pBuf)
	{
		printf("OpenFileMappingA(read) fail! Error: %u\n", GetLastError());
		return 0;
	}

	printf("Data: %s\n", pBuf);

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);

	// ---

	hMapFile = OpenFileMappingA(FILE_MAP_WRITE, FALSE, "Global\\SharedMemoryTest");
	if (!hMapFile || hMapFile == INVALID_HANDLE_VALUE)
	{
		printf("OpenFileMappingA(write) fail! Error: %u\n", GetLastError());
		return 0;
	}

	pBuf = (char*)MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 4096);
	if (!pBuf)
	{
		printf("OpenFileMappingA(write) fail! Error: %u\n", GetLastError());
		return 0;
	}

	RtlCopyMemory(pBuf, "Message from user", 17);

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);

	// ---

	szMessage = "read_shared_memory";

	dwWriteCount = 0UL;
	if (WriteFile(hDriver, szMessage.c_str(), szMessage.size() + 1, &dwWriteCount, NULL) == FALSE)
	{
		printf("WriteFile(read) fail! Error: %u\n", GetLastError());
		return false;
	}

	printf("Read Message sent! Size: %u Content size: %u\n", dwWriteCount, szMessage.size());

	printf("-------------------------\n");

	printf("Completed\n");
    return 0;
}

