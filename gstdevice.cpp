#include "gstdevice.h"
if (!buffer) return;


GstMapInfo map;
if (!gst_buffer_map(buffer, &map, GST_MAP_WRITE)) {
gst_buffer_unref(buffer);
return;
}
memcpy(map.data, Data, Length);
gst_buffer_unmap(buffer, &map);


if (pts == GST_CLOCK_TIME_NONE) {
guint64 frame = video_frame_count_++;
gint64 est_ns = (gint64)((double)frame * (GST_SECOND / estimated_fps_));
pts = est_ns;
} else {
video_frame_count_++;
}


GST_BUFFER_PTS(buffer) = pts;
GST_BUFFER_DURATION(buffer) = (gint64)(GST_SECOND / estimated_fps_);


GstFlowReturn ret = GST_FLOW_ERROR;
g_signal_emit_by_name(appsrc_video_, "push-buffer", buffer, &ret);
if (ret != GST_FLOW_OK && ret != GST_FLOW_UNEXPECTED) {
std::cerr << "appsrc_video push-buffer returned flow " << ret << std::endl;
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


void cGstDevice::Flush() {
std::lock_guard<std::mutex> lk(push_mutex_);
if (appsrc_video_) g_signal_emit_by_name(appsrc_video_, "end-of-stream", nullptr);
if (appsrc_audio_) g_signal_emit_by_name(appsrc_audio_, "end-of-stream", nullptr);
}


void cGstDevice::SetMode(tDisplayMode Mode) {
(void)Mode;
}