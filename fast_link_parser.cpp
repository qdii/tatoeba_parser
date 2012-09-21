#include "prec.h"
#include "fast_link_parser.h"
#include "file_mapper.h"

NAMESPACE_START

int fastLinkParser::start() throw()
{
    register const char * __restrict ptr = getMap().getRegion();
    dataset & __restrict data = *getDataset();
    register size_t size = getMap().getSize();

    if( size == 0 || ptr == nullptr )
        return 0;

    sentence::id firstId;
    sentence::id id = 0;
    int nbLinks = 0;
    register char c;
    
    while( 1 != --size )
    {
        c = *ptr++;

        if( ( c != '\t' ) && ( c != '\n' ) )
        {
            id = 10 * id + c - '0';
        }
        else
        {
            if( c == '\t' )
            {
                firstId = id;
            }
            else if( c == '\n' )
            {
                data.addLink( firstId, id );
                ++nbLinks;
            }

            id = 0;
        }
    }
    
    qlog::info << "parsed " << nbLinks << " links\n";
    
    return nbLinks;
}

NAMESPACE_END
