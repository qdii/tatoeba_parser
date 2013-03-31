#ifdef TATO_ANDROID
#include <jni.h>
#include "prec_library.h"
#include "tatoparser/sentence.h"
#include "tatoparser/interface_lib.h"
#include "tatoparser/namespace.h"

////////////////////////////////////////////
//           EXPORTED FUNCTIONS           //
////////////////////////////////////////////
JNIEXPORT void JNICALL Java_com_qdii_tatoparser_tatoparser_init( JNIEnv*, jobject )
{
    init( NONE );
}

JNIEXPORT void JNICALL Java_com_qdii_tatoparser_tatoparser_finish(JNIEnv*, jobject )
{
    terminate();
}

NAMESPACE_START


// road map
// - start should initialize its own data structures
// - a set of helper functions should let the java code retrieve the sentences

static dataset * java_dataset = nullptr;
static linkset * java_linkset = nullptr;
static tagset * java_tagset = nullptr;



JNIEXPORT jobject JNICALL retrieveSentence( JNIEnv _environment, jobject _object, jint _id )
{
    jobject returnedSentence;

    // - retrieve the sentence from its id
    // if it exists
    // - fill up the String part
    // - fill up the Language part
    // - fill up the Id part
    // if it doesn't exist
    // set _id to 0 (invalid)

    return returnedSentence;
}


static jstring retrieveStringFromJavaObject( JNIEnv _environment, jobject _sentence, const char * _attributeName )
{
    assert( nullptr != _attributeName );
    assert( 0 != _sentence );
    assert( 0 != _environment );
    const jclass sentenceClass = _environment.GetObjectClass( _sentence );
    const jfieldID id = _environment.GetFieldID( sentenceClass, _attributeName, "Ljava/lang/String;" );
    assert( 0 != id );
    return static_cast<jstring>( _environment.GetObjectField( _sentence, id ) );
}

/////////////////////////////////////////////
//          INTERNAL FUNCTIONS             //
/////////////////////////////////////////////
template<typename SENTENCE>
bool fillUpJavaSentence( JNIEnv _environment, SENTENCE && _origin, jobject javaSentence_ )
{
    jclass sentenceClass = _environment.GetObjectClass( javaSentence_ );

    // retrieve the different fields of the Java object to fill them up
    jstring languageId = _environment.GetFieldID( sentenceClass, "language", "Ljava/lang/String;" );
    jstring javaData = _environment.GetFieldID( sentenceClass, "data", "Ljava/lang/String;" );

    // this is an INTERNAL ID, it is not the SENTENCE ID.
    // It is the ID identifying the Sentence attribute named "id"
    jfieldID internalId = _environment.GetFieldID( sentenceClass, "id", "I" );

    jint javaId = _environment.GetIntField( sentenceClass, internalId );

    static_assert( sizeof( sentence::id ) == sizeof( jint ), "jint should be the same size as sentence::id" );

    assert( nullptr != java_dataset );

    // copying the data
    //_environment.SetObjectField( javaSentence_, )
}

NAMESPACE_END


#endif
