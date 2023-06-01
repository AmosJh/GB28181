#ifndef __SIP_PACKET_H__
#define __SIP_PACKET_H__

#include <iostream>
#include <map>

namespace AmosGB28181
{

class SIPPacket
{
public:
    SIPPacket();

    SIPPacket(const char* data);

    std::string& GetData();

    void ParseFirstLine();
    
    void ParseHeader();

    const std::string& GetMethod() const;

    const std::string& GetURI() const;
    
    const std::map<std::string, std::string>& GetFiled() const;

    const std::string GetFiled(const std::string& key) const;

    void SetStatus(int statusCode, const std::string& statusMessage);

    void AddFiled(const std::string& key, const std::string& value);

    void DumpResponse(char* response);

private:

    std::string m_Data;           //save packetdates

    std::string m_Method;         //method

    std::string m_URI;            //m_URI

    int m_ReadPos;

    std::map<std::string, std::string> m_FileMap;

    int m_StatusCode;

    std::string m_StatusMessage;
};



}

#endif // __SIP_PACKET_H__