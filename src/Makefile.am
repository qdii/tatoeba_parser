AM_CXXFLAGS = -std=c++0x -Winvalid-pch
BUILT_SOURCES = prec.h.gch prec_library.h.gch
prec.h.gch: prec.h
	$(CXXCOMPILE) -x c++-header -c $<

prec_library.h.gch: prec_library.h
	$(CXXCOMPILE) -x c++-header -fPIC -c $<

lib_LTLIBRARIES = libtatoparser.la
libtatoparser_la_SOURCES = file_mapper.cpp linkset.cpp sentence.cpp tagset.cpp interface_lib.cpp dataset.cpp
libtatoparser_la_LDFLAGS = -version-info @TATOPARSER_SO_VERSION@
libtatoparser_la_CPPFLAGS = -iquote $(top_srcdir)/include -iquote $(top_srcdir)/src -I $(includedir)
bin_PROGRAMS = tatoparser
tatoparser_SOURCES =  main.cpp options.cpp
tatoparser_LDADD = libtatoparser.la
tatoparser_CPPFLAGS = -iquote $(top_srcdir) -I $(top_srcdir)/include
