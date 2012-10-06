// Shell编程:
// $ echo hello | nc time-a.timefreq.bldrdoc.gov 37 | xxd -p

// 用TCC编译(-run不能写在开头):
// C:\tcc>tcc ws2_32.def -run hellotime.c
// Sat Oct 06 10:31:27 2012
//
// DEF文件用tiny_impdef生成:
// C:\tcc>tiny_impdef.exe C:\WINDOWS\system32\ws2_32.dll
// --> C:\WINDOWS\system32\ws2_32.dll
// <-- ws2_32.def
// 头文件来自w32api软件包.
// Winsock2: winsock2.h with ws2_32.lib.
// Winsock(deprecated): winsock.h with wsock32.lib.

// In computing, the Windows Sockets API (WSA), which was later shortened
// to Winsock, is a technical specification that defines how Windows network
// software should access network services, especially TCP/IP. It defines a
// standard interface between a Windows TCP/IP client application (such as an
// FTP client or a web browser) and the underlying TCP/IP protocol stack. The
// nomenclature is based on the Berkeley sockets API model used in BSD for
// communications between programs.

// The Time Protocol (RFC-868) provides a 32-bit number that
// indicates the number of seconds since midnight January 1, 1900.
// This time is in UTC (which, despite the ordering of the letters,
// stands for Coordinated Universal Time), which is very similar to
// what was once called Greenwich Mean Time or GMT—the time at
// Greenwich, England.

#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[])
{
	WSADATA wsadata;
	WORD wsaversion = MAKEWORD(2, 0);
	int error = WSAStartup(wsaversion, &wsadata);
	assert(!error);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	assert(sock != INVALID_SOCKET);
	const DWORD onesec = 1000;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void *)&onesec, sizeof(onesec));
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	struct hostent* host = gethostbyname("time-a.timefreq.bldrdoc.gov");
	addr.sin_addr.S_un.S_addr = *((ULONG*)host->h_addr_list[0]);
	//addr.sin_addr.S_un.S_addr = inet_addr("132.163.4.101");
	addr.sin_port = htons(37);
	while(1) {
		error = sendto(sock, NULL, 0, 0, (struct sockaddr *)&addr, sizeof(addr)); // empty package
		assert(error == 0);
		char buf[4];
		error = recv(sock, buf, 4, 0);
		//assert(error == 4);
		if (error != 4) {
			printf("Error code %d\n", error);
		} else {
			time_t timet = ntohl(*((u_long*)buf)) - 2208988800L; // seconds since 1900 to 1970
			printf("%s", ctime(&timet));
		}
	}

	closesocket(sock);

	WSACleanup();
	return 0;
}
