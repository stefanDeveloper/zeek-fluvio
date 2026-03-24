#pragma once
#include <zeek/plugin/Plugin.h>

namespace plugin {
namespace Zeek_Fluvio {

class Plugin : public zeek::plugin::Plugin {
protected:
    zeek::plugin::Configuration Configure() override;
};

extern Plugin plugin;

}
}
