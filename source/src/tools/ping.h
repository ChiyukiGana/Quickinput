#ifndef CG_PING_H
#define CG_PING_H
#include <IPExport.h>
#include <IcmpAPI.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <mutex>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

struct PingResult
{
    bool    ok = false;
    int     sent = 0;
    int     received = 0;
    double  avgMs = 0.0;
    double  minMs = 0.0;
    double  maxMs = 0.0;
    DWORD   lastError = 0;
};

static bool ResolveIPv4(const std::wstring& host, in_addr& outAddr, DWORD& err)
{
    ADDRINFOW hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    ADDRINFOW* res = nullptr;
    int rc = GetAddrInfoW(host.c_str(), nullptr, &hints, &res);
    if (rc != 0 || !res)
    {
        err = (DWORD)rc;
        return false;
    }
    sockaddr_in* sa = reinterpret_cast<sockaddr_in*>(res->ai_addr);
    outAddr = sa->sin_addr;
    FreeAddrInfoW(res);
    return true;
}

inline std::once_flag _ping_hose_once;
static PingResult PingHost(const std::wstring& host, int count = 4, int dataSize = 32, DWORD timeoutMs = 1000, int ttl = 128)
{
    std::call_once(_ping_hose_once, [] {
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
    });

    PingResult st;

    if (count <= 0)     count = 1;
    if (dataSize < 0)     dataSize = 0;
    if (dataSize > 65500) dataSize = 65500;
    st.sent = count;

    in_addr dest = {};
    if (!ResolveIPv4(host, dest, st.lastError)) return st;

    HANDLE hIcmp = IcmpCreateFile();
    if (hIcmp == INVALID_HANDLE_VALUE)
    {
        st.lastError = GetLastError();
        return st;
    }

    std::vector<char> sendData((size_t)dataSize, 'a');
    DWORD  replySize = (DWORD)(sizeof(ICMP_ECHO_REPLY) + (size_t)dataSize + 64);
    std::vector<char> replyBuf(replySize);

    IP_OPTION_INFORMATION ipOpt = {};
    ipOpt.Ttl = (UCHAR)ttl;
    PIP_OPTION_INFORMATION pOpt = (ttl > 0) ? &ipOpt : nullptr;

    double total = 0.0;
    st.minMs = 1e18;

    for (int i = 0; i < count; ++i)
    {
        DWORD ret = IcmpSendEcho(hIcmp, dest.s_addr, dataSize ? sendData.data() : nullptr, (WORD)dataSize, pOpt, replyBuf.data(), replySize, timeoutMs);

        if (ret > 0)
        {
            PICMP_ECHO_REPLY reply = reinterpret_cast<PICMP_ECHO_REPLY>(replyBuf.data());
            if (reply->Status == IP_SUCCESS)
            {
                st.received++;
                double ms = (double)reply->RoundTripTime;
                total += ms;
                if (ms < st.minMs) st.minMs = ms;
                if (ms > st.maxMs) st.maxMs = ms;
            }
            else
            {
                st.lastError = reply->Status;
            }
        }
        else
        {
            st.lastError = GetLastError();
        }
    }

    IcmpCloseHandle(hIcmp);

    if (st.received > 0)
    {
        st.ok = true;
        st.avgMs = total / st.received;
        if (st.minMs > 1e17) st.minMs = 0.0;
    }
    return st;
}
#endif