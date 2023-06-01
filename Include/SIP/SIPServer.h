#ifndef __SIPSERVER_H__
#define __SIPSERVER_H__

#include "Network/UDPServer.h"
#include "SIP/SIPPacket.h"

namespace AmosGB28181
{

class SIPServer
{

public:
    SIPServer(const char* ip, int port, std::string userAgent = "Amos");

    ~SIPServer();

    void Start();

private:

    void Make401Response(SIPPacket& pkgIn, SIPPacket& pkgOut);

    void Make200Response(SIPPacket& pkgIn, SIPPacket& pkgOut);

    bool CheckAuth(std::string& username, std::string& password, SIPPacket& pkgIn);

    std::string CalcResponse(std::string& username, std::string& password, std::string& realm, std::string& nonce, SIPPacket& packet);

    void OnRegister(SIPPacket& pkg);

    void OnInvite(SIPPacket& pkg);
    
    void OnACK(SIPPacket& pkg);
    
    void OnBYE(SIPPacket& pkg);
    
    void OnMessage(SIPPacket& pkg);
    
    void OnCancel(SIPPacket& pkg);

    void OnRead();

    UDPServer* m_UDPServer;

    std::string m_Tag;  // sip local identifier

    std::string m_UserAgent;

};







} // namespace AmosGB28181

#endif // __SIPSERVER_H__