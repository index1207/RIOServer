/*********************************************************************************************************
 * Windows Registered I/O (RIO) Sample Code (Echo Server)
 * Minimum requirement: Windows 8 or Windows Server 2012
 * Author: @sm9kr
 *
 * Notice
 * ��� �ڵ� �������� (http://www.serverframework.com/asynchronousevents/rio/)���� ������.
 * �׷��� ���⵵ ����� �ϼ��� �ڵ尡 �־��� ���� �ƴ϶� ��� �κ� ����� �߰��Ͽ� �����ϵ��� ��.
 * RIO�� ������ �����ֱ� ���� �ڵ�� �ִ��� �����ϰ� �ʿ��� �κи� �־���.
 * �׷���, ���װ� ���� �� ����...
 * UDP���·� ���� ������ �����Ƽ�...TCP�� �ϸ� ���� ������ ���� �ؾߵǰ� ���� �������� �ؾߵǴ� �����..
 * RIO�� ���� ����� �Ȱ��� ������ �Ʒ� �ڵ带 ���� TCP�� ���� ��Ű�� �͵� ����� ����.
 *********************************************************************************************************/

#define NOMINMAX

#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>

#include <WinSock2.h>
#include <MSWsock.h>
#include <WS2tcpip.h>
#include <iostream>
#include <deque>

#include <process.h>

#pragma comment(lib, "ws2_32.lib")

using std::cout;
using std::endl;
using std::deque;


RIO_EXTENSION_FUNCTION_TABLE g_rio;
RIO_CQ g_completionQueue = 0;
RIO_RQ g_requestQueue = 0;

HANDLE g_hIOCP = NULL;

SOCKET g_socket;

CRITICAL_SECTION g_criticalSection;

RIO_BUFFERID g_sendBufferId;
RIO_BUFFERID g_recvBufferId;
RIO_BUFFERID g_addrBufferId;

char* g_sendBufferPointer = NULL;
char* g_recvBufferPointer = NULL;
char* g_addrBufferPointer = NULL;


typedef std::deque<HANDLE> Threads;
Threads g_threads;

static const unsigned short PORTNUM = 7777;

static const DWORD RIO_PENDING_RECVS = 100000;
static const DWORD RIO_PENDING_SENDS = 10000;

static const DWORD RECV_BUFFER_SIZE = 1024;
static const DWORD SEND_BUFFER_SIZE = 1024;

static const DWORD ADDR_BUFFER_SIZE = 64;

static const DWORD NUM_IOCP_THREADS = 4;

static const DWORD RIO_MAX_RESULTS = 1000;


enum COMPLETION_KEY
{
	CK_STOP = 0,
	CK_START = 1
};

enum OPERATION_TYPE
{
	OP_NONE = 0,
	OP_RECV = 1,
	OP_SEND = 2
};

struct EXTENDED_RIO_BUF : public RIO_BUF
{
	OPERATION_TYPE operation;
};


/// SEND�� RIO_BUF�� ��ȯť���·� ����� ���� ���� ����.
EXTENDED_RIO_BUF* g_sendRioBufs = NULL;
DWORD g_sendRioBufTotalCount = 0;
__int64 g_sendRioBufIndex = 0;

/// ADDR�� RIO_BUF pointer
EXTENDED_RIO_BUF* g_addrRioBufs = NULL;
DWORD g_addrRioBufTotalCount = 0;
__int64 g_addrRioBufIndex = 0;

/// RIO���� Registering�� ���� �Ҵ��Լ�
char* AllocateBufferSpace(const DWORD bufSize, const DWORD bufCount, DWORD& totalBufferSize, DWORD& totalBufferCount);

unsigned int __stdcall IOThread(void* pV);


int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA data;
	InitializeCriticalSectionAndSpinCount(&g_criticalSection, 4000);

	if (0 != ::WSAStartup(0x202, &data))
	{
		const DWORD gle = ::GetLastError();
		cout << "WSAStartup Error: " << gle << endl;
		exit(0);
	}


	/// RIO ���� ����
	g_socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_REGISTERED_IO);
	if (g_socket == INVALID_SOCKET)
	{
		const DWORD gle = ::GetLastError();
		cout << "WSASocket Error: " << gle << endl;
		exit(0);
	}

	/// port binding
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORTNUM);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (SOCKET_ERROR == ::bind(g_socket, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)))
	{
		const DWORD gle = ::GetLastError();
		cout << "Bind Error: " << gle << endl;
		exit(0);
	}

	/// RIO �Լ� ���̺� ��������
	GUID functionTableId = WSAID_MULTIPLE_RIO;
	DWORD dwBytes = 0;

	if (NULL != WSAIoctl(g_socket, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER, &functionTableId, sizeof(GUID),
		(void**)&g_rio, sizeof(g_rio), &dwBytes, NULL, NULL))
	{
		const DWORD gle = ::GetLastError();
		cout << "WSAIoctl Error: " << gle << endl;
		exit(0);
	}

	/// rio�� completion ����� iocp�� ���.
	g_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (NULL == g_hIOCP)
	{
		const DWORD gle = ::GetLastError();
		cout << "CreateIoCompletionPort Error: " << gle << endl;
		exit(0);
	}


	OVERLAPPED overlapped;

	RIO_NOTIFICATION_COMPLETION completionType;

	completionType.Type = RIO_IOCP_COMPLETION;
	completionType.Iocp.IocpHandle = g_hIOCP;
	completionType.Iocp.CompletionKey = (void*)CK_START;
	completionType.Iocp.Overlapped = &overlapped;

	/// RIO CQ ���� (RQ ������� ũ�ų� ���ƾ� ��)
	g_completionQueue = g_rio.RIOCreateCompletionQueue(RIO_PENDING_RECVS + RIO_PENDING_SENDS, &completionType);
	if (g_completionQueue == RIO_INVALID_CQ)
	{
		const DWORD gle = ::GetLastError();
		cout << "RIOCreateCompletionQueue Error: " << gle << endl;
		exit(0);
	}

	/// RIO RQ ����
	/// SEND CQ�� RECV CQ�� ���� ��.. (���� ����� �ᵵ ��)
	g_requestQueue = g_rio.RIOCreateRequestQueue(g_socket, RIO_PENDING_RECVS, 1, RIO_PENDING_SENDS, 1, g_completionQueue, g_completionQueue, NULL);
	if (g_requestQueue == RIO_INVALID_RQ)
	{
		const DWORD gle = ::GetLastError();
		cout << "RIOCreateRequestQueue Error: " << gle << endl;
		exit(0);
	}


	/// SEND�� RIO ���� ���
	{
		DWORD totalBufferCount = 0;
		DWORD totalBufferSize = 0;

		g_sendBufferPointer = AllocateBufferSpace(SEND_BUFFER_SIZE, RIO_PENDING_SENDS, totalBufferSize, totalBufferCount);
		g_sendBufferId = g_rio.RIORegisterBuffer(g_sendBufferPointer, static_cast<DWORD>(totalBufferSize));

		if (g_sendBufferId == RIO_INVALID_BUFFERID)
		{
			const DWORD gle = ::GetLastError();
			cout << "RIORegisterBuffer Error: " << gle << endl;
			exit(0);
		}

		DWORD offset = 0;

		g_sendRioBufs = new EXTENDED_RIO_BUF[totalBufferCount];
		g_sendRioBufTotalCount = totalBufferCount;

		for (DWORD i = 0; i < g_sendRioBufTotalCount; ++i)
		{
			/// RIO operation�� �µ��� �Ҵ�� ���۸� �ɰ���
			EXTENDED_RIO_BUF* pBuffer = g_sendRioBufs + i;

			pBuffer->operation = OP_SEND;
			pBuffer->BufferId = g_sendBufferId;
			pBuffer->Offset = offset;
			pBuffer->Length = SEND_BUFFER_SIZE;

			offset += SEND_BUFFER_SIZE;

		}

	}


	/// ADDR�� (RECV�� ��ܿ��� �ּҿ�) RIO ���� ���
	{
		DWORD totalBufferCount = 0;
		DWORD totalBufferSize = 0;

		g_addrBufferPointer = AllocateBufferSpace(ADDR_BUFFER_SIZE, RIO_PENDING_RECVS, totalBufferSize, totalBufferCount);
		g_addrBufferId = g_rio.RIORegisterBuffer(g_addrBufferPointer, static_cast<DWORD>(totalBufferSize));

		if (g_addrBufferId == RIO_INVALID_BUFFERID)
		{
			const DWORD gle = ::GetLastError();
			cout << "RIORegisterBuffer Error: " << gle << endl;
			exit(0);
		}

		DWORD offset = 0;

		g_addrRioBufs = new EXTENDED_RIO_BUF[totalBufferCount];
		g_addrRioBufTotalCount = totalBufferCount;

		for (DWORD i = 0; i < totalBufferCount; ++i)
		{
			EXTENDED_RIO_BUF* pBuffer = g_addrRioBufs + i;

			pBuffer->operation = OP_NONE;
			pBuffer->BufferId = g_addrBufferId;
			pBuffer->Offset = offset;
			pBuffer->Length = ADDR_BUFFER_SIZE;

			offset += ADDR_BUFFER_SIZE;
		}
	}

	/// RECV�� RIO ���� ��� �� RECV �̸� �ɾ����
	{
		DWORD totalBufferCount = 0;
		DWORD totalBufferSize = 0;

		g_recvBufferPointer = AllocateBufferSpace(RECV_BUFFER_SIZE, RIO_PENDING_RECVS, totalBufferSize, totalBufferCount);

		g_recvBufferId = g_rio.RIORegisterBuffer(g_recvBufferPointer, static_cast<DWORD>(totalBufferSize));
		if (g_recvBufferId == RIO_INVALID_BUFFERID)
		{
			const DWORD gle = ::GetLastError();
			cout << "RIORegisterBuffer Error: " << gle << endl;
			exit(0);
		}


		DWORD offset = 0;

		EXTENDED_RIO_BUF* pBufs = new EXTENDED_RIO_BUF[totalBufferCount];

		for (DWORD i = 0; i < totalBufferCount; ++i)
		{
			EXTENDED_RIO_BUF* pBuffer = pBufs + i;

			pBuffer->operation = OP_RECV;
			pBuffer->BufferId = g_recvBufferId;
			pBuffer->Offset = offset;
			pBuffer->Length = RECV_BUFFER_SIZE;

			offset += RECV_BUFFER_SIZE;

			/// �̸� RECV ���� �ɾ� ���´�.
			if (!g_rio.RIOReceiveEx(g_requestQueue, pBuffer, 1, NULL, &g_addrRioBufs[g_addrRioBufIndex++], NULL, 0, 0, pBuffer))
			{
				const DWORD gle = ::GetLastError();
				cout << "RIOReceive Error: " << gle << endl;
				exit(0);
			}
		}

		cout << totalBufferCount << " total receives pending" << endl;
	}


	/// IO ������ ����
	for (DWORD i = 0; i < NUM_IOCP_THREADS; ++i)
	{
		unsigned int notUsed;
		const uintptr_t result = ::_beginthreadex(0, 0, IOThread, (void*)i, 0, &notUsed);
		if (result == 0)
		{
			const DWORD gle = ::GetLastError();
			cout << "_beginthreadex Error: " << gle << endl;
			exit(0);
		}

		g_threads.push_back(reinterpret_cast<HANDLE>(result));
	}

	/// Completion�� �غ�Ǿ����� �˸���
	INT notifyResult = g_rio.RIONotify(g_completionQueue);
	if (notifyResult != ERROR_SUCCESS)
	{
		const DWORD gle = ::GetLastError();
		cout << "RIONotify Error: " << gle << endl;
		exit(0);
	}


	/// �ƹ�Ű�� ������ �ڿ� ��ȯ�ϰ� ������ -.- ;
	cout << "Press Any Key to Stop" << endl;
	getchar();



	/// ���ø���Ű�� CK_STOP���� ���� ������ ������ ������� ��� 
	for (Threads::const_iterator it = g_threads.begin(), end = g_threads.end(); it != end; ++it)
	{
		if (0 == ::PostQueuedCompletionStatus(g_hIOCP, 0, CK_STOP, 0))
		{
			const DWORD gle = ::GetLastError();
			cout << "PostQueuedCompletionStatus Error: " << gle << endl;
			exit(0);
		}
	}

	/// ������ ����
	for (Threads::const_iterator it = g_threads.begin(), end = g_threads.end(); it != end; ++it)
	{
		HANDLE hThread = *it;

		if (WAIT_OBJECT_0 != ::WaitForSingleObject(hThread, INFINITE))
		{
			const DWORD gle = ::GetLastError();
			cout << "WaitForSingleObject (thread join) Error: " << gle << endl;
			exit(0);
		}

		::CloseHandle(hThread);
	}


	if (SOCKET_ERROR == ::closesocket(g_socket))
	{
		const DWORD gle = ::GetLastError();
		cout << "closesocket Error: " << gle << endl;
	}

	g_rio.RIOCloseCompletionQueue(g_completionQueue);

	g_rio.RIODeregisterBuffer(g_sendBufferId);
	g_rio.RIODeregisterBuffer(g_recvBufferId);
	g_rio.RIODeregisterBuffer(g_addrBufferId);

	DeleteCriticalSection(&g_criticalSection);

	return 0;
}


unsigned int __stdcall IOThread(void* pV)
{
	DWORD numberOfBytes = 0;

	ULONG_PTR completionKey = 0;
	OVERLAPPED* pOverlapped = 0;

	RIORESULT results[RIO_MAX_RESULTS];

	while (true)
	{
		if (!::GetQueuedCompletionStatus(g_hIOCP, &numberOfBytes, &completionKey, &pOverlapped, INFINITE))
		{
			const DWORD gle = ::GetLastError();
			cout << "GetQueuedCompletionStatus Error: " << gle << endl;
			exit(0);
		}

		/// ck�� 0�� �Ѿ���� ������
		if (completionKey == CK_STOP)
			break;

		memset(results, 0, sizeof(results));

		ULONG numResults = g_rio.RIODequeueCompletion(g_completionQueue, results, RIO_MAX_RESULTS);
		if (0 == numResults || RIO_CORRUPT_CQ == numResults)
		{
			const DWORD gle = ::GetLastError();
			cout << "RIODequeueCompletion Error: " << gle << endl;
			exit(0);
		}

		/// Dequeue �� �������� ���� ���ø����� �����ϵ��� �˸���.
		INT notifyResult = g_rio.RIONotify(g_completionQueue);
		if (notifyResult != ERROR_SUCCESS)
		{
			const DWORD gle = ::GetLastError();
			cout << "RIONotify Error: " << gle << endl;
			exit(0);
		}

		for (DWORD i = 0; i < numResults; ++i)
		{
			EXTENDED_RIO_BUF* pBuffer = reinterpret_cast<EXTENDED_RIO_BUF*>(results[i].RequestContext);

			if (OP_RECV == pBuffer->operation)
			{
				/// UDP�ϱ� �۽��������� ���� �����Ͱ� ���� �ȿ��� ���� ����
				if (results[i].BytesTransferred != RECV_BUFFER_SIZE)
					break;

				///// ECHO TEST
				const char* offset = g_recvBufferPointer + pBuffer->Offset;

				/// RQ�� thread-safe���� �ʱ� ������ ��ȣ����� �Ѵ�. (����ȭ�� ������ �ʾ���. ���� LOCK...)
				::EnterCriticalSection(&g_criticalSection);
				{

					EXTENDED_RIO_BUF* sendBuf = &(g_sendRioBufs[g_sendRioBufIndex++ % g_sendRioBufTotalCount]);
					char* sendOffset = g_sendBufferPointer + sendBuf->Offset;
					memcpy_s(sendOffset, RECV_BUFFER_SIZE, offset, pBuffer->Length);

					/// TEST PRINT
					cout << strlen(sendOffset) << " ";


					if (!g_rio.RIOSendEx(g_requestQueue, sendBuf, 1, NULL, &g_addrRioBufs[g_addrRioBufIndex % g_addrRioBufTotalCount], NULL, NULL, 0, sendBuf))
					{
						const DWORD gle = ::GetLastError();
						cout << "RIOSend Error: " << gle << endl;
						exit(0);;
					}

				}
				::LeaveCriticalSection(&g_criticalSection);


			}
			else if (OP_SEND == pBuffer->operation)
			{
				/// ���������� RQ ��ȣ
				::EnterCriticalSection(&g_criticalSection);
				{

					if (!g_rio.RIOReceiveEx(g_requestQueue, pBuffer, 1, NULL, &g_addrRioBufs[g_addrRioBufIndex % g_addrRioBufTotalCount], NULL, 0, 0, pBuffer))
					{
						const DWORD gle = ::GetLastError();
						cout << "RIOReceive Error: " << gle << endl;
						exit(0);
					}
					else
					{
					}

					g_addrRioBufIndex++;

				}
				::LeaveCriticalSection(&g_criticalSection);

			}
			else
				break;

		}
	}


	return 0;
}


template <typename TV, typename TM>
inline TV RoundDown(TV Value, TM Multiple)
{
	return((Value / Multiple) * Multiple);
}

template <typename TV, typename TM>
inline TV RoundUp(TV Value, TM Multiple)
{
	return(RoundDown(Value, Multiple) + (((Value % Multiple) > 0) ? Multiple : 0));
}


char* AllocateBufferSpace(const DWORD bufSize, const DWORD bufCount, DWORD& totalBufferSize, DWORD& totalBufferCount)
{
	SYSTEM_INFO systemInfo;

	::GetSystemInfo(&systemInfo);

	const unsigned __int64 granularity = systemInfo.dwAllocationGranularity;

	const unsigned __int64 desiredSize = bufSize * bufCount;

	unsigned __int64 actualSize = RoundUp(desiredSize, granularity);

	if (actualSize > std::numeric_limits<DWORD>::max())
	{
		actualSize = (std::numeric_limits<DWORD>::max() / granularity) * granularity;
	}

	totalBufferCount = std::min<DWORD>(bufCount, static_cast<DWORD>(actualSize / bufSize));

	totalBufferSize = static_cast<DWORD>(actualSize);

	char* pBuffer = reinterpret_cast<char*>(VirtualAllocEx(GetCurrentProcess(), 0, totalBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

	if (pBuffer == 0)
	{
		const DWORD gle = ::GetLastError();
		cout << "VirtualAllocEx Error: " << gle << endl;
		exit(0);
	}

	return pBuffer;
}