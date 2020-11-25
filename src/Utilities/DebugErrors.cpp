// Nicholas Baldwin
// 03-05-11
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "precompiled.h"
#include "DebugErrors.h"

namespace Improbability
{

void PrintError(char const *expression, char const *file, int line, 
                char const *format_message, ...)
{
  const int k_buffer_size = 1024;
  char full_message[k_buffer_size];

  int offset = sprintf_s(full_message,"%s(%d) : ", file , line );	

  // if there is a message
  if (format_message) 
  {
    // get the format arguments
    va_list args; 
    va_start(args, format_message);

    // add the formatted message
    vsnprintf_s(full_message + offset, k_buffer_size - offset, _TRUNCATE, 
                format_message, args);
    // end the format arguments
    va_end(args); 
  }
  else
  {
    // print a message with the expression in it
    sprintf_s(full_message + offset, k_buffer_size - offset, "expression = %s", 
              expression);
  }

  // output to standard error
  std::cerr << full_message << std::endl;

  // Print to visual studio output window
  OutputDebugStringA(full_message);
  OutputDebugStringA("\n");

  // Display a message box to the user
  MessageBoxA(NULL, full_message, "Error", 0);
}

void PrintToDebugConsole(char const *format_message, ...)
{
  const int k_buffer_size = 1024;
  char full_message[k_buffer_size];

  // get the format arguments
  va_list args; 
  va_start(args, format_message);

  // add the formatted message
  vsnprintf_s(full_message, k_buffer_size, _TRUNCATE, format_message, args);

  // end the format arguments
  va_end(args); 

  // output to standard error
  std::cerr << full_message << std::endl;

  // Print to visual studio output window
  OutputDebugStringA(full_message);
  OutputDebugStringA("\n");
}

}
