/* Website Manager Library

MIT License

Copyright (c) 2019 Chris Gregg

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

Describes the About web page

*/


#ifndef ABOUT_PAGE_H

    #define ABOUT_H


    #include "WebManager.h"


    class AboutPage {
        public:

            const char* URL;
            void (*handler)();
            void (*init)();

            EmbAJAXVarBool int_status;

            EmbAJAXMutableSpan device_id;
            EmbAJAXMutableSpan build_env;
            EmbAJAXMutableSpan build_no;
            EmbAJAXMutableSpan build_time;

            EmbAJAXBase* page_elements[5] = {
      
                &int_status,

                &device_id,
                &build_env,
                &build_no,
                &build_time,

            };

           AboutPage( void(*phandler)(), void(*pinit)() ) : 

                int_status("int_status"),
                
                device_id("device_id"),
                build_env("build_env"),
                build_no("build_no"),
                build_time("build_time"),

                ajax(page_elements, "")
                {
                    URL = "/about.html";            // TODO - need to deal with '/'
                    handler = phandler;
                    init = pinit;
                };

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;

            void handleAjax();

            void initializeAjax();
    };
    
    extern AboutPage aboutpage;

#endif