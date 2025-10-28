PREFIX ?= /usr
LIBDIR ?= $(PREFIX)/lib
CXX = g++
CXXFLAGS += -fPIC -O2 -Wall `pkg-config --cflags vdr`
LDFLAGS += `pkg-config --libs vdr gstreamer-1.0 gstreamer-app-1.0`

SRCS = gstdevice.cpp plugin.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = libvdr-gstreamer.so

.PHONY: all clean install

all: $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) -shared -Wl,-soname,$(TARGET) -o $@ $(OBJS) $(LDFLAGS)

install: $(TARGET)
	@- mkdir -p $(DESTDIR)$(LIBDIR)/vdr/plugins
	@- install -m 0644 $(TARGET) $(DESTDIR)$(LIBDIR)/vdr/plugins/

clean:
	@- rm -f $(OBJS) $(TARGET)