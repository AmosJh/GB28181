#include "SIP/SIPPacket.h"
#include "Tools/Log.h"
#include "SIP/SIPServer.h"
#include "Network/UDPServer.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>

using namespace AmosGB28181;

int main(int argc, char const *argv[])
{
    SIPServer sipServer("192.168.31.218", 50601);

    sipServer.Start();

    while (1)
    {
       sleep(1);
    }
    
    return 0;
}
