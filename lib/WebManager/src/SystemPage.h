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


#ifndef SYSTEM_PAGE_H

    #define SYSTEM_PAGE_H

    #include "EmbAJAX.h"
    #include "WebManager.h"


    class SystemPage {
        public:

            const char* URL;
            void (*handler)();
            void (*init)();

            EmbAJAXServerFunction btn_restart;
            EmbAJAXServerFunction btn_rst_net;
            EmbAJAXServerFunction btn_rst_all;

            EmbAJAXBase* page_elements[3] = {
      
                &btn_restart,
                &btn_rst_net,
                &btn_rst_all

            };

           SystemPage( void(*phandler)(), void(*pinit)() ) : 

                btn_restart("btn_restart"),
                btn_rst_net("btn_rst_net"),
                btn_rst_all("btn_rst_all"),

                ajax(page_elements, "")
                {
                    URL = "/system.html";            // TODO - need to deal with '/'
                    handler = phandler;
                    init = pinit;
                };

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> ajax;

            void handleAjax();

            void initializeAjax();
    };
    
    extern SystemPage systempage;

#endif