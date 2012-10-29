noinst_LIBRARIES = libtatoparser.a
libtatoparser_a_SOURCES = file_mapper.cpp linkset.cpp sentence.cpp tagset.cpp interface_lib.cpp
bin_PROGRAMS = tatoparser
tatoparser_SOURCES = main.cpp options.cpp
tatoparser_LDADD = libtatoparser.a
AM_CXXFLAGS = -std=c++0x
