SUBDIRS = mpitransmit

pkginclude_HEADERS = \
	log4espp.hpp \
	types.hpp \
	exceptions.hpp \
	transmit.hpp \
	workerMaps.hpp \
	workersActive.hpp \
	ParallelMethod.hpp \
	ParallelClass.hpp \
	ParallelObject.hpp \
	common.hpp \
	controller.hpp \
	worker.hpp \
	pmi.hpp

lib_LTLIBRARIES = libpmi.la 
libpmi_la_SOURCES = \
	types.cpp \
	exceptions.cpp \
	workerMaps.cpp \
	workersActive.cpp \
	ParallelClass.cpp \
	ParallelMethod.cpp \
	ParallelObject.cpp \
	controller.cpp \
	worker.cpp
libpmi_la_LIBADD = \
	mpitransmit/libpmimpi.la


