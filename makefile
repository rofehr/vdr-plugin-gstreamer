PREFIX ?= /usr
LIBDIR ?= $(PREFIX)/lib
CXX = g++

PKGCFG = $(if $(VDRDIR),$(shell PKG_CONFIG_PATH="$$PKG_CONFIG_PATH:../../.." pkg-config --variable=$(1) vdr))

export CFLAGS   = $(call PKGCFG,cflags) -O0 
export CXXFLAGS = $(call PKGCFG,cxxflags) -O0


SRCS = gstdevice.cpp plugin.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = libvdr-gstreamer.so


INCLUDES += $(shell pkg-config --cflags gstreamer-1.0 ) 
INCLUDES += $(VDRDIR)

.PHONY: all clean install

all: $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES)  $< -o $@

$(TARGET): $(OBJS)
	$(CXX) -shared -Wl,-soname,$(TARGET) -o $@ $(OBJS) $(LDFLAGS)

install: $(TARGET)
	@- mkdir -p $(DESTDIR)$(LIBDIR)/vdr/plugins
	@- install -m 0644 $(TARGET) $(DESTDIR)$(LIBDIR)/vdr/plugins/

clean:
	@- rm -f $(OBJS) $(TARGET)