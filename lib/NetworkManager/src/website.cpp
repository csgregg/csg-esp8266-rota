

#include "website.h"


// Public :

/*

void update_thispage() {
    thispage.check_d.setValue(thispage.check.isChecked() ? "checked" : "not checked");
}


void update_thatpage() {
    thatpage.check_d.setValue(thatpage.check.isChecked() ? "checked" : "not checked");
}
*/
/*
#define MAKE_EmbAJAXPage(name, title, header_add, ...) \
    EmbAJAXBase* name##_elements[] = {__VA_ARGS__}; \
    EmbAJAXPage<sizeof(name##_elements)/sizeof(EmbAJAXBase*)> name(name##_elements, title, header_add);
*/
/*

newbtn check("dsdsd");
EmbAJAXMutableSpan statictext("sdsd");
*/
/*
MAKE_EmbAJAXPage(page, "title", "",
  &check,
  &statictext
)
*/
/*
EmbAJAXBase* page_elements[] = {&check, &statictext};
EmbAJAXPage<2> page(page_elements, "title", "");
*/


AJAXWebPage
    < void(*)()
    , EmbAJAXCheckButton
    , EmbAJAXMutableSpan
> page1
    ( [](){ page1.get<2>().setValue(page1.get<1>().isChecked() ? "checked" : "not checked"); }
    , {"check", "check"}
    , {"check_d"}
);

void tmp(){
    page1.get<0>();
}




// Create the global config instance

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LITERALS)
//    this_webpage thispage;
  //  that_webpage thatpage;
#endif