#pragma once
#include <logging/WriterBackend.h>
#include <threading/formatters/JSON.h>
#include <fluvio.h>
#include <string>

namespace logging {
namespace writer {

class FluvioWriter : public WriterBackend {
public:
    explicit FluvioWriter(WriterFrontend* frontend);
    ~FluvioWriter() override;

    static WriterBackend* Instantiate(WriterFrontend* frontend) {
        return new FluvioWriter(frontend);
    }

protected:
    bool DoInit(const WriterInfo& info, int num_fields, const threading::Field* const* fields) override;
    bool DoWrite(int num_fields, const threading::Field* const* fields, threading::Value** vals) override;
    bool DoSetBuf(bool enabled) override;
    bool DoRotate(const char* rotated_path, double open, double close, bool terminating) override;
    bool DoFlush(double network_time) override;
    bool DoFinish(double network_time) override;
    bool DoHeartbeat(double network_time, double current_time) override;

private:
    threading::formatter::JSON* formatter;
    fluvio_client_t* client;
    fluvio_topic_producer_t* producer;
    std::string topic_name;
};

}
}
