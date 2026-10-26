#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <optional>
#include <string>
#include <cstdint>
#include <chrono>
#pragma comment(lib, "ws2_32.lib")

struct NtpTimestamp
{
    uint32_t seconds;
    uint32_t fraction;
};

struct NtpPacket
{
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t rootDelay;
    uint32_t rootDispersion;
    uint32_t refId;
    NtpTimestamp refTimestamp;
    NtpTimestamp origTimestamp;
    NtpTimestamp recvTimestamp;
    NtpTimestamp transTimestamp;
};
static_assert(sizeof(NtpPacket) == 48, "NTP packet must be 48 bytes");

class NtpClient
{
public:
    int error = 0;
    double lastRoundTripMs = 0.0;

    NtpClient()
    {
        WSADATA wsaData;
        int _ = WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
    ~NtpClient()
    {
        WSACleanup();
    }

    std::optional<int64_t> get(const std::string& server, int port = 123, int timeout = 5000)
    {
        error = 0;
        SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == INVALID_SOCKET)
        {
            error = WSAGetLastError();
            return {};
        }
        auto closeSock = [&]() { if (sock != INVALID_SOCKET) { closesocket(sock); sock = INVALID_SOCKET; } };

        DWORD tv = static_cast<DWORD>(timeout);
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof(tv));

        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        addrinfo* result = nullptr;
        int gaiErr = getaddrinfo(server.c_str(), nullptr, &hints, &result);
        if (gaiErr != 0)
        {
            error = 1000 + gaiErr;
            closeSock();
            return {};
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(static_cast<u_short>(port));
        serverAddr.sin_addr = reinterpret_cast<sockaddr_in*>(result->ai_addr)->sin_addr;
        freeaddrinfo(result);

        NtpPacket packet{};
        packet.li_vn_mode = static_cast<uint8_t>((0 << 6) | (3 << 3) | 3);

        auto t1 = std::chrono::steady_clock::now();

        if (sendto(sock, reinterpret_cast<const char*>(&packet), static_cast<int>(sizeof(packet)), 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
        {
            error = WSAGetLastError();
            closeSock();
            return {};
        }

        sockaddr_in fromAddr{};
        int fromLen = static_cast<int>(sizeof(fromAddr));
        int bytesReceived = recvfrom(sock, reinterpret_cast<char*>(&packet), static_cast<int>(sizeof(packet)), 0, reinterpret_cast<sockaddr*>(&fromAddr), &fromLen);

        auto t4 = std::chrono::steady_clock::now();

        if (bytesReceived == SOCKET_ERROR)
        {
            error = WSAGetLastError();
            closeSock();
            return {};
        }
        closeSock();

        if (bytesReceived < static_cast<int>(sizeof(NtpPacket)))
        {
            error = 2;
            return {};
        }

        uint8_t mode = packet.li_vn_mode & 0x07;
        uint8_t version = (packet.li_vn_mode >> 3) & 0x07;
        uint8_t leap = (packet.li_vn_mode >> 6) & 0x03;

        if (mode != 4) { error = 3; return {}; }
        if (version < 3 || version > 4) { error = 4; return {}; }
        if (leap == 3) { error = 5; return {}; }
        if (packet.stratum == 0 || packet.stratum >= 16) { error = 6; return {}; }

        int64_t t2_ns = ntpToUnixNs(packet.recvTimestamp);
        int64_t t3_ns = ntpToUnixNs(packet.transTimestamp);
        if (t2_ns == 0 || t3_ns == 0) { error = 7; return {}; }

        int64_t elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t1).count();
        int64_t serverProcessing_ns = t3_ns - t2_ns;

        if (serverProcessing_ns < 0 || serverProcessing_ns > elapsed_ns) serverProcessing_ns = 0;

        int64_t serverAtT4_ns = t3_ns + (elapsed_ns - serverProcessing_ns) / 2;

        lastRoundTripMs = static_cast<double>(elapsed_ns) / 1e6;

        return serverAtT4_ns / 1000000LL;
    }

    std::optional<time_t> unix(const std::string& server, int port = 123, int timeout = 5000)
    {
        auto result = get(server, port, timeout);
        if (!result) return {};
        return static_cast<time_t>(result.value() / 1000);
    }
    std::optional<int64_t> unix_ms(const std::string& server, int port = 123, int timeout = 5000)
    {
        return get(server, port, timeout);
    }

    static int64_t ntpToUnixNs(const NtpTimestamp& ntp)
    {
        uint32_t sec = ntohl(ntp.seconds);
        uint32_t frac = ntohl(ntp.fraction);
        if (sec == 0 && frac == 0) return 0;
        int64_t unix_sec = static_cast<int64_t>(sec) - 2208988800LL;
        int64_t ns = (static_cast<int64_t>(frac) * 1000000000LL) >> 32;
        return unix_sec * 1000000000LL + ns;
    }
    static int64_t ntpToUnixMs(const NtpTimestamp& ntp)
    {
        return ntpToUnixNs(ntp) / 1000000LL;
    }
    static time_t toUnix(const NtpTimestamp& ntp)
    {
        return static_cast<time_t>(ntpToUnixNs(ntp) / 1000000000LL);
    }
    static time_t toUnixMs(const NtpTimestamp& ntp)
    {
        return static_cast<time_t>(ntpToUnixMs(ntp));
    }
};