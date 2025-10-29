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
  virtual void PlayVideo(const uchar *Data, int Length) override;
  virtual void PlayAudio(const uchar *Data, int Length) override;
  virtual void Flush() override;
  virtual void SetMode(tDisplayMode Mode) override;
  virtual const char *DeviceName() const override { return "GStreamer output"; }
  void PushVideo(const uchar *Data, int Length, GstClockTime pts = GST_CLOCK_TIME_NONE);
  void PushAudio(const uchar *Data, int Length, GstClockTime pts = GST_CLOCK_TIME_NONE);
  void SetEstimatedFps(double fps) { estimated_fps_ = fps; }
  void SetEstimatedAudioSampleRate(uint32_t rate) { estimated_samplerate_ = rate; }
private:
  void InitGst();
  void ShutdownGst();
  GstElement *pipeline_ = nullptr;
  GstElement *appsrc_video_ = nullptr;
  GstElement *appsrc_audio_ = nullptr;
  std::mutex push_mutex_;
  std::string pipeline_desc_;
  std::atomic<guint64> video_frame_count_{0};
  std::atomic<guint64> audio_frame_count_{0};
  std::atomic<gint64> last_video_pts_{GST_CLOCK_TIME_NONE};
  std::atomic<gint64> last_audio_pts_{GST_CLOCK_TIME_NONE};
  double estimated_fps_ = 25.0;
  uint32_t estimated_samplerate_ = 48000;
};
