#include "prec_library.h"
#ifdef USE_PYTHON_WRAPPER
#include "tatoparser/dataset.h"
#include "tatoparser/linkset.h"
#include "tatoparser/listset.h"
#include "tatoparser/tagset.h"
#include "tatoparser/sentence.h"
#include "tatoparser/interface_lib.h"
#include <boost/python.hpp>

using namespace boost::python;

#ifndef NAMESPACE
#   define NAMESPACE
#endif
BOOST_PYTHON_MODULE( tatoparser )
{
    // __ DATASET _________________________________________________________________________________________________________
    class_<NAMESPACE :: dataset, boost::noncopyable>( "dataset" )
        .def( "getBySentenceId", & NAMESPACE ::dataset::getBySentenceId, return_value_policy<reference_existing_object>() )
        .def( "getByIndex", & NAMESPACE ::dataset::getByIndex, return_value_policy<reference_existing_object>() )
        .def( "size", & NAMESPACE ::dataset::size )
    ;

    // __ LINKSET _________________________________________________________________________________________________________
    class_<NAMESPACE :: linkset, boost::noncopyable>( "linkset" )
        .def( "areLinked", & NAMESPACE ::linkset::areLinked )
    ;

    // __ LISTSET _________________________________________________________________________________________________________
    class_<NAMESPACE :: listset, boost::noncopyable>( "listset" )
        .def( "isSentenceInList", & NAMESPACE ::listset::pythonIsSentenceInList )
    ;

    // __ TAGSET __________________________________________________________________________________________________________
    class_<NAMESPACE :: tagset, boost::noncopyable>( "tagset" )
        .def( "getTagId", & NAMESPACE ::tagset::getTagId )
        .def( "isSentenceTagged", & NAMESPACE ::tagset::isSentenceTagged )
    ;

    // __ SENTENCE ________________________________________________________________________________________________________
    class_<NAMESPACE :: sentence>( "sentence" )
        .def( "str", & NAMESPACE ::sentence::str )
        .def( "lang", & NAMESPACE ::sentence::lang )
        .def( "id", & NAMESPACE ::sentence::getId )
    ;

    // __ INTERFACE _______________________________________________________________________________________________________
    def( "init", NAMESPACE ::init );
    def( "terminate", NAMESPACE ::terminate );
    def( "parse", NAMESPACE :: parse_ );
}

#endif // USE_PYTHON_WRAPPER
