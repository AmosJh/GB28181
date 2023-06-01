#ifndef __UDPSERER_H__
#define __UDPSERER_H__

#include <string>
#include <sys/socket.h>

namespace AmosGB28181
{

class UDPServer{

public:

    UDPServer();

    UDPServer(const char* ip, int port);

    ~UDPServer();

    int Send(char* buf, int bufLen, struct sockaddr* targetAddr, int addrLen);

    int Recv(char* buf, int len);

private:
    int m_SocketFd;

    std::string m_IP;

    std::string m_Port;

};

}   //namespace AmosGB28181

#endif //__UDPSERER_H__
