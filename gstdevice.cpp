#include "gstdevice.h"
"format", GST_FORMAT_TIME,
nullptr);

// Start the pipeline
gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}

void cGstDevice::ShutdownGst()
{
if (pipeline_) {
gst_element_set_state(pipeline_, GST_STATE_NULL);
if (appsrc_) {
gst_object_unref(appsrc_);
appsrc_ = nullptr;
}
gst_object_unref(pipeline_);
pipeline_ = nullptr;
}
}


void cGstDevice::PlayVideo(const uchar *Data, int Length)
{
if (!appsrc_ || !pipeline_ || Length <= 0) return;


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


// Optionally set PTS/DTS here if available. For minimal example we'll skip.
GstFlowReturn ret = GST_FLOW_ERROR;
g_signal_emit_by_name(appsrc_, "push-buffer", buffer, &ret);
if (ret != GST_FLOW_OK && ret != GST_FLOW_UNEXPECTED) {
std::cerr << "appsrc push-buffer returned flow " << ret << std::endl;
}


gst_buffer_unref(buffer);
}


void cGstDevice::Flush()
{
if (!appsrc_) return;
std::lock_guard<std::mutex> lk(push_mutex_);
// Notify end-of-stream so pipeline can flush
g_signal_emit_by_name(appsrc_, "end-of-stream", nullptr);
}


void cGstDevice::SetMode(tDisplayMode Mode)
{
// Minimal implementation: no mode change handling
(void)Mode;
}