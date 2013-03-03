AM_CXXFLAGS = -std=c++0x -Winvalid-pch
BUILT_SOURCES = prec.h.gch prec_library.h.gch
prec.h.gch: prec.h
	$(CXXCOMPILE) -x c++-header -c $<

prec_library.h.gch: prec_library.h
	$(CXXCOMPILE) -x c++-header -fPIC -iquote $(top_srcdir)/include -c $<

lib_LTLIBRARIES = libtatoparser.la
libtatoparser_la_SOURCES = file_mapper.cpp linkset.cpp sentence.cpp tagset.cpp interface_lib.cpp dataset.cpp listset.cpp
libtatoparser_la_LDFLAGS = -version-info @TATOPARSER_SO_VERSION@ 
libtatoparser_la_CPPFLAGS = -iquote $(top_srcdir)/include -iquote $(top_srcdir)/src -I $(includedir) $(BOOST_CPPFLAGS)
bin_PROGRAMS = tatoparser
tatoparser_SOURCES =  main.cpp options.cpp
tatoparser_LDADD = libtatoparser.la $(BOOST_REGEX_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS)
tatoparser_LDFLAGS = $(BOOST_REGEX_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
tatoparser_CPPFLAGS = -iquote $(top_srcdir) -I $(top_srcdir)/include $(BOOST_CPPFLAGS)

myboost_include="/home/qdii/Documents/Geek/Android/Tatoeba/jni" 				# path to the header of the version of boost to link with
myboost_lib="boost_program_options-gcc-mt-1_49"					 		# path to the compiled library of that version of boost
myboost_lib_directory="/home/qdii/Documents/Geek/Android/Boost/Boost-for-Android/build/lib"	# path to the directory where the library is

Android.mk: Makefile.am
	./androgenizer -:PROJECT tatoparser -:REL_TOP $(top_srcdir) -:ABS_TOP $(abs_top_srcdir)	\
		       -:SHARED tatoparser \
		       -:SOURCES $(libtatoparser_la_SOURCES) \
		       -:LDFLAGS $(libtestlib_la_LDFLAGS) \
		       -:PASSTHROUGH \
		       		"LOCAL_CXXFLAGS	:= -std=c++0x" \
				"LOCAL_C_INCLUDES	:= $(top_srcdir) $(top_srcdir)/include $(top_srcdir)/src $(includedir) ${myboost_include}" \
				"LOCAL_CPPFLAGS	:= $(BOOST_CPPFLAGS)" "LOCAL_CPP_FEATURES	:= exceptions rtti" \
				"LOCAL_LDFLAGS := -l${myboost_lib} -L${myboost_lib_directory}"> $@

android: Android.mk
	ndk-build NDK_PROJECT_PATH=$(top_srcdir)/src APP_BUILD_SCRIPT=$(top_srcdir)/src/Android.mk APP_STL=gnustl_static V=1
