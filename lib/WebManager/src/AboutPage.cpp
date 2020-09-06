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


*/



#include "AboutPage.h"
#include "Logger.h"
#include "Device.h"

// TODO: Flashstrings


void AboutPage::initializeAjax(){

    LOG_HIGH("(Page) About - Initialize AJAX");

  //  device_id.setValue(device.getChipId());
 //   build_env.setValue(flag_BUILD_ENV);
  //  build_no.setValue(flag_BUILD_NO);
 //   build_time.setValue(flag_BUILD_TIMESTAMP);

}

void AboutPage::handleAjax(){

    LOG_HIGH("(Page) About - Handle AJAX");     

}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ABOUTPAGE)

    AboutPage aboutpage(
        []() { 
            aboutpage.ajax.handleRequest( 
                []() {
                    aboutpage.handleAjax();
                }
            ); 
        },
        []() { 
            aboutpage.initializeAjax();
        }
    );

#endif