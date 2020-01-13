
#ifndef WEBSITE_MANAGER_H

    #define WEBSITE_MANAGER_H

    #include <EmbAJAX.h>

 //   #include "Literals.h"

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
        
*/        

    class newbtn : public EmbAJAXCheckButton {
        public:
            newbtn(const char* id) : EmbAJAXCheckButton(id, id) {};
    };

//    template<size_t idx, typename T>
 //   struct GetHelper;
/*
    template<typename ... First>
    struct DataStructure
    {
    };

    template<typename First, typename ... Rest>
    struct DataStructure<First, Rest ...>
    {
        DataStructure(const First& first, const Rest& ... rest)
      //      : page_elements{first, rest...}
         //   , page(page_elements,"T")
        {}
        
        First first;
        DataStructure<Rest ... > rest;

     //   EmbAJAXBase* page_elements[sizeof...(Rest)];

     //   EmbAJAXPage<sizeof...(Rest)> page;

        template<size_t idx>
        auto get()
        {
            return GetHelper<idx, DataStructure<Ttitle,Telements...>>::get(*this);
        }
      */  
 //   };

/*
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

*/

 //   DataStructure<EmbAJAXMutableSpan,EmbAJAXMutableSpan> data("check","Check2");

  //  void datahandle() {
    //    data.get<2>().setValue(data.get<1>().isChecked() ? " Checked" : " Not checked");
   // };








// Helper template
template<size_t idx, typename Path>
struct GetHelper;

// Primary template for AJAXWebpage class
template<typename ... Path>
class AJAXWebPage
{
};

// AJAXWebpage class
// Built on EmbAJAX
template<typename Path, typename ... Elements>
class AJAXWebPage<Path, Elements ...>
{
    public:
        AJAXWebPage(const Path& path, const Elements& ... elements)
            : path(path)
            , elements(elements...)
            , page_elements{const_cast<Elements*>(&elements)...}
            , page(page_elements,"","")
        {}
        
        Path path;
        AJAXWebPage<Elements ... > elements;

        EmbAJAXBase* page_elements[sizeof...(Elements)];
        EmbAJAXPage<sizeof...(Elements)> page;

        template<size_t idx>
        auto get()
        {
            return GetHelper<idx, AJAXWebPage<Path,Elements...>>::get(*this);
        }
};

template<typename Path, typename ... Elements>
struct GetHelper<0, AJAXWebPage<Path, Elements ... >>
{
    static Path get(AJAXWebPage<Path, Elements...>& page)
    {
        return page.path;
    }
};

template<size_t idx, typename Path, typename ... Elements>
struct GetHelper<idx, AJAXWebPage<Path, Elements ... >>
{
    static auto get(AJAXWebPage<Path, Elements...>& page)
    {
        return GetHelper<idx-1, AJAXWebPage<Elements ...>>::get(page.elements);
    }
};













#endif





