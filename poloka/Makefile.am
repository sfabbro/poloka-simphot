CLEANFILES = *~
MAINTAINERCLEANFILES = \
	Makefile.in \
	stamp-*

lib_LTLIBRARIES = libpoloka-simphot.la

src_includedir = $(includedir)/poloka
src_include_HEADERS = \
	array4d.h \
	calibratedstar.h \
	imagepsfserver.h \
	lightcurvefile.h \
	lightcurvesyntaxerror.h \
	model.h \
	objecttofit.h \
	pmstar.h \
	resampler.h \
	rimage.h \
	simphotfit.h \
	vignette.h


libpoloka_simphot_la_SOURCES = \
	$(src_include_HEADERS) \
	array4d.cc \
	calibratedstar.cc \
	imagepsfserver.cc \
	lightcurvefile.cc \
	model.cc \
	objecttofit.cc \
	pmstar.cc \
	resampler.cc \
	simphotfit.cc \
	vignette.cc

libpoloka_simphot_la_CPPFLAGS = @POLOKA_CORE_CFLAGS@ @POLOKA_PSF_CFLAGS@

libpoloka_simphot_la_LIBADD = @POLOKA_CORE_LIBS@ @POLOKA_PSF_LIBS@
