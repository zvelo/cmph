SUBDIRS = src tests examples man $(CXXMPH)
EXTRA_DIST = cmph.spec configure.ac cmph.pc.in cxxmph.pc.in LGPL-2 MPL-1.1
ACLOCAL_AMFLAGS = -I m4
pkgconfig_DATA = cmph.pc
if USE_CXXMPH
  pkgconfig_DATA += cxxmph.pc
endif

pkgconfigdir = $(libdir)/pkgconfig
