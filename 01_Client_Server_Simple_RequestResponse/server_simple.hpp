#include <memory>
#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <cstdio>

#define LOG_INF(...) std::fprintf(stdout, __VA_ARGS__), std::fprintf(stdout, "\n")
#define LOG_ERR(...) std::fprintf(stderr, __VA_ARGS__), std::fprintf(stderr, "\n")

constexpr vsomeip::service_t server_id = 0x0100;
constexpr vsomeip::instance_t instance_id = 0x8888;
constexpr vsomeip::method_t method_id = 0x4022;

void on_message_available(vsomeip::service_t _service,
                      vsomeip::instance_t _instance, bool _is_available)
{
    LOG_INF("Service [%04x.%04x] is %s", _service, _instance, _is_available ? "available" : "unavailable");
}

class server_app
{
private:
    std::shared_ptr<vsomeip::application> _app_ptr;

public:
    void on_message_handler(const std::shared_ptr<vsomeip::message> &_response)
    {
        LOG_INF("Received request for Method [%04x]", _response->get_method());
        std::cout << "Processing client request" << std::endl;
        auto response = vsomeip::runtime::get()->create_response(_response);
        auto payload = vsomeip::runtime::get()->create_payload();
        std::vector<vsomeip::byte_t> data = {0x01};
        payload->set_data(data);
        response->set_payload(payload);
        _app_ptr->send(response);
        LOG_INF("Sent response to client : elslamo 3aleko");
    }

    bool init(void)
    {
        _app_ptr = vsomeip::runtime::get()->create_application("ServerApp");
        if (!_app_ptr->init()) {
            LOG_ERR("Failed to initialize server application");
            return false;
        }
        _app_ptr->register_message_handler(server_id, instance_id, method_id,
            [this](const std::shared_ptr<vsomeip::message> &_response) {
                LOG_INF("Message handler triggered for Method [%04x]", _response->get_method());
                this->on_message_handler(_response);
            });
        _app_ptr->register_availability_handler(server_id, instance_id, on_message_available);
        _app_ptr->offer_service(server_id, instance_id);
        LOG_INF("Server initialized: Service [%04x.%04x]", server_id, instance_id);
        return true;
    }

    void start(void)
    {
        _app_ptr->start();
    }
};