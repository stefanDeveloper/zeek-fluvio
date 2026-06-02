#include "FluvioWriter.h"
#include <zeek/Desc.h>

namespace logging {
namespace writer {

FluvioWriter::FluvioWriter(zeek::logging::WriterFrontend* frontend) : zeek::logging::WriterBackend(frontend) {
    formatter = new zeek::threading::formatter::JSON(this, zeek::threading::formatter::JSON::TS_EPOCH);
}

FluvioWriter::~FluvioWriter() {
    delete formatter;
}

bool FluvioWriter::DoInit(const zeek::logging::WriterBackend::WriterInfo& info, int num_fields, const zeek::threading::Field* const* fields) {
    topic_name = info.path;

    const char* domain = std::getenv("FLUVIO_TLS_DOMAIN");
    const char* key = std::getenv("FLUVIO_TLS_KEY");
    const char* cert = std::getenv("FLUVIO_TLS_CERT");
    const char* ca = std::getenv("FLUVIO_TLS_CA");

    auto fluvioConfig = FluvioConfig::create("localhost:9003");

    if (domain && key && cert && ca) {
        Info("Active TLS parameters detected! Configuring strict mTLS execution pipeline.");
        fluvioConfig->set_tls_file_paths(domain, key, cert, ca);
    } else {
        Warning("No TLS parameters detected in ENV. Proceeding with TLS-Disabled configuration.");
        fluvioConfig->disable_tls();
    }

    try {
        this->client = Fluvio::connect_with_config(*fluvioConfig);
    } catch (const std::exception& e) {
        std::string err = std::string("Failed to connect to Fluvio: ") + e.what();
        Error(err.c_str());
        return false;
    }

    try {
        this->producer = (*this->client)->topic_producer(topic_name);
    } catch (const std::exception& e) {
        std::string err = std::string("Failed to create producer for topic '") + topic_name + "': " + e.what();
        Error(err.c_str());
        return false;
    }

    return true;
}

bool FluvioWriter::DoWrite(int num_fields, const zeek::threading::Field* const* fields, zeek::threading::Value** vals) {
    if (!this->producer) return false;

    zeek::ODesc desc;
    this->formatter->Describe(&desc, num_fields, fields, vals);
    
    std::string payload = desc.Description();
    auto out = (*this->producer)->send(
        rust::Slice<const uint8_t>(nullptr, 0),
        rust::Slice<const uint8_t>(
            reinterpret_cast<const uint8_t*>(payload.data()),
            payload.size()
        )
    );

    return true;
}

bool FluvioWriter::DoSetBuf(bool enabled) { return true; }
bool FluvioWriter::DoRotate(const char* rotated_path, double open, double close, bool terminating) { return true; }

bool FluvioWriter::DoFlush(double network_time) {
    if (!this->producer) return false;
    (*this->producer)->flush();
    return true;
}

bool FluvioWriter::DoFinish(double network_time) {
    if (this->producer) (*this->producer)->flush();
    return true;
}

bool FluvioWriter::DoHeartbeat(double network_time, double current_time) { return true; }

}
}
