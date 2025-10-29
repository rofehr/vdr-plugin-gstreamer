#include "gstdevice.h"
#include <cstring>
#include <iostream>
#include <fstream>


cGstDevice::cGstDevice(const std::string &pipeline_desc)
: pipeline_desc_(pipeline_desc)
{
gst_init(nullptr, nullptr);
InitGst();
}


cGstDevice::~cGstDevice() {
ShutdownGst();
}


void cGstDevice::InitGst() {
GError *err = nullptr;


std::string pipeline_str = pipeline_desc_;
if (pipeline_str.empty()) {
// Fallback: Standard-Pipeline
pipeline_str = "appsrc name=src format=time is-live=true ! queue ! decodebin ! autovideosink sync=true";
}


pipeline_ = gst_parse_launch(pipeline_str.c_str(), &err);
if (!pipeline_ || err) {
std::cerr << "Failed to create pipeline: " << (err ? err->message : "unknown") << std::endl;
if (err) g_error_free(err);
pipeline_ = nullptr;
return;
}


appsrc_ = gst_bin_get_by_name(GST_BIN(pipeline_), "src");
if (!appsrc_) {
std::cerr << "Failed to find appsrc 'src' in pipeline." << std::endl;
gst_object_unref(pipeline_);
pipeline_ = nullptr;
return;
}


g_object_set(G_OBJECT(appsrc_),
"is-live", TRUE,
"format", GST_FORMAT_TIME,
nullptr);


gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}


void cGstDevice::ShutdownGst() {
if (pipeline_) {
gst_element_set_state(pipeline_, GST_STATE_NULL);
if (appsrc_) {
gst_object_unref(appsrc_);
}