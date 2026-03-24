#pragma once
#include <zeek/logging/WriterBackend.h>
#include <zeek/threading/formatters/JSON.h>
#include <fluvio.h>
#include <string>

namespace logging {
namespace writer {

class FluvioWriter : public zeek::logging::WriterBackend {
public:
    explicit FluvioWriter(zeek::logging::WriterFrontend* frontend);
    ~FluvioWriter() override;

    static zeek::logging::WriterBackend* Instantiate(zeek::logging::WriterFrontend* frontend) {
        return new FluvioWriter(frontend);
    }

protected:
    bool DoInit(const zeek::logging::WriterBackend::WriterInfo& info, int num_fields, const zeek::threading::Field* const* fields) override;
    bool DoWrite(int num_fields, const zeek::threading::Field* const* fields, zeek::threading::Value** vals) override;
    bool DoSetBuf(bool enabled) override;
    bool DoRotate(const char* rotated_path, double open, double close, bool terminating) override;
    bool DoFlush(double network_time) override;
    bool DoFinish(double network_time) override;
    bool DoHeartbeat(double network_time, double current_time) override;

private:
    zeek::threading::formatter::JSON* formatter;
    fluvio_client_t* client;
    fluvio_topic_producer_t* producer;
    std::string topic_name;
};

}
}
