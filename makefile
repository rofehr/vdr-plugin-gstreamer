PLUGIN = gstreamer
PLUGINSDIR ?= $(DESTDIR)/usr/lib/vdr/plugins
PKGCFG = pkg-config
GST_DEPS = gstreamer-1.0 gstreamer-app-1.0 gstreamer-audio-1.0 gstreamer-video-1.0
CXXFLAGS ?= -O2 -Wall -Wextra -std=c++11 -fPIC
CXXFLAGS += $(shell $(PKGCFG) --cflags $(GST_DEPS)) -D_FILE_OFFSET_BITS=64
LDFLAGS ?=
LIBS = $(shell $(PKGCFG) --libs $(GST_DEPS))
SRCS = src/gstdevice.cpp src/plugin-gstreamer.cpp
OBJS = $(SRCS:.cpp=.o)
all: libvdr-$(PLUGIN).so
libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $(OBJS) $(LIBS)
install: libvdr-$(PLUGIN).so
	mkdir -p $(PLUGINSDIR)
	cp -a libvdr-$(PLUGIN).so $(PLUGINSDIR)/
clean:
	rm -f $(OBJS) libvdr-$(PLUGIN).so
