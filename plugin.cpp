#include <vdr/plugin.h>
#include "gstdevice.h"
#include <fstream>
#include <iostream>


class cPluginGStreamer : public cPlugin {
private:
cGstDevice *device_ = nullptr;
std::string pipeline_desc_;


void LoadPipelineConfig() {
std::ifstream conf("/etc/vdr/plugins/gstreamer.conf");
if (conf.is_open()) {
std::getline(conf, pipeline_desc_);
conf.close();
std::cerr << "Loaded pipeline from config: " << pipeline_desc_ << std::endl;
}
}


public:
cPluginGStreamer() {}
virtual ~cPluginGStreamer() {}


const char *Version() { return "0.0.2"; }
const char *Description() { return "GStreamer output plugin (PTS + config pipeline)"; }


bool ProcessArgs(int argc, char *argv[]) override {
for (int i = 0; i < argc; ++i) {
if (!strncmp(argv[i], "pipeline=", 9)) {
pipeline_desc_ = argv[i] + 9;
}
}
return true;
}


bool Start() override {
if (pipeline_desc_.empty()) {
LoadPipelineConfig();
}


device_ = new cGstDevice(pipeline_desc_);
return true;
}


void Stop() override {
delete device_;
device_ = nullptr;
}
};


VDRPLUGINCREATOR(cPluginGStreamer);