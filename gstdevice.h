#pragma once


#include <vdr/plugin.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <mutex>
#include <atomic>
#include <string>


class cGstDevice : public cDevice {
public:
cGstDevice(const std::string &pipeline_desc = "");
virtual ~cGstDevice();


// VDR device overrides
virtual int PlayVideo(const uchar *Data, int Length);
virtual bool Flush() override;
//virtual void SetMode(tDisplayMode Mode);
virtual const char *DeviceName() const { return "GStreamer output"; }


void PushVideo(const uchar *Data, int Length, GstClockTime pts = GST_CLOCK_TIME_NONE);
void SetEstimatedFps(double fps) { estimated_fps_ = fps; }


private:
void InitGst();
void ShutdownGst();


GstElement *pipeline_ = nullptr;
GstElement *appsrc_ = nullptr;
std::mutex push_mutex_;
std::string pipeline_desc_;


std::atomic<guint64> frame_count_{0};
std::atomic<gint64> last_pts_{GST_CLOCK_TIME_NONE};
double estimated_fps_ = 25.0;
};