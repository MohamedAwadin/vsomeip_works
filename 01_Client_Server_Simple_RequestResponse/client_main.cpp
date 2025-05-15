#include "client_simple.hpp"
#include <iostream>
#include <csignal>

client_app* myclapp_ptr = nullptr;

void handle_signal(int _signal)
{
    if (myclapp_ptr && _signal == SIGINT) {
        myclapp_ptr->stop();
        exit(0);
    }
}

int main(void)
{
    client_app myclapp;
    myclapp_ptr = &myclapp;

    signal(SIGINT, handle_signal);

    if (!myclapp.init()) {
        std::cerr << "Client initialization failed" << std::endl;
        return 1;
    }
    std::cout << "Client initialized successfully" << std::endl;
    myclapp.start();
    myclapp.stop();
    return 0;
}