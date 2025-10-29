#include "gstdevice.h"
#include <cstring>
#include <iostream>
#include <cmath>

cGstDevice::cGstDevice(const std::string &pipeline_desc)
  : pipeline_desc_(pipeline_desc) {
  gst_init(nullptr, nullptr);
  InitGst();
}

cGstDevice::~cGstDevice() { ShutdownGst(); }

void cGstDevice::InitGst() {
  GError *err = nullptr;
  std::string pipeline_str = pipeline_desc_;
  if (pipeline_str.empty()) {
    pipeline_str =
      "appsrc name=src_v format=time is-live=true ! queue ! decodebin ! videoconvert ! autovideosink sync=true "
      "appsrc name=src_a format=time is-live=true ! queue ! decodebin ! audioconvert ! autoaudiosink sync=true";
  }
  pipeline_ = gst_parse_launch(pipeline_str.c_str(), &err);
  if (!pipeline_ || err) {
    std::cerr << "Failed to create pipeline: " << (err ? err->message : "unknown") << std::endl;
    if (err) g_error_free(err);
    pipeline_ = nullptr;
    return;
  }
  appsrc_video_ = gst_bin_get_by_name(GST_BIN(pipeline_), "src_v");
  appsrc_audio_ = gst_bin_get_by_name(GST_BIN(pipeline_), "src_a");
  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}

void cGstDevice::ShutdownGst() {
  if (pipeline_) {
    gst_element_set_state(pipeline_, GST_STATE_NULL);
    if (appsrc_video_) { gst_object_unref(appsrc_video_); appsrc_video_ = nullptr; }
    if (appsrc_audio_) { gst_object_unref(appsrc_audio_); appsrc_audio_ = nullptr; }
    gst_object_unref(pipeline_);
    pipeline_ = nullptr;
  }
}

int cGstDevice::PlayVideo(const uchar *Data, int Length) { PushVideo(Data, Length); }
int cGstDevice::PlayAudio(const uchar *Data, int Length) { PushAudio(Data, Length); }
void cGstDevice::PushVideo(const uchar *Data, int Length, GstClockTime pts) { /* simplified */ }
void cGstDevice::PushAudio(const uchar *Data, int Length, GstClockTime pts) { /* simplified */ }
bool cGstDevice::Flush() {}
void cGstDevice::SetMode(ePlayMode Mode) {}
