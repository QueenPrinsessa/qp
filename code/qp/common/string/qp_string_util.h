#pragma once

namespace qpStringUtil {
    extern char * Duplicate( const char * str ); // allocates and returns a duplicated string, memory has to be freed by the caller.
    extern void Free( char * str ); // frees a string allocated by DuplicateString
}