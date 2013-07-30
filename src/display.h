#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include <string>

NAMESPACE_START

struct sentence;
/**
 * @struct cannot_write
 * @author qdii
 * @desc An exception thrown when a display cannot write to its output */
struct cannot_write
{
};

/**
 * @struct display
 * @author qdii
 * @desc A virtual structure to output sentences to devices */
struct display
{
    typedef uint32_t flag;

    /**
     * @brief destructs a display
     */
    virtual ~display() = default;

    /**
     * @param[in] _sentence The sentence to write on the device
     * @param[in] _flags Information to help the display write the correct things
     *            (like, should we display the language, or the ID as well?)
     * @param[in] _translation A translation to display, or nullptr if none is to be displayed
     *            note that the translation is only displayed if DISPLAY_FIRST_TRANSL is part
     *            of the flags.
     * @param[in] _lineNumber Which line should the sentence be output at
     * @throw cannot_write if for some reason the output device could not be written */
    virtual void writeSentence( const sentence & _sentence,
                                flag _flags, unsigned _lineNumber,
                                const sentence * _translation ) = 0;

    static constexpr flag   DISPLAY_NONE            = 0;
    static constexpr flag   DISPLAY_IDS             = 1 << 0;
    static constexpr flag   DISPLAY_LANGUAGES       = 1 << 1;
    static constexpr flag   DISPLAY_FIRST_TRANSL    = 1 << 2;
    static constexpr flag   DISPLAY_LINE_NUMBER     = 1 << 3;

};

/**
 * @struct cout_display
 * @author qdii
 * @desc A display that writes on std::cout */
struct cout_display : public display
{
    /**
     * @brief Constructs a cout_display
     * @param[in] _colored Whether colors should be added to the output
     * @param[in] _separator A string used to separate the different items */
    explicit cout_display( bool _colored, const std::string & _separator );

    /**
     * @brief Writes a sentence to std::cout
     * @param[in] _sentence The sentence to write
     * @param[in] _flags Information on what data to write
     * @param[in] _lineNumber Which line should the sentence be output at
     * @param[in] _translation A translation to display, or nullptr */
    virtual void writeSentence( const sentence & _sentence, display::flag _flags,
                                unsigned _lineNumber,
                                const sentence * _translation ) override final;

private:
    const bool m_colored;
    const std::string m_separator;
};

NAMESPACE_END

#endif
