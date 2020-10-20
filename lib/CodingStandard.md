# 1. Files

## 1.1 Source Files

| C++ header files should have the extension *.h*. Source files should have the extension *.cpp*.  |
| --- |
```
MyClass.c++, MyClass.h
```

| A class should be declared in a header file and defined in a source file where the name of the files match the name of the class. Multiple classes in one file should be avoided. |
| --- |
```
MyClass.c++, MyClass.h
```

|  All definitions should reside in source files, unless they are very short.  |
| --- |
```
class MyClass
{
    public:
        int getValue() { return value_; }    // Okay
        ...

    private:
        int value_; 
}
```

## 1.2 Include Files and Statements

| Header files must contain an include guard. |
| --- |
```
#ifndef COM_COMPANY_MODULE_CLASSNAME_H
    #define COM_COMPANY_MODULE_CLASSNAME_H
        :
#endif // COM_COMPANY_MODULE_CLASSNAME_H
```

| Include statements should be sorted and grouped. Sorted by their hierarchical position in the system with low level files included first. Leave an empty line between groups of include statements. |
| --- |
```
#include <fstream>
#include <iomanip>

#include <qt/qbutton.h>
#include <qt/qtextfield.h>

#include "com/company/ui/PropertiesDialog.h"
#include "com/company/ui/MainWindow.h"
```

| Include statements must be located at the top of a file only. |
| --- |
```
class MyClassA
{
    ...
}

#include <fstream>      // NOT here, at the top

class MyClassB
{
    ...
}
```


# 2. Naming Conventions

## 2.1 General Naming Conventions

| Abbreviations and acronyms must not be uppercase when used as name. |
| --- |
```
exportHtmlSource();     // NOT: exportHTMLSource();
openDvdPlayer();        // NOT: openDVDPlayer();
```

| Names representing namespaces should be all lowercase. |
| --- |
```
model::analyzer, io::iomanager, common::math::geometry
```

| The name of the object is implicit, and should be avoided in a method name. |
| --- |
```
line.getLength();     // NOT: line.getLineLength();
```

| Plural form should be used on names representing a collection of objects. |
| --- |
```
vector<Point>  points;
int            values[];
```

| Abbreviations in names should be avoided. |
| --- |
```
computeAverage();     // NOT: compAvg();
```

| Naming pointers specifically should be avoided. |
| --- |
```
Line* line;     // NOT: Line* pLine;
                // NOT: LIne* linePtr;
```

| Negated boolean variable names must be avoided. |
| --- |
```
bool isError;     // NOT: isNoError
bool isFound;     // NOT: isNotFound
```

| Numeration constants can be prefixed by a common type name. |
| --- |
```
enum Color {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE
};
```

## 2.2 Variable and Constant Naming

| Names representing types must be in mixed case starting with upper case. |
| --- |
```
Line, SavingsAccount
```

| Variable names must be in mixed case starting with lower case. |
| --- |
```
line, savingsAccount
```

| Named constants (including enumeration values) must be all uppercase using underscore to separate words. |
| --- |
```
MAX_ITERATIONS, COLOR_RED, PI
```

| Preprocesor *#defines* must be all uppercase using underscore to separate words, and be prefixed with a reference to the namespace or class. |
| --- |
| Named constants should be used in preference to preprocessor defines.
```
#defines LOGGER_MAX_STRING_LENGTH 32
```

| Generic variables should have the same name as their type. |
| --- |
``` 
void setTopic( Topic* topic )       // NOT: void setTopic(Topic* value)
                                    // NOT: void setTopic(Topic* aTopic)
                                    // NOT: void setTopic(Topic* t)

void connect( Database* database )  // NOT: void connect(Database* db)
                                    // NOT: void connect (Database* oracleDB)
```

| Variables with a large scope should have long names, scratch variables with a small scope can have short names. |
| --- |
| Common scratch variables for integers are *i, j, k, m, n* and for characters *c* and *d*. |
```
```

| The prefix *n* should be used for variables representing a number of objects. |
| --- |
```
nPoints, nLines
```

| The suffix *No* should be used for variables representing an entity number. |
| --- |
```
tableNo, employeeNo
```

| Iterator variables should be called *i, j, k* etc. |
| --- |
```
for(int i = 0; i < nTables; i++) {
    ...
}

for( vector<MyClass>::iterator i = list.begin(); i != list.end(); i++ ) {
    Element element = *i;
    ...
}
```

## 2.3 Function and Method Naming

| Names representing methods or functions must be verbs and written in mixed case starting with lower case. |
| --- |
```
getName(), computeTotalWidth()
```

| The terms *get/set* must be used where an attribute is accessed directly. |
| --- |
```
employee.getName();
employee.setName(name);

matrix.getElement( 2, 4 );
matrix.setElement( 2, 4, value );
```

| The term *calc* can be used in methods where something is calculated. |
| --- |
```
valueSet->calcAverage();
matrix->calceInverse();
```

| The term *find* can be used in methods where something is looked up. |
| --- |
```
vertex.findNearestVertex();

matrix.findMinElement();
```

| The term *initialize* can be used where an object or a concept is established. |
| --- |
```
printer.initializeFontSet();
```

| The prefix is should be used for boolean variables and methods. |
| --- |
| Alternatives prefixes include *has, can* and *should*. 
```
isSet, isVisible, isFinished, isFound, isOpen
```

| Complement names must be used for complement operations. |
| --- |
```
get/set, add/remove, create/destroy, start/stop, insert/delete,
increment/decrement, old/new, begin/end, first/last, up/down, min/max,
next/previous, old/new, open/close, show/hide, suspend/resume, etc.
```

## 2.4 Class Naminng

| Names representing template types should be a single uppercase letter, or *SIZE*. |
| --- |
```
template<class T> ...
template<class C, class D> ... 
template<size_t SIZE> ... 
```

| Private class variables should have underscore suffix. |
| --- |
``` 
class SomeClass {
    private:
        int length_;
}
```


# 3. Statements

## 3.1 Types

| Types that are local to one file only should be declared inside that file. |
| --- |
``` 
typedef LocalType int;
}
```

| The parts of a class must be sorted *public, protected* and *private*. All sections must be identified explicitly. Not applicable sections should be left out. |
| --- |
``` 
class SomeClass {
    private:
        int length_;
}
```

| Type conversions must always be done explicitly. Never rely on implicit type conversion. |
| --- |
``` 
floatValue = static_cast<float>( intValue );      // NOT: floatValue = intValue;
```

## 3.2 Variables

| Variables should be declared in the smallest scope possible. |
| --- |
``` 
```

| Variables should be initialized where they are declared. |
| --- |
| Sometimes it is impossible to initialize a variable to a valid value where it is declared. In these cases it should be left uninitialized rather than initialized to some phony value.
``` 
    int x, y, z;
    getCenter( &x, &y, &z );
```

| Variables must never have dual meaning. |
| --- |
``` 
bool isOnAndHot      // NO!;
```

| Use of global variables, global functions or file scope (static variables) should be minimized. |
| --- |
``` 
```

| C++ pointers and references should have their reference symbol next to the type rather than to the name. |
| --- |
``` 
float* x;     // NOT: float *x; 
int& y;       // NOT: int &y;
```

| Class variables should never be declared public. Use private variables and access functions instead |
| --- |
| One exception to this rule is when the class is essentially a data structure, with no behavior (equivalent to a C struct). |
``` 
class SomeClass {
    pubic:
        getLength();
    protected:
        int _length;
}
```

| Implicit test for 0 should not be used other than for boolean variables and pointers. |
| --- |
``` 
if( nLines != 0 )      // NOT: if (nLines)
if( value != 0.0 )     // NOT: if (value)
```

## 3.3 Loops

| Only loop control statements must be included in the for() construction. |
| --- |
``` 
sum = 0;                       // NOT: for( i = 0, sum = 0; i < 100; i++ )
for( i = 0; i < 100; i++ )                sum += value[i];
    sum += value[i];
```

| Loop variables should be initialized immediately before the loop. |
| --- |
``` 
isDone = false;             // NOT: bool isDone = false;
while( !isDone ) {          //      :
    :                       //      while( !isDone ) {
}                           //        :
                            //      }
```

| The form *while(true)* should be used for infinite loops. |
| --- |
``` 
while( true ) {
    :
}

for( ;; ) {  // NO!
    :
}

while( 1 ) { // NO!
    :
}
```

| The nominal case should be put in the *if-part* and the exception in the *else-part* of an if statement. |
| --- |
``` 
bool isOk = readFile (fileName);
if( isOk ) {
    :
}
else {
    :
}
```

| The conditional should be put on a separate line. |
| --- |
``` 
if( isDone )       // NOT: if (isDone) doCleanup();
    doCleanup();
```

| Executable statements in conditionals must be avoided. |
| --- |
``` 
File* fileHandle = open( fileName, "w" );
if( !fileHandle ) {
    :
}

// NOT:
if( !( fileHandle = open(fileName, "w") ) ) {
    :
}
```

| Avoid *do-while* loops. |
| --- |
``` 
```

| The use of *break* and *continue* in loops should be avoided. |
| --- |
``` 
```

# 4. Layout

## 4.1 File structure

| All files must start with license and description |
| --- |
``` 
/* MIT License

Copyright (c) 2020 Chris Gregg

...

/** Description of the library
 *  @file   template.h
 *  @author Chris Gregg
 *  @date   2020
 *  @version
 */
```

| Proprocessor *#defines* should be made at the start of the code if they are relevant to the whole file. *#defines* relevant to just one class should be placed with the class declaration. |
| --- |
``` 
    // Sizes
    #define FOR_THIS_LIBRARY 1              // Description
```

| Forward declarations should be made at the start of the code, following any *#defines*. |
| --- |
``` 
    // Forward Declarations
    class OtherClass;
```


## 4.2 Layout of code

| Basic indentation should be 4 spaces. |
| --- |
``` 
for( i = 0; i < nElements; i++ )
    a[i] = 0;
```

| Block layout should be as illustrated below. |
| --- |
``` 
while( !done ) {
    doSomething();
    done = moreToDo();
}

// NOT:
while( !done )
{
    doSomething();
    done = moreToDo();
}
```

| The *if-else* class of statements should have the following form. |
| --- |
``` 
if( condition ) {
    statements;
}

if( condition ) {
    statements;
}
else {
    statements;
}

if( condition ) {
    statements;
}
else if( condition ) {
    statements;
}
else {
    statements;
}
```

| A *for* statement should have the following form. |
| --- |
``` 
for( initialization; condition; update ) {
    statements;
}
```

| A *while* statement should have the following form. |
| --- |
``` 
while( condition ) {
    statements;
}
```

| A *switch* statement should have the following form. |
| --- |
``` 
switch( condition ) {
    case ABC :
        statements;
        // Fallthrough

    case DEF :
        statements;
        break;

    case XYZ :
        statements;
        break;

    default :
        statements;
        break;
}
```

| A *try-catch* statement should have the following form. |
| --- |
``` 
try {
    statements;
}
catch ( Exception& exception ) {
    statements;
}
```

| Single statement *if-else*, *for* or *while* statements can be written without brackets. |
| --- |
``` 
if( condition )
    statement;

while( condition )
    statement;

for( initialization; condition; update )
    statement;
```

| Class declarations should have the following form. Methods should be grouped and members should be grouped. |
| --- |
``` 
class SomeClass : public BaseClass
{
    public:

        void doThis();
        String getThat( int size );
        

    protected:

        size_t calcSize( String name );
        void startFile( String name );

        String _storedName;
        bool _isOpen;


    private:
        ...
}
```

| Method definitions should have the following form. |
| --- |
``` 
void someMethod()
{
    ...
}
```

## 4.3 Whitespace

| Conventional operators should be surrounded by a space character.<br>Condition statements should be surrounded by a space character.<br>Commas should be followed by a white space.<br>Colons should be surrounded by white space.<br>Semicolons in for statments should be followed by a space character. |
| --- |
``` 
a = (b + c) * d;        // NOT: a=(b+c)*d

doSomething( a, b, c, d );        // NOT: doSomething(a,b,c,d);

case 100 :  // NOT: case 100:

for(i = 0; i < 10; i++) {      // NOT: for(i=0;i<10;i++){
    ...

if( a == b ) {         // NOT: if(a == b)
    ...
}
```

| Method and function arguments should be surrounded by a space character. Also in declarations and definitions. |
| --- |
``` 
doSomething( currentFile );             // NOT: doSomething(currentFile);

doSomething( currentFile, newFile );    // NOT: doSomething(currentFile, newFile);

void doSomething( String currentFile, String newFile );
```

| Logical units within a block should be separated by one blank line. |
| --- |
``` 
Matrix4x4 matrix = new Matrix4x4();

double cosAngle = Math.cos( angle );
double sinAngle = Math.sin( angle );

matrix.setElement( 1, 1,  cosAngle );
matrix.setElement( 1, 2,  sinAngle );
matrix.setElement( 2, 1, -sinAngle );
matrix.setElement( 2, 2,  cosAngle );

multiply( matrix );
```

| *Public:*, *Protected:* and *Private:* specifiers should be surrounded by a blank line, and contribute to indentation. |
| --- |
``` 
class SomeClass : public BaseClass
{
    public:

        void doThis();
        String getThat( int size );
        

    protected:

        ...
}
```

| Use limited alignment if it increases readibility. |
| --- |
``` 
matrix.setElement( 1, 1,  cosAngle );
matrix.setElement( 1, 2,  sinAngle );
matrix.setElement( 2, 1, -sinAngle );
matrix.setElement( 2, 2,  cosAngle );
``` 

| Methods should be separated by two blank lines. |
| --- |
``` 

```

| The incompleteness of split lines must be made obvious |
| --- |
``` 
totalSum = a + b + c +
           d + e;

function (param1, param2,
          param3);

setText ("Long line split"
         "into two parts.");

for (int tableNo = 0; tableNo < nTables;
     tableNo += tableStep) {
  ...
}
```

## 4.4 Comments

| Use // for all comments, including multi-line comments. |
| --- |
``` 
// Comment spanning
// more than one line.
```

| Proprocessor *#defines* should have a comment to describe purpose and be grouped. |
| --- |
``` 
// Comment spanning
// more than one line.

    // Sizes
    #define LOGGER_MAX_MESSAGE_SIZE 10             // Maximum message size in the Logger Class
    #define LOGGER_MIN_MESSAGE SIZE 2              // Minimum message size in the Logger Class

    // Defaults
    #define LOGGER_DEFAULT_MESSAGE Hello World!    // If no message is set, use this.
```

| Class declaration comments should be in the following format. |
| --- |
```
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
```

| Class definition comments should be in the following format. |
| --- |
```
////////////////////////////////////////////
//// Example Class

// Public

ExampleClass::ExampleClass()
{

}


bool ExampleClass::ExampleMethod( int paramexample )
{

}


// Protected

bool ExampleClass::ExampleMethod2( int paramexample )
{

}
```

| Class members should be followed with a comment describing purpose. |
| --- |
```
    int _exampleMember;             // Description of member
```

| Method declaration comments should be in the following format. |
| --- |
```
    /** Brief description
     *  @param paramexample      Description of parameter
     *                           continued. Default = 1
        @returns                 Ture or false */
    bool ExampleMethod( int paramexample = 1 );
```

# 5. ESP Specifics

## 5.1 Hardware Defintion

| Place all hardware defintions and build parameters into header file named *device.h*. |
| --- |
```
#define DEVICE_RED_LED D1

static const char flag_BOARD [] PROGMEM = ESCAPEQUOTE(BOARD);
```

## 5.2 Memory Use

| Use PROGMEM where at all possible to move strings to program memory. |
| --- |
```
    static const char tagName [] PROGMEM = "This is a tag";

    Serial.println( PSTR("Hello World!") );
```

| Use FlashStrings where FlashString helpers are avaiable. |
| --- |
```
    String name;
    name += F("suffix);
```

| Make functions and methods be loaded at runtime, not preloaded into flash. The execptions are frequently run function or those in an interrupt. |
| --- |
```
bool ICACHE_FLASH_ATTR OTAUpdater::checkForUpdate()
{
    ...
}
```


# References
http://geosoft.no/development/cppstyle.html