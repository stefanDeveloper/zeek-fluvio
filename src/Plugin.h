#pragma once
#include <plugin/Plugin.h>

namespace plugin {
namespace Zeek_Fluvio {

class Plugin : public ::plugin::Plugin {
protected:
    plugin::Configuration Configure() override;
};

extern Plugin plugin;

}
}
