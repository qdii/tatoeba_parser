#ifndef FILTER_H
#define FILTER_H

NAMESPACE_START

struct sentence;

struct filter
{
    virtual ~filter() { }
    virtual bool parse( const sentence & _sentence ) = 0;
};

typedef std::vector< std::shared_ptr< filter > > FilterVector ;

NAMESPACE_END

#endif // FILTER_H
