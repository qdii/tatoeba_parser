#ifndef FILTER_LANG_H
#define FILTER_LANG_H

#include "filter.h"
#include "sentence.h"

NAMESPACE_START

struct filterLang : public filter
{
    filterLang( const std::string & _lang ): m_lang( _lang ) { }
    virtual ~filterLang() { }
    virtual bool parse( const sentence & _sentence ) TATO_OVERRIDE
    {
        return m_lang == _sentence.lang();
    }

private:
    std::string m_lang;
};

NAMESPACE_END

#endif // FILTER_LANG_H
