#include <plugin.h>
#include "gstdevice.h"

class cPluginGStreamer : public cPlugin {
public:
cPluginGStreamer() {}
virtual ~cPluginGStreamer() {}

const char *Version() override { return "0.0.1"; }
const char *Description() override { return "VDR output plugin using GStreamer"; }

bool Start() override {
// Create device instance. VDR will keep pointer; simple minimal approach.
new cGstDevice();
return true;
}

bool Configure(const char *Name, const char *Value) override {
(void)Name; (void)Value; // Not implemented in minimal example
return false;
}
};

VDRPLUGINCREATOR(cPluginGStreamer)