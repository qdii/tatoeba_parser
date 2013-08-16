AM_CXXFLAGS = -std=c++0x -Winvalid-pch
BUILT_SOURCES = prec.h.gch prec_library.h.gch
prec.h.gch: prec.h
	$(CXXCOMPILE) -x c++-header -c $<

prec_library.h.gch: prec_library.h
	$(CXXCOMPILE) -x c++-header -fPIC -iquote $(top_srcdir)/include -c $<

lib_LTLIBRARIES = libtatoparser.la
libtatoparser_la_SOURCES = file_mapper.cpp linkset.cpp sentence.cpp tagset.cpp interface_lib.cpp dataset.cpp listset.cpp python.cpp
libtatoparser_la_LDFLAGS = -version-info @TATOPARSER_SO_VERSION@ @LDFLAGS_PYTHON@
libtatoparser_la_CPPFLAGS = -iquote $(top_srcdir)/include -iquote $(top_srcdir)/src -I $(includedir) $(BOOST_CPPFLAGS) @CPPFLAGS_PYTHON@ @INCLUDE_PYTHON@
libtatoparser_la_CFLAGS = @CFLAGS_PYTHON@
bin_PROGRAMS = tatoparser
tatoparser_SOURCES =  main.cpp options.cpp display.cpp
tatoparser_LDADD = libtatoparser.la $(BOOST_REGEX_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS) $(NCURSES_LIBS)
tatoparser_LDFLAGS = $(BOOST_REGEX_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(NCURSES_LDFLAGS)
tatoparser_CPPFLAGS = -iquote $(top_srcdir) -I $(top_srcdir)/include $(BOOST_CPPFLAGS) $(NCURSES_CPPFLAGS)
