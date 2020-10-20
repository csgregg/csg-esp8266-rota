/* MIT License

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
SOFTWARE. */

/** Manages the storage of string literals in program memory
 *  Based on ideas from https://github.com/jjssoftware/iD8266
 * 
 *  @file   LiteralManager.h
 *  @author Chris Gregg
 *  @date   2020
 *  @version
 */


#ifndef LITERAL_MANAGER_H

    #define LITERAL_MANAGER_H

    // Global Libraries
    #include <Arduino.h>

    // Project Libraries
    #include "Literals.h"


    // Enumerate the IDs for the literals
    enum literalID {
    #define LITERAL( enumVal, str ) enumVal,
        LITERAL_TABLE
    #undef LITERAL
    };


    #define MAX_LITERAL_SIZE    48          // Largest allowed literal size

    // Put the literals in flash, checking they are not too large along the way
    #define LITERAL( enumVal, str ) const char LIT_REF_ ## enumVal[] PROGMEM = str; static_assert( sizeof(str) <= MAX_LITERAL_SIZE, "Literal too large");
        LITERAL_TABLE
    #undef LITERAL

    // Create array of references 
    const char* const LiteralRefs[] PROGMEM = {
    #define LITERAL( enumVal, str ) LIT_REF_ ## enumVal,
        LITERAL_TABLE
    #undef LITERAL
    };


    /** @class Literal Manager Class
     * 
     *  @brief Manages fetching the strings from program memory */
    class LiteralManager
    {

        public:

            /** Retrieve literal by ID
             *  @param id      The ID of the literal
             *  @returns       String class for the literal */
            String GetString( literalID id );
            
            /** Retrieve literal by ID
             *  @param id      The ID of the literal
             *  @returns       Char pointer to the literal */
            const char* GetChar( literalID id );

            /** Retrieve literal by ID
             *  @param id      The ID of the literal
             *  @returns       Size of the literal */
	        size_t GetSize( literalID id ) { return strlen( LiteralRefs[id] ); };


        protected:

            char _Buffer[MAX_LITERAL_SIZE];         // Buffer to hold literal for return reference


    };
    
    
    extern LiteralManager literals;                     // Declaring the global instance

    #define sLIT( name ) literals.GetString( name )     // Returns String for literal
    #define cLIT( name ) literals.GetChar( name )       // Returns char* to literal
    #define zLIT( name ) literals.GetSize( name )       // Returns size of literal

#endif