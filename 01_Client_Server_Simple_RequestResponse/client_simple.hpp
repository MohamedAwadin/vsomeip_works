#include <memory>
#include <vsomeip/vsomeip.hpp>
#include <cstdio>
#include <chrono>
#include <thread>

#define LOG_INF(...) std::fprintf(stdout, __VA_ARGS__), std::fprintf(stdout, "\n")
#define LOG_ERR(...) std::fprintf(stderr, __VA_ARGS__), std::fprintf(stderr, "\n")

constexpr vsomeip::service_t server_id = 0x0100;
constexpr vsomeip::instance_t instance_id = 0x8888;
constexpr vsomeip::method_t method_id = 0x4022;

class client_app
{
private:
    std::shared_ptr<vsomeip::application> _app_ptr;
    bool service_available = false;

    void send_request()
    {
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(server_id);
        request->set_instance(instance_id);
        request->set_method(method_id);
        auto payload = vsomeip::runtime::get()->create_payload();
        std::vector<vsomeip::byte_t> data = {0x00};
        payload->set_data(data);
        request->set_payload(payload);
        _app_ptr->send(request);
        LOG_INF("Sent request to Service [%04x.%04x] Method [%04x]", server_id, instance_id, method_id);
    }

public:
    bool init(void)
    {
        _app_ptr = vsomeip::runtime::get()->create_application("ClientApp");
        if (!_app_ptr->init()) {
            LOG_ERR("Failed to initialize client application");
            return false;
        }
        _app_ptr->register_state_handler(
            [this](vsomeip::state_type_e _state) {
                if (_state == vsomeip::state_type_e::ST_REGISTERED) {
                    LOG_INF("Client registered with routing manager");
                    _app_ptr->request_service(server_id, instance_id);
                }
            });
        _app_ptr->register_availability_handler(server_id, instance_id,
            [this](vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
                LOG_INF("Service [%04x.%04x] is %s", _service, _instance, _is_available ? "available" : "unavailable");
                if (_is_available && !service_available) {
                    service_available = true;
                    send_request();
                }
            });
        _app_ptr->register_message_handler(server_id, instance_id, method_id,
            [](const std::shared_ptr<vsomeip::message> &_response) {
                auto payload = _response->get_payload();
                auto data = payload->get_data();
                LOG_INF("Received response: %d : wa3lekom elsalam", data[0]);
            });
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        LOG_INF("Client initialized: Requesting Service [%04x.%04x]", server_id, instance_id);
        return true;
    }

    void start(void)
    {
        _app_ptr->start();
    }

    void stop(void)
    {
        _app_ptr->unregister_availability_handler(server_id, instance_id);
        _app_ptr->unregister_message_handler(server_id, instance_id, method_id);
        _app_ptr->unregister_state_handler();
        _app_ptr->stop();
        LOG_INF("Client stopped");
    }
};