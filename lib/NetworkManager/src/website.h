
#ifndef WEBSITE_MANAGER_H

    #define WEBSITE_MANAGER_H

    #include <EmbAJAX.h>

    #include "Literals.h"

    void update_thispage();
    void update_thatpage();


    class this_webpage {

        public:
            EmbAJAXCheckButton check;
            EmbAJAXMutableSpan check_d;

            EmbAJAXBase* page_elements[2] = {&check, &check_d};

            this_webpage()
                : check("check", " This option")
                , check_d("check_d")
                , page(page_elements, "This")
                {};

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> page;

            String URI = "/this.html";
    };


    class that_webpage {

        public:
            EmbAJAXCheckButton check;
            EmbAJAXMutableSpan check_d;

            EmbAJAXBase* page_elements[2] = {&check, &check_d};

            that_webpage()
                : check("check", " That option")
                , check_d("check_d")
                , page(page_elements, "That")
                {};

            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> page;

            String URI = "/that.html";
    };

    extern this_webpage thispage;
    extern that_webpage thatpage;


/*


    class WebsiteManager {

        public:



        protected:



    };
    
    
    extern WebsiteManager website;             // Declaring the global instance



    class page1_elements {

        public:
            EmbAJAXCheckButton check;
            EmbAJAXMutableSpan check_d;

            EmbAJAXBase* page_elements[2] = {&check, &check_d};

            page1_elements() : 

                // Initialize page elements
                check("check", " Check Test"),
                check_d("check_d"),

                page(page_elements, "Page 1")            // Initialize page title
                {};


        protected:
            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> page;


    } page1;

    void page1_ajax() {
        page1.check_d.setValue(page1.check.isChecked() ? " Checked" : " Not checked");
    };


   #define MAX_PAGE_PATH_LEN 20

    class page2_elements {

        public:
            EmbAJAXCheckButton check;
            EmbAJAXMutableSpan check_d;

            EmbAJAXBase* page_elements[2] = {&check, &check_d};

            page2_elements() : 

                // Initialize page elements
                check("check", "check"),
                check_d("check_d"),

                page(page_elements, "")            // Initialize page title
                {};


        protected:
            EmbAJAXPage<sizeof(page_elements)/sizeof(EmbAJAXBase*)> page;


    } page2;

    void page2_ajax() {
        page2.check_d.setValue(page2.check.isChecked() ? " Checked" : " Not checked");
    };



    typedef void(*ajax_page)();


    ajax_page ajaxPages[] = {
        page1_ajax,
        page2_ajax
    };


// https://riptutorial.com/cplusplus/example/19276/variadic-template-data-structures
        

    class newbtn : public EmbAJAXCheckButton {
        public:
            newbtn(const char* id) : EmbAJAXCheckButton(id, id) {};
    };

    template<size_t idx, typename T>
    struct GetHelper;

    template<typename ... Ttitle>
    struct DataStructure
    {
    };

    template<typename Ttitle, typename ... Telements>
    struct DataStructure<Ttitle, Telements ...>
    {
        DataStructure(const Ttitle& ptitle, const Telements& ... pelements)
            : title(ptitle)
            , elements(pelements...)
            , page_elements{&pelements...}
        //  , page(page_elements, ptitle)
        {}
        
        Ttitle title;
        DataStructure<Telements ... > elements;

        const EmbAJAXBase* page_elements[sizeof...(Telements)];

        //   EmbAJAXPage<(sizeof...(Telements)/sizeof(EmbAJAXBase*))> page;

        template<size_t idx>
        auto get()
        {
            return GetHelper<idx, DataStructure<Ttitle,Telements...>>::get(*this);
        }
        
    };


    template<typename Ttitle, typename ... Telements>
    struct GetHelper<0, DataStructure<Ttitle, Telements ... >>
    {
        static Ttitle get(DataStructure<Ttitle, Telements...>& data)
        {
            return data.title;
        }
    };


    template<size_t idx, typename Ttitle, typename ... Telements>
    struct GetHelper<idx, DataStructure<Ttitle, Telements ... >>
    {
        static auto get(DataStructure<Ttitle, Telements...>& data)
        {
            return GetHelper<idx-1, DataStructure<Telements ...>>::get(data.elements);
        }
    };



    DataStructure<char*, newbtn, EmbAJAXMutableSpan> data("Title","check","check_d");

    void datahandle() {
        data.get<2>().setValue(data.get<1>().isChecked() ? " Checked" : " Not checked");
    };
*/
#endif





