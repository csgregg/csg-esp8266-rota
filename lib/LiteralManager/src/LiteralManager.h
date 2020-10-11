/* Literal Manager Library

MIT License

Copyright (c) 2020 Chris Gregg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-----------------------------------------------------------------------------

Based on ideas from https://github.com/jjssoftware/iD8266

*/


#ifndef LITERAL_MANAGER_H

    #define LITERAL_MANAGER_H

    #define MAX_LITERAL_SIZE    48          // Largest allowed literal size

    #include <Arduino.h>

    #include "Literals.h"

    // Enumerate the IDs for the literals
    enum literalID {
    #define LITERAL(enumVal, str) enumVal,
        LITERAL_TABLE
    #undef LITERAL
    };

    // Put the literals in flash, checking they are not too large along the way
    #define LITERAL(enumVal, str) const char LIT_REF_ ## enumVal[] PROGMEM = str; static_assert( sizeof(str) <= MAX_LITERAL_SIZE, "Literal too large");
        LITERAL_TABLE
    #undef LITERAL

    // Create array of references 
    const char* const LiteralRefs[] PROGMEM = {
    #define LITERAL(enumVal, str) LIT_REF_ ## enumVal,
        LITERAL_TABLE
    #undef LITERAL
    };


    // Literal Manager Class
    class LiteralManager {

        public:

            String Get(literalID ID);                // Retrieve literal by ID
            const char* pGet(literalID ID);          // Retrieve literal ptr by ID
	        size_t Size(literalID ID) { return strlen(LiteralRefs[ID]) ; };

        protected:

            char _Buffer[MAX_LITERAL_SIZE];


    };
    
    
    extern LiteralManager literals;             // Declaring the global instance

    #define sLIT(name) literals.Get(name)       // Returns String for literal
    #define cLIT(name) literals.pGet(name)      // Returns char* to literal
    #define zLIT(name) literals.Size(name)      // Returns size of literal

#endif