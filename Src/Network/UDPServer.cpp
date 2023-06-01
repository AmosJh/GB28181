#include "Network/UDPServer.h"
#include "Tools/Log.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

namespace AmosGB28181
{

UDPServer::UDPServer()
{
    
}

UDPServer::UDPServer(const char* ip, int port)
{
    m_SocketFd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(m_SocketFd > 0);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    int reuse = 1;
    if (setsockopt(m_SocketFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopet error\n");
        return;
    }

    int ret = bind(m_SocketFd, (struct sockaddr*)&addr, sizeof(addr));
    assert(ret == 0);
}

UDPServer::~UDPServer()
{
    close(m_SocketFd);
}

int UDPServer::Send(char* buf, int bufLen, struct sockaddr* targetAddr, int addrLen)
{
    return sendto(m_SocketFd, buf, bufLen, 0, targetAddr, addrLen);
}

int UDPServer::Recv(char* buf, int len)
{
    int readLen =  recvfrom(m_SocketFd, buf, len, 0, NULL, NULL);
    
    return readLen;
}

} //namespace AMOSGB28181
