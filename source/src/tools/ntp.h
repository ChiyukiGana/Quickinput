#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <optional>
#include <iomanip>
#include <ctime>
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

class NtpClient
{
public:
    int error = 0;
    NtpClient()
    {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
    ~NtpClient()
    {
        WSACleanup();
    }
    std::optional<NtpTimestamp> get(std::string server, int port = 123, int timeout = 5000)
    {
        error = 0;
        SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == INVALID_SOCKET)
        {
            error = WSAGetLastError();
            return {};
        }

        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);

        addrinfo* result = nullptr;
        if (getaddrinfo(server.c_str(), nullptr, nullptr, &result) != 0)
        {
            error = 1;
            closesocket(sock);
            return {};
        }

        serverAddr.sin_addr = ((sockaddr_in*)result->ai_addr)->sin_addr;
        freeaddrinfo(result);

        NtpPacket packet{};
        packet.li_vn_mode = 0x1B;

        if (sendto(sock, (char*)&packet, sizeof(packet), 0, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            error = WSAGetLastError();
            closesocket(sock);
            return {};
        }

        sockaddr_in fromAddr{};
        int fromLen = sizeof(fromAddr);
        int bytesReceived = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (sockaddr*)&fromAddr, &fromLen);

        if (bytesReceived == SOCKET_ERROR)
        {
            error = WSAGetLastError();
            closesocket(sock);
            return {};
        }

        closesocket(sock);
        return packet.transTimestamp;
    }
    std::optional<time_t> unix(std::string server, int port = 123, int timeout = 5000)
    {
        auto result = get(server, port, timeout);
        if (result) return toUnix(*result);
        return {};
    }
    std::optional<time_t> unix64(std::string server, int port = 123, int timeout = 5000)
    {
        auto result = get(server, port, timeout);
        if (result) return toUnix64(*result);
        return {};
    }
    static time_t toUnix64(const NtpTimestamp& ntp)
    {
        return ((static_cast<time_t>(ntohl(ntp.seconds)) - 2208988800ULL) * 1000ULL) + ((static_cast<time_t>(ntohl(ntp.fraction)) * 1000ULL) / UINT_MAX);
    }
    static time_t toUnix(const NtpTimestamp& ntp)
    {
        return static_cast<time_t>(ntohl(ntp.seconds)) - 2208988800ULL;
    }
};