#include "SIP/SIPServer.h"
#include "SIP/SIPPacket.h"
#include "Tools/Log.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <openssl/md5.h>

namespace AmosGB28181
{

SIPServer::SIPServer(const char* ip, int port, std::string userAgent)
: m_UserAgent(userAgent)
{
    m_UDPServer = new UDPServer(ip, port);
    m_Tag = "123456789"; //TODO:use uuid to generate
}

SIPServer::~SIPServer()
{

}

void SIPServer::Start()
{
    std::thread t(&SIPServer::OnRead, this);
    t.detach();
}

void SIPServer::Make401Response(SIPPacket& pkgIn, SIPPacket& pkgOut)
{
    pkgOut.SetStatus(401, "Unauthorized");

    pkgOut.AddFiled("Via", pkgIn.GetFiled("Via"));
    pkgOut.AddFiled("From", pkgIn.GetFiled("From"));
    pkgOut.AddFiled("To", pkgIn.GetFiled("To")+";tag="+m_Tag);
    pkgOut.AddFiled("Call-ID", pkgIn.GetFiled("Call-ID"));
    pkgOut.AddFiled("CSeq", pkgIn.GetFiled("CSeq"));
    pkgOut.AddFiled("Max-Forwards", pkgIn.GetFiled("Max-Forwards"));

    pkgOut.AddFiled("WWW-Authenticate", "Digest realm=\"3402000000\", nonce=\"1234567890123456\"");//TODO: realm和nonce需要自己生成
    pkgOut.AddFiled("Content-Type", "Application/MANSCDP+xml");
    pkgOut.AddFiled("User-Agent", m_UserAgent);
}

void SIPServer::Make200Response(SIPPacket& pkgIn, SIPPacket& pkgOut)
{
    pkgOut.SetStatus(200, "OK");
    std::string via = pkgIn.GetFiled("Via");
    int posPortS =  via.find_last_of(":")+1;
    int posPortE =  via.find(";");

    std::string port = via.substr(posPortS, posPortE - posPortS);
    LOG("port is %s", port.c_str());

    int posRportE = via.find("rport", 0) + strlen("rport");

    pkgOut.AddFiled("Via", via.substr(0, posRportE) + "=" + port + ";" + via.substr(posRportE+1, via.size()-posRportE-1));
    pkgOut.AddFiled("From", pkgIn.GetFiled("From"));
    pkgOut.AddFiled("To", pkgIn.GetFiled("To") + m_Tag);
    pkgOut.AddFiled("Call-ID", pkgIn.GetFiled("Call-ID"));
    pkgOut.AddFiled("CSeq", pkgIn.GetFiled("CSeq"));
    pkgOut.AddFiled("Max-Forwards", pkgIn.GetFiled("Max-Forwards"));
    pkgOut.AddFiled("User-Agent", m_UserAgent);
}

bool SIPServer::CheckAuth(std::string& username, std::string& password, SIPPacket& pkgIn)
{
    const std::string authStr = pkgIn.GetFiled("Authorization");

    int posRealmS = authStr.find("realm=\"", 0) + strlen("realm=\"");
    int posRealmE = authStr.find("\"", posRealmS);
    int posNonceS = authStr.find("nonce=\"", 0) + strlen("nonce=\"");
    int posNonceE = authStr.find("\"", posNonceS);


    std::string realm = authStr.substr(posRealmS, posRealmE - posRealmS);
    std::string nonce = authStr.substr(posNonceS, posNonceE - posNonceS);

    std::string rsp = CalcResponse(username, password, realm, nonce, pkgIn);
    LOG("calc response is %s", rsp.c_str());

    // std::string auth = "response=\""+ rsp + "\"";

    if(authStr.find("response=\""+ rsp + "\"", 0))
    {
        return true;
    }

    return false;
}

std::string SIPServer::CalcResponse(std::string& username, std::string& password, std::string& realm, std::string& nonce, SIPPacket& packet)
{
    // response =H(H(username:realm:password)：nonce：cnonce：H(requestMothod:request-URI))

    LOG("user is %s, passwd is %s, realm is %s, nonce is %s", username.c_str(), password.c_str(), realm.c_str(), nonce.c_str());

    std::string HA1Str = username + ":" + realm + ":" + password;
    std::string HA2Str = packet.GetMethod() + ":" + packet.GetURI();

    unsigned char HA1[MD5_DIGEST_LENGTH];
    unsigned char HA2[MD5_DIGEST_LENGTH];
    unsigned char resp[MD5_DIGEST_LENGTH];

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, HA1Str.c_str(), HA1Str.length());
    MD5_Final(HA1, &ctx);

    MD5_Init(&ctx);
    MD5_Update(&ctx, HA2Str.c_str(), HA2Str.length());
    MD5_Final(HA2, &ctx);

    char HA1_32[MD5_DIGEST_LENGTH*2 + 1];
    char HA2_32[MD5_DIGEST_LENGTH*2 + 1];
    char resp_32[MD5_DIGEST_LENGTH*2 + 1];

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        snprintf(HA1_32 + i * 2, 2 + 1, "%02x", HA1[i]);
        snprintf(HA2_32 + i * 2, 2 + 1, "%02x", HA2[i]);
    }

    std::string respStr = std::string(HA1_32) + ":" + nonce + ":" + HA2_32;

    MD5_Init(&ctx);
    MD5_Update(&ctx, respStr.c_str(), respStr.length());
    MD5_Final(resp, &ctx);

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        snprintf(resp_32 + i * 2, 2 + 1, "%02x", resp[i]);
    }

    return resp_32;
}

void SIPServer::OnRegister(SIPPacket& pkg)
{
    LOG("start OnRegister");

    char response[1024];
    SIPPacket pkgResponse;

    if(pkg.GetFiled().find("Authorization") == pkg.GetFiled().end())
    {
        Make401Response(pkg, pkgResponse);

        pkgResponse.DumpResponse(response);

        LOG("make 401 packet is\n[%s]", response);
    }
    else
    {
        std::string usrname = "34020000002000000002";
        std::string passwd = "123456";

        if(CheckAuth(usrname, passwd, pkg) == true)
        {
            LOG("auth successful");
            Make200Response(pkg, pkgResponse);
            pkgResponse.DumpResponse(response);
            LOG("make 401 packet is\n[%s]", response);
        }
        else
        {
            LOG("auth failed");
        }
    }

    struct sockaddr_in targetAddr;
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_addr.s_addr = inet_addr("192.168.30.217");
    targetAddr.sin_port = htons(5060);

    m_UDPServer->Send(response, sizeof(response), (sockaddr*)&targetAddr, sizeof(targetAddr));
}

void SIPServer::OnRead()
{
    LOG("Call OnRead");
    while(1)
	{
        char buf[1024];
        int len = m_UDPServer->Recv(buf, sizeof(buf));

		LOG("read pkg, len = %d", len);
		LOG("---------------------------\npacket is\n%s\n---------------------------", buf);
        
        SIPPacket pkg(buf);
        pkg.ParseFirstLine();
        pkg.ParseHeader();

        if(strcmp(pkg.GetMethod().c_str(), "REGISTER") == 0)
        {
            OnRegister(pkg);
        }
        else if(strcmp(pkg.GetMethod().c_str(), "INVITE") == 0)
        {
            OnInvite(pkg);
        }
        else if(strcmp(pkg.GetMethod().c_str(), "ACK") == 0)
        {
            OnACK(pkg);
        }
        else if(strcmp(pkg.GetMethod().c_str(), "BYE") == 0)
        {
            OnBYE(pkg);
        }
        else if(strcmp(pkg.GetMethod().c_str(), "MESSAGE") == 0)
        {
            OnMessage(pkg);
        }
        else if(strcmp(pkg.GetMethod().c_str(), "CANCEL") == 0)
        {
            OnCancel(pkg);
        }

        sleep(2);
	}
}

} // namespace AmosGB28181