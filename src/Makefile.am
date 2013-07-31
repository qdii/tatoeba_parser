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

android_boost_CPPFLAGS=@android_boost_CPPFLAGS@
android_boost_LIBS=@android_boost_LIBS@
android_boost_version=@android_boost_version@
android_boost_lib="boost_program_options-gcc-mt-$(android_boost_version)"
NDKBUILD = @NDK_BUILD@

Android.mk: Makefile
	./androgenizer -:PROJECT tatoparser -:REL_TOP $(top_srcdir) -:ABS_TOP $(abs_top_srcdir)	\
		       -:SHARED tatoparser \
		       -:SOURCES $(libtatoparser_la_SOURCES) android.cpp \
		       -:LDFLAGS $(libtestlib_la_LDFLAGS) \
		       -:PASSTHROUGH \
		       		"LOCAL_CXXFLAGS	:= -std=c++0x" \
				"LOCAL_C_INCLUDES	:= $(top_srcdir) $(top_srcdir)/include $(top_srcdir)/src $(includedir)" \
				"LOCAL_CPPFLAGS	:= $(android_boost_CPPFLAGS) -DTATO_ANDROID" "LOCAL_CPP_FEATURES	:= exceptions rtti" \
				"LOCAL_LDLIBS := -llog" \
				"LOCAL_LDFLAGS := -l$(android_boost_lib) $(android_boost_LIBS)"> $@

android-ndk: Android.mk
	$(NDKBUILD) NDK_PROJECT_PATH=$(top_srcdir)/src APP_BUILD_SCRIPT=$(top_srcdir)/src/Android.mk APP_STL=gnustl_static V=1
