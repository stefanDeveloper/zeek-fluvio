#include "Plugin.h"
#include "FluvioWriter.h"

namespace plugin {
namespace Zeek_Fluvio {

Plugin plugin;

zeek::plugin::Configuration Plugin::Configure() {
    AddComponent(new ::zeek::logging::Component("FluvioWriter", ::logging::writer::FluvioWriter::Instantiate));

    zeek::plugin::Configuration config;
    config.name = "Zeek::Fluvio";
    config.description = "Writes Zeek logs to Fluvio Streams";
    config.version.major = 0;
    config.version.minor = 1;
    config.version.patch = 0;
    return config;
}

}
}
