# Makefile for vdr-plugin-gstreamer with AV (audio+video) support
# ---------------------------------------------------------------
# Supports GStreamer 1.x and pkg-config detection

PLUGIN   = gstreamer
PLUGINSDIR ?= $(DESTDIR)/usr/lib/vdr/plugins

PKGCFG   = pkg-config
GST_DEPS = gstreamer-1.0 gstreamer-app-1.0 gstreamer-audio-1.0 gstreamer-video-1.0

CXXFLAGS ?= -O2 -Wall -Wextra -std=c++11 -fPIC
CXXFLAGS += $(shell $(PKGCFG) --cflags $(GST_DEPS)) -D_FILE_OFFSET_BITS=64
LDFLAGS  ?=
LIBS      = $(shell $(PKGCFG) --libs $(GST_DEPS))

# Quellen
SRCS = \
  gstdevice.cpp \
  plugin-gstreamer.cpp

OBJS = $(SRCS:.cpp=.o)

all: libvdr-$(PLUGIN).so

libvdr-$(PLUGIN).so: $(OBJS)
	@echo "LD  $@"
	$(CXX) $(CXXFLAGS) -shared -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	@echo "CXX $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: libvdr-$(PLUGIN).so
	@echo "Installing to $(PLUGINSDIR)"
	mkdir -p $(PLUGINSDIR)
	cp -a libvdr-$(PLUGIN).so $(PLUGINSDIR)/

clean:
	rm -f $(OBJS) libvdr-$(PLUGIN).so

# Optional: pkg-config check
check-deps:
	@for dep in $(GST_DEPS); do \
		if ! $(PKGCFG) --exists $$dep; then \
			echo "Missing $$dep (please install GStreamer development packages)"; \
			exit 1; \
		fi; \
	done
	@echo "All GStreamer dependencies found."

.PHONY: all install clean check-deps
