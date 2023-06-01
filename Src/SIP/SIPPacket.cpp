#include "SIP/SIPPacket.h"
#include "Tools/Log.h"

#include <string.h>

namespace AmosGB28181
{
    SIPPacket::SIPPacket()
    {

    }

    SIPPacket::SIPPacket(const char* data) : m_Data(data), m_ReadPos(0)
    {
        
    }

    std::string& SIPPacket::GetData()
    {
        return m_Data;
    }



void SIPPacket::ParseFirstLine()
{
    int posMethodE = m_Data.find(" ", 0);
    int posURIE = m_Data.find(" ", posMethodE + 1);

    // LOG("posMethodE is %d", posMethodE);

    m_Method = std::string(m_Data, 0, posMethodE);
    m_URI = std::string(m_Data, posMethodE + 1, posURIE - posMethodE - 1);

    LOG("method is [%s]", m_Method.c_str());
    LOG("m_URI is [%s]", m_URI.c_str());

    m_ReadPos = m_Data.find("\r\n", 0) + 2;
}

void SIPPacket::ParseHeader()
{
    // LOG("After parsing first line,data is \n[%s]", std::string(m_Data, m_ReadPos).c_str());

    while (m_Data.substr(m_ReadPos, 2).compare("\r\n") != 0)
    {
        int startPos = m_ReadPos;

        int midPos  = m_Data.find(": ", m_ReadPos);

        m_ReadPos = m_Data.find("\r\n", startPos);

        // LOG("first is [%s], second is [%s]", 
        //     m_Data.substr(startPos, midPos-startPos).c_str(),  m_Data.substr(midPos+2, m_ReadPos-midPos-2).c_str());

        m_FileMap.insert(std::make_pair(m_Data.substr(startPos, midPos-startPos), m_Data.substr(midPos+2, m_ReadPos-midPos-2)));

        m_ReadPos += 2;
    }
    
}

const std::string& SIPPacket::GetMethod() const
{
    return m_Method;
}

const std::string& SIPPacket::GetURI() const
{
    return m_URI;
}

const std::map<std::string, std::string>& SIPPacket::GetFiled() const
{
    return m_FileMap;
}

const std::string SIPPacket::GetFiled(const std::string& key) const
{
    return m_FileMap.find(key)->second;
}

void SIPPacket::SetStatus(int statusCode, const std::string& statusMessage)
{
    m_StatusCode = statusCode;
    m_StatusMessage = statusMessage;
}

void SIPPacket::AddFiled(const std::string& key, const std::string& value)
{
    m_FileMap.insert(std::make_pair(key, value));
}

void SIPPacket::DumpResponse(char* response)
{
    sprintf(response, "SIP/2.0 %d %s\r\n", m_StatusCode, m_StatusMessage.c_str());

    for(auto it = m_FileMap.begin(); it != m_FileMap.end(); it++)
    {
        sprintf(response + strlen(response), "%s: %s\r\n", it->first.c_str(), it->second.c_str());
    }

    sprintf(response + strlen(response), "Content-Length: 0\r\n");//TODO: need figure out this

    sprintf(response + strlen(response),"\r\n");
}



} // namespace AmosGB28181