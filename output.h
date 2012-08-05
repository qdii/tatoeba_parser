#ifndef OUTPUT_H
#define OUTPUT_H

#include <iostream>
#include "prec.h"

/**@struct cout_warning
 * @brief A structure much like cout, but that can be shut off easily */
struct cout_warning
{
public:
    cout_warning();

    /*@brief Stops displaying warnings */
    int mute();

public:
    static cout_warning & getCoutWarning();
private:
    bool m_isMuted;

    template<typename T>
    friend cout_warning & operator<<( cout_warning & _cout, const T & _ostr );
};

// __________________________________________________________________________ //

inline
cout_warning::cout_warning()
    :m_isMuted( false )
{
}

// __________________________________________________________________________ //

inline
int cout_warning::mute()
{
    m_isMuted = true;
    return 0;
}

// __________________________________________________________________________ //

template<typename T>
cout_warning & operator<<( cout_warning & _cout, const T & _ostr )
{
    if( !_cout.m_isMuted )
        std::cout << _ostr;

    return _cout;
}

// __________________________________________________________________________ //

extern cout_warning * gs_coutWarning;

// __________________________________________________________________________ //

static
inline cout_warning & getCoutWarning()
{
    ASSERT( gs_coutWarning );
    return * gs_coutWarning;
}

// __________________________________________________________________________ //

#define WARN getCoutWarning()
#define ERR std::cerr

#endif //OUTPUT_H
