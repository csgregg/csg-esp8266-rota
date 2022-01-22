/**
 * @file        AboutPage.h
 * @author      Chris Gregg
 * 
 * @brief       Server-side functions of about.html
 * 
 * @copyright   Copyright (c) 2020
 * 
 */

/* MIT License

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


#ifndef ABOUT_PAGE_H

    #define ABOUT_PAGE_H

    // Global Libraries
    #include <Arduino.h>
    
    // Project Libraries
    #include "WebManager.h"


    /** @class AboutPage
     *  @brief Server-side functions for about.html page */
    class AboutPage {

        public:

            PageHandler handler;                // Handler for this page

            EmbAJAXMutableSpan device_id;       // Span that holds device ID
            EmbAJAXMutableSpan build_env;       // Span that holds build environment
            EmbAJAXMutableSpan build_no;        // Span that holds build number
            EmbAJAXMutableSpan build_time;      // Span that holds build timestamp

            // Array of page elements
            EmbAJAXBase* page_elements[WEB_PAGE_COMMON_ELEMENTS_COUNT + 4] = {
      
                WEB_PAGE_COMMON_ELEMENTS,       // Add the elements comment to every page

                &device_id,
                &build_env,
                &build_no,
                &build_time,

            };

            /** Construct a new page object
             * @param ajaxHander        Pointer to the lamda function that handles ajax for this page
             * @param initHandler       Pointer to the lamda function that initializes this page */
            AboutPage( void(*ajaxHander)(), void(*initHandler)() ) : 
               
                device_id( "device_id" ),
                build_env( "build_env" ),
                build_no( "build_no" ),
                build_time( "build_time" ),

                // Setup the EmbAJAX page base
                ajax( page_elements, "" )
                {
                    handler.URL = "/about.html";
                    handler.ajaxHander = ajaxHander;
                    handler.initHandler = initHandler;
                };

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;       // Instance of EmbAJAX for this page

            /** Function to initialize AJAX on this page */
            void ICACHE_FLASH_ATTR InitializeAjax();

            /** Function to handle AJAX requests for this page */
            void ICACHE_FLASH_ATTR HandleAjax();

    };
    

    extern AboutPage aboutpage;     // Global instance of this page

#endif      // ABOUT_PAGE_H