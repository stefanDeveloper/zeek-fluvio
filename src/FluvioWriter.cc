#include "FluvioWriter.h"
#include "fluvio.bif.h"

namespace logging {
namespace writer {

FluvioWriter::FluvioWriter(WriterFrontend* frontend) : WriterBackend(frontend), client(nullptr), producer(nullptr) {
    formatter = new threading::formatter::JSON(this, threading::formatter::JSON::TS_EPOCH);
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

bool FluvioWriter::DoInit(const WriterInfo& info, int num_fields, const threading::Field* const* fields) {
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

bool FluvioWriter::DoWrite(int num_fields, const threading::Field* const* fields, threading::Value** vals) {
    if (!producer) return false;

    ODesc desc;
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
