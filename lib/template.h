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

/** Description of the library
 *  @file   template.h
 *  @author Chris Gregg
 *  @date   2020
 *  @version
 */


#ifndef LIBRARY_NAME_H

    #define LIBRARY_NAME_H

    // Global Libraries
    #include <Arduino.h>
    // #include <GlobalLibrary.h>

    // Project Libraries
    // #include "ProjectLibrary.h"


    // Sizes
    #define FOR_THIS_LIBRARY 1              // Description

    // Defaults
    #define FOR_THIS_LIBRARY_DEFAULT 2      // Description 


    // Forward Declarations
    class OtherClass;


    ////////////////////////////////////////////
    //// Example Class

    // Sizes
    #define FOR_THIS_CLASS_MAX 1            // Description 

    // Defaults
    #define FOR_THIS_CLASS_DEFAULT 2        // Description 


    /** @class Example Class
     * 
     *  @brief Brief description of class.
     *
     *  Full description of class
     *  continued
     */
    class ExampleClass {

        public:

            /** Constructor */
            ExampleClass();

            /** Brief description
             * @param paramexample      Description of parameter
             *                          continued. Default = 1
             * @returns                 Ture or false */
            bool ExampleMethod( int paramexample = 1 );

            bool operator==(const ExampleClass& other) const {
                return _exampleMember == other._exampleMember;
            }
            bool operator!=(const ExampleClass& other) const {
                return _exampleMember != other._exampleMember;
            }


        protected:

            /** Brief description
             * @param paramexample      Description of parameter
             * @returns                 Ture or false */
            bool ExampleMethod2( int paramexample );

            /** Brief description
             * @param paramexample      Description of parameter
             * @returns                 Ture or false */
            bool ExampleMethod3( int paramexample ) {
                return true;
            };

            int _exampleMember;             // Description of member
            static int _ExampleMember2;     // Description of member


        private:


    };


    extern ExampleClass exampleclass;        // Global instance of the libary name

#endif      // LIBRARY_NAME_H
