#include "Plugin.h"
#include "FluvioWriter.h"

namespace plugin {
namespace Zeek_Fluvio {

Plugin plugin;

plugin::Configuration Plugin::Configure() {
    AddComponent(new ::logging::Component("FluvioWriter", ::logging::writer::FluvioWriter::Instantiate));

    plugin::Configuration config;
    config.name = "Zeek::Fluvio";
    config.description = "Writes Zeek logs to Fluvio Streams";
    config.version.major = 0;
    config.version.minor = 1;
    config.version.patch = 0;
    return config;
}

}
}
