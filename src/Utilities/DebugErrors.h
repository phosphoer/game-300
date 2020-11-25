// Nicholas Baldwin
// 03-05-11
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
      
namespace Improbability
{

// Prints a nice error message to stderr, if using visual studio 
//   it will print to the error list/output console, if it is 
//   halting it will pop up a message box in Visual Studio.		
// expression - the expression that causes the error     
// file - the file the expression was found in    
// line - the line the expression was found on      
// formatMessage - an error message with printf format specifiers
void PrintError(char const *expression, 
                char const *file,
                int line,
                char const *format_message = 0, ...);

// Prints a nice error message to stderr, if using visual studio 
//   it will print to the error list/output console.
// format_message - an error message with printf format specifiers
void PrintToDebugConsole(char const *format_message, ...);

// Always crashes on an error whether in debug or release mode, when writing error
//   messages please keep users in mind, as they will see this
// exp - the expression to test
// NOTE: resolves to a possibly unnecessary if, please check if you actually need
//   to ship with this checking enabled
#define CriticalErrorIf(exp, ...)                                               \
  do {if((exp)){ PrintError(#exp, __FILE__, __LINE__,__VA_ARGS__ );             \
                 __debugbreak(); }} while(0)

// If we are in debug mode these macros do stuff otherwise they optimize out
#if _DEBUG

// From the mouth of Chris Peters
// The do/while blocks embedded here are ABSOLUTELY NECESSARY to prevent
// bizarre compilation errors. 
// Check out http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
// for more details.

// Throws an error in a nicely formatted message box and debug breaks when an 
//   expression is true
// exp - the expression to test for truth
// ... - an optional formatted message followed by the arguments passed to it
#define ErrorIf(exp, ...)                                                       \
  do {if((exp)){ PrintError(#exp, __FILE__, __LINE__,__VA_ARGS__ );             \
                 __debugbreak(); }} while(0)

#define Stringize_(a) #a
#define Stringize(a) Stringize_(a)

// Throws an error in a nicely formatted message box and debug breaks when an 
//   expression is equal to a certain result
// exp - the expression to test with
// res - the result to test against
// ... - an optional formatted message followed by the arguments passed to it
#define ErrorOnResult(exp, res, ...)                                                             \
  do { if((exp) == (res)){ PrintError(Stringize_(exp == res), __FILE__, __LINE__, __VA_ARGS__);   \
                           __debugbreak(); }} while(0)

// Throws an error in a nicely formatted message box, runs DXTrace, and debug 
//   breaks when a HRESULT fails, used with DirectX functions
// exp - the expression to test for failure
// ... - an optional formatted message followed by the arguments passed to it
#define HandleErrorResult(exp, ...)                                             \
  do {                                                                          \
    HRESULT hr = (exp);                                                         \
    if (FAILED(hr)){ PrintError(#exp, __FILE__, __LINE__,__VA_ARGS__ );         \
                     DXTraceW(__FILE__, __LINE__, hr, L#exp, TRUE); } } while(0)
 
#define Reminder(exp, ...)                                                      \
  PrintToDebugConsole(__FILE__##"("##Stringize(__LINE__)##") : Reminder: "##Stringize_(exp), __VA_ARGS__)

#else

//If the diagnostics are disabled all the debug functions
//are no ops, ErrorOnResult and HandleErrorResult should still call the function

#define ErrorIf(...) ((void)0)
#define ErrorOnResult(exp, ...) (exp)
#define HandleErrorResult(exp, ...) (exp)
#define Reminder(...) ((void)0)

#endif

}