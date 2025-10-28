#pragma once


#include <vdr/plugin.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <mutex>


class cGstDevice : public cDevice {
public:
cGstDevice();
virtual ~cGstDevice();


// VDR device overrides (vereinfachte Signaturen)
virtual void PlayVideo(const uchar *Data, int Length) override;
virtual void Flush() override;
virtual void SetMode(tDisplayMode Mode) override;
virtual const char *DeviceName() const override { return "GStreamer output"; }


private:
void InitGst();
void ShutdownGst();


GstElement *pipeline_ = nullptr;
GstElement *appsrc_ = nullptr;
std::mutex push_mutex_;
};