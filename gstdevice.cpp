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


void cGstDevice::PushVideo(const uchar *Data, int Length, GstClockTime pts) { 
if (!appsrc_video_ || !pipeline_ || Length <= 0) return;
std::lock_guard<std::mutex> lk(push_mutex_);


GstBuffer *buffer = gst_buffer_new_allocate(nullptr, Length, nullptr);
if (!buffer) return;


GstMapInfo map;
if (!gst_buffer_map(buffer, &map, GST_MAP_WRITE)) {
gst_buffer_unref(buffer);
return;
}
memcpy(map.data, Data, Length);
gst_buffer_unmap(buffer, &map);


if (pts == GST_CLOCK_TIME_NONE) {
guint64 frame = frame_count_++;
gint64 est_ns = (gint64)((double)frame * (GST_SECOND / estimated_fps_));
pts = est_ns;
} else {
frame_count_++;
}


GST_BUFFER_PTS(buffer) = pts;
GST_BUFFER_DURATION(buffer) = (gint64)(GST_SECOND / estimated_fps_);


GstFlowReturn ret = GST_FLOW_ERROR;
g_signal_emit_by_name(appsrc_, "push-buffer", buffer, &ret);
if (ret != GST_FLOW_OK && ret != GST_FLOW_UNEXPECTED) {
std::cerr << "appsrc push-buffer returned flow " << ret << std::endl;
}
gst_buffer_unref(buffer);
}
void cGstDevice::PushAudio(const uchar *Data, int Length, GstClockTime pts) { 
if (!appsrc_audio_ || !pipeline_ || Length <= 0) return;
std::lock_guard<std::mutex> lk(push_mutex_);


GstBuffer *buffer = gst_buffer_new_allocate(nullptr, Length, nullptr);
if (!buffer) return;


GstMapInfo map;
if (!gst_buffer_map(buffer, &map, GST_MAP_WRITE)) {
gst_buffer_unref(buffer);
return;
}
memcpy(map.data, Data, Length);
gst_buffer_unmap(buffer, &map);


if (pts == GST_CLOCK_TIME_NONE) {
// Estimate audio PTS based on sample rate and frames pushed. Here Length is bytes; we can't reliably
// compute samples without knowing format. For minimal example assume 16-bit stereo.
guint64 frame = audio_frame_count_++;
// estimate samples per audio-chunk is unknown; as fallback, space using a fixed small duration
gint64 est_ns = (gint64)((double)frame * (GST_MSECOND * 20)); // 20ms per audio chunk guess
pts = est_ns;
} else {
audio_frame_count_++;
}


GST_BUFFER_PTS(buffer) = pts;
// Duration unknown; set to 20ms guess
GST_BUFFER_DURATION(buffer) = (gint64)(GST_MSECOND * 20);


GstFlowReturn ret = GST_FLOW_ERROR;
g_signal_emit_by_name(appsrc_audio_, "push-buffer", buffer, &ret);
if (ret != GST_FLOW_OK && ret != GST_FLOW_UNEXPECTED) {
std::cerr << "appsrc_audio push-buffer returned flow " << ret << std::endl;
}
gst_buffer_unref(buffer);
}

bool cGstDevice::Flush() {
std::lock_guard<std::mutex> lk(push_mutex_);
if (appsrc_video_) g_signal_emit_by_name(appsrc_video_, "end-of-stream", nullptr);
if (appsrc_audio_) g_signal_emit_by_name(appsrc_audio_, "end-of-stream", nullptr);
}

void cGstDevice::SetMode(ePlayMode Mode) {}
