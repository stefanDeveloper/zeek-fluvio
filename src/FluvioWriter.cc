#include "FluvioWriter.h"
#include "fluvio.bif.h"
#include <zeek/Desc.h>

namespace logging {
namespace writer {

FluvioWriter::FluvioWriter(zeek::logging::WriterFrontend* frontend) : zeek::logging::WriterBackend(frontend), client(nullptr), producer(nullptr) {
    formatter = new zeek::threading::formatter::JSON(this, zeek::threading::formatter::JSON::TS_EPOCH);
}

FluvioWriter::~FluvioWriter() {
    delete formatter;
    if (producer) {
        fluvio_c_producer_free(producer);
    }
    if (client) {
        fluvio_c_client_free(client);
    }
}

bool FluvioWriter::DoInit(const zeek::logging::WriterBackend::WriterInfo& info, int num_fields, const zeek::threading::Field* const* fields) {
    topic_name = info.path;

    if (fluvio_c_connect(&client) != 0) {
        Error("Failed to connect to Fluvio Cluster");
        return false;
    }

    if (fluvio_c_create_producer(client, topic_name.c_str(), &producer) != 0) {
        std::string err = "Failed to instantiate producer for topic: " + topic_name;
        Error(err.c_str());
        return false;
    }

    return true;
}

bool FluvioWriter::DoWrite(int num_fields, const zeek::threading::Field* const* fields, zeek::threading::Value** vals) {
    if (!producer) return false;

    zeek::ODesc desc;
    formatter->Describe(&desc, num_fields, fields, vals);
    
    std::string payload = desc.Description();
    if (fluvio_c_producer_send(producer, nullptr, 0, (const uint8_t*)payload.data(), payload.size(), nullptr) != 0) {
        Warning("Dropped record due to internal queue/send limits");
        return false;
    }

    return true;
}

bool FluvioWriter::DoSetBuf(bool enabled) { return true; }
bool FluvioWriter::DoRotate(const char* rotated_path, double open, double close, bool terminating) { return true; }

bool FluvioWriter::DoFlush(double network_time) {
    if (producer) fluvio_c_producer_flush(producer);
    return true;
}

bool FluvioWriter::DoFinish(double network_time) {
    if (producer) fluvio_c_producer_flush(producer);
    return true;
}

bool FluvioWriter::DoHeartbeat(double network_time, double current_time) { return true; }

}
}
