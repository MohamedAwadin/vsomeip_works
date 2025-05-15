#include "server_simple.hpp"



int main(void)
{
    server_app _server;
    _server.init();
    _server.start();

    return 0 ;
}