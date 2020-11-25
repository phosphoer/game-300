/*------------------------------------------------------------------------------
Filename: CefHandler.cpp
Author  : Ryan Rohrer
Purpose : Handles the CEF interface
Date    : 2011/11/29
      All content (C) 2011 DigiPen (USA) Corporation, all rights reserved.
------------------------------------------------------------------------------*/ 
#include "Precompiled.h"
#include "CefHandler.h"
#include <string>
#include <sstream>
#pragma warning( push )
#pragma warning( disable : 4100 )
#include <cef_runnable.h>

namespace Improbability
{
  void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
  {
    AutoLock lock_scope(this);

    if(!mBrowser.get())
    {
      // We need to keep the main child window, but not popup windows
      mBrowser = browser;
      mBrowserHwnd = browser->GetWindowHandle();
      mBrowser->SetSize( PET_VIEW, mWidth, mHeight );
    }
  }

  bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
  {
    REQUIRE_UI_THREAD();
    return false;
  }

  void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
  {
    REQUIRE_UI_THREAD();
    mBrowser = NULL;
  }

  void ClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame)
  {
    REQUIRE_UI_THREAD();
  }

  void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    int httpStatusCode)
  {
    REQUIRE_UI_THREAD();

  }

  bool ClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString& failedUrl,
    CefString& errorText)
  {
    REQUIRE_UI_THREAD();
    if(errorCode == ERR_CACHE_MISS) {
      // Usually caused by navigating to a page with POST data via back or
      // forward buttons.
      errorText = "<html><head><title>Expired Form Data</title></head>"
        "<body><h1>Expired Form Data</h1>"
        "<h2>Your form request has expired. "
        "Click reload to re-submit the form data.</h2></body>"
        "</html>";
    } else {
      // All other messages.
      std::stringstream ss;
      ss <<       "<style>body{background:rgb(128,128,128,0.5);}</style><html><head><title>Load Failed</title></head>"
        "<body><h1>Load Failed</h1>"
        "<h2>Load of URL " << std::string(failedUrl) <<
        " failed with error code " << static_cast<int>(errorCode) <<
        ".</h2></body>"
        "</html>";
      errorText = ss.str();
    }

    return false;
  }

  void ClientHandler::OnNavStateChange(CefRefPtr<CefBrowser> browser,
    bool canGoBack,
    bool canGoForward)
  {
    REQUIRE_UI_THREAD();
  }

  bool ClientHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
    const CefString& message,
    const CefString& source,
    int line)
  {
    REQUIRE_UI_THREAD();

    return false;
  }


  void ClientHandler::OnJSBinding(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Value> object)
  {
    REQUIRE_UI_THREAD();
    AutoLock lock_scope(this);

    //clear the current list of registered callbacks
    mListenerMap.Clear();
    mMessageQueue.clear();

    //this will handle both functions
    CefRefPtr<CefV8Handler> handler = new MsgV8Handler(this);

    //bind the listener function
    char const * listenFuncName = "AddImprobabilityListener";
    CefRefPtr<CefV8Value> listenFunc = CefV8Value::CreateFunction( listenFuncName, handler );
    object->SetValue( listenFuncName, listenFunc, V8_PROPERTY_ATTRIBUTE_NONE );

    //bind the trigger function
    char const * triggerFuncName = "DispatchImprobabilityEvent";
    CefRefPtr<CefV8Value> triggerFunc = CefV8Value::CreateFunction( triggerFuncName, handler );
    object->SetValue( triggerFuncName, triggerFunc, V8_PROPERTY_ATTRIBUTE_NONE );
  }

  bool ClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> parentBrowser,
    const CefPopupFeatures& popupFeatures,
    CefWindowInfo& windowInfo,
    const CefString& url,
    CefRefPtr<CefClient>& client,
    CefBrowserSettings& settings)
  {
    REQUIRE_UI_THREAD();
    parentBrowser.get()->GetMainFrame()->LoadURL( url );
    return true;
  }

  bool ClientHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefRequest> request,
    CefString& redirectUrl,
    CefRefPtr<CefStreamReader>& resourceStream,
    CefRefPtr<CefResponse> response,
    int loadFlags)
  {
    REQUIRE_UI_THREAD();
    return false;
  }

  void ClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& url)
  {
    REQUIRE_UI_THREAD();
  }

  void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
    const CefString& title)
  {
    REQUIRE_UI_THREAD();
  }

  ClientHandler::ClientHandler( int Width, int Height )
    : mWidth( Width ),
    mHeight( Height ),
    mBrowserHwnd(0),
    mDirtyTexture(false),
    mPopupBuffer(0),
    mPopupBufferSize(0)
  {
    mBuffer = new unsigned char[mWidth * mHeight * 4];
  }

  ClientHandler::~ClientHandler()
  {
    delete [] mBuffer;
  }


  bool ClientHandler::GetViewRect(CefRefPtr<CefBrowser> browser,
    CefRect& rect)
  {

    // The simulated screen and view rectangle are the same. This is necessary
    // for popup menus to be located and sized inside the view.
    rect.x = rect.y = 0;
    rect.width = mWidth;
    rect.height = mHeight;
    return true;
  }

  bool ClientHandler::GetScreenRect(CefRefPtr<CefBrowser> browser,
    CefRect& rect)
  {
    return GetViewRect(browser, rect);
  }

  bool ClientHandler::GetScreenPoint(CefRefPtr<CefBrowser> browser,
    int viewX,
    int viewY,
    int& screenX,
    int& screenY)
  {
    // Convert the point from view coordinates to actual screen coordinates.
    POINT screen_pt = {viewX, viewY};
    MapWindowPoints(mBrowserHwnd, HWND_DESKTOP, &screen_pt, 1);
    screenX = screen_pt.x;
    screenY = screen_pt.y;
    return true;
  }

  void ClientHandler::OnPopupShow(CefRefPtr<CefBrowser> browser,
    bool show) 
  {
    if ( !show )
    {
      mPopupRect.Set( 0,0,0,0 );
      delete [] mPopupBuffer;
      mPopupBuffer = 0;
      mPopupBufferSize = 0;
    }
  }

  void ClientHandler::OnPopupSize(CefRefPtr<CefBrowser> browser,
    const CefRect& rect) 
  {
    mPopupRect = rect;
  }

  void ClientHandler::OnPaint(CefRefPtr<CefBrowser> browser,
    PaintElementType type,
    const CefRect& dirtyRect,
    const void* buffer) 
  {
    REQUIRE_UI_THREAD();
    AutoLock lock_scope(this);
    mDirtyTexture = true;

    if ( type == PET_POPUP )
    {
      unsigned destSize = mPopupRect.width * mPopupRect.height * 4;
      if ( !mPopupBuffer && destSize != 0 )
      {
        mPopupBuffer = new unsigned char[destSize];
        mPopupBufferSize = destSize;
      }
      ErrorIf(!mPopupBufferSize == destSize, "Bad pop up buffer size!");
      std::memcpy( mPopupBuffer, buffer, destSize );
    }
    else
    {
      mBrowser->GetSize( PET_VIEW, mcefSizeW, mcefSizeH );
      if(mcefSizeH == mHeight && mcefSizeW == mWidth)
        std::memcpy( mBuffer, buffer, mWidth * mHeight * 4 );
    }

    if (mPopupBuffer)
    {
      unsigned char * basePtr = mBuffer + ((mWidth * 4) * mPopupRect.y + mPopupRect.x*4);
      unsigned char * popupPtr = mPopupBuffer;

      for (unsigned i = 0; i < (unsigned)mPopupRect.height; ++i, basePtr += (mWidth * 4), popupPtr += (mPopupRect.width * 4) )
      {
        std::memcpy( basePtr, popupPtr, mPopupRect.width * 4);
      }
    }
  }

  void ClientHandler::OnCursorChange(CefRefPtr<CefBrowser> browser,
    CefCursorHandle cursor) 
  {

  }

  void ClientHandler::ResizeHandler( int Width, int Height )
  {
    AutoLock lock_scope(this);
    mWidth = Width;
    mHeight = Height;
    delete [] mBuffer;
    delete [] mPopupBuffer;
    mPopupBuffer = 0;
    mPopupBufferSize = 0;
    mBuffer = new unsigned char[mWidth * mHeight * 4];
    mBrowser->SetSize( PET_VIEW, mWidth, mHeight );
  }

  void ClientHandler::GetTexture( Improbability::Texture * FillBuffer )
  {
    if(FillBuffer == NULL)
      return;

    AutoLock lock_scope(this);
    if ( mDirtyTexture ) FillBuffer->Fill( mBuffer );
    mDirtyTexture = false;
  }

  /* Hardcoded because we need a return value */
  bool ClientHandler::IsClickOnHTMLElement(int x, int y)
  {
    JSMessage message;
    auto found = mListenerMap.Find("OnEditorClick");
    CefRefPtr<CefV8Context> v8 = found->second.jsContext;
    CefRefPtr<CefV8Value> retval;
    CefString exception;

    //convert the arguments to JS Arguments
    CefV8ValueList arguments;
    arguments.push_back( CefV8Value::CreateInt( (int)x ) );
    arguments.push_back( CefV8Value::CreateInt( (int)y ) );

    if((found->second.jsFunction)->ExecuteFunctionWithContext(v8,NULL, arguments, retval, exception))
    {
      CriticalErrorIf(exception.length() != 0, "JavaScript Exception:\n%s", CefStringToIDString(exception).GetCString()); 
    }

    return retval->GetBoolValue();
    
  }
  void CallJsMethod( CefRefPtr<CefV8Context> const & v8, CefRefPtr<CefV8Value> func, JSMessage message )
  {
    //call the JS function
    CefRefPtr<CefV8Value> retval;
    CefString exception;

    //convert the arguments to JS Arguments
    CefV8ValueList arguments;
    for ( unsigned i = 0; i < message.args.size(); ++i )
    {
      if(CefBuildJSValueFromVariable( arguments, message.args[i] ) == false)
        return;
    }

    if ( func->ExecuteFunctionWithContext(v8,/*v8->GetGlobal()*/NULL, arguments, retval, exception) ) 
    {
      CriticalErrorIf(exception.length() != 0, "JavaScript Exception:\n%s", CefStringToIDString(exception).GetCString()); 
    }
  }

  void ClientHandler::SyncronizeMessages( std::vector<JSMessage> & SentMsgs, std::vector<JSMessage> & RecievedMsgs )
  {
    AutoLock lock_scope(this);

    RecievedMsgs = mMessageQueue;

    for ( unsigned i = 0; i < SentMsgs.size(); ++i )
    {
      //look up the message name in the JS map
      auto found = mListenerMap.Find( SentMsgs[i].name );
      if ( found == mListenerMap.End() ) continue;

      //queue the function to get called
      CefRefPtr<CefV8Context> v8 = found->second.jsContext;//mBrowser->GetMainFrame()->GetV8Context();
      CefPostTask( TID_UI, NewCefRunnableFunction( CallJsMethod, v8, found->second.jsFunction, SentMsgs[i]) );
    }

    mMessageQueue.clear();
  }

  void ClientHandler::AddListener( Improbability::String const & Str, JSListener const & Listen )
  {
    AutoLock lock_scope( this );
    mListenerMap.Insert( Str, Listen );
  }

  void ClientHandler::AddIDMessage( JSMessage const & Msg )
  {
    AutoLock lock_scope( this );
    mMessageQueue.push_back( Msg );
  }

  bool MsgV8Handler::Execute( const CefString& name, 
    CefRefPtr<CefV8Value> object, 
    const CefV8ValueList& arguments, 
    CefRefPtr<CefV8Value>& retval, 
    CefString& exception )
  {
    if ( name == "AddImprobabilityListener" )
    {
      JSListener listen;

      //error handle
      CriticalErrorIf( arguments.size() != 2, "In order to listen to Improbability, you must pass 2 arguments.\n"
        "1. The Message Name\n"
        "2. The callback function" );
      CriticalErrorIf( !arguments[0]->IsString(), "Argument 0 must be a string." );
      CriticalErrorIf( !arguments[1]->IsFunction(), "Argument 1 must be a function." );

      //register the listener
      CefString s = arguments[0]->GetStringValue();
      listen.jsFunction = arguments[1];
      listen.jsContext = CefV8Context::GetCurrentContext();
      mClientHandler->AddListener( CefStringToIDString(s), listen );
      return true;
    }
    else if ( name == "DispatchImprobabilityEvent" )
    {
      JSMessage msg;
      CriticalErrorIf( arguments.size() < 1, "To Trigger a message you must at least have one argument." );
      CriticalErrorIf( arguments.size() > 5, "To Trigger a message you must at most have five arguments." );

      // Check to see message isn't a failure
      if(CefBuildVariableFromJsFunc(arguments[0]).GetType() == "Null")
        return false;

      CriticalErrorIf( !arguments[0]->IsString(), "The first message argument must be a string!" );

      //get the name of the message
      CefString name = arguments[0]->GetStringValue();
      msg.name = CefStringToIDString( name );

      //for the rest of the arguments, add them to the message queue
      for ( unsigned i = 1; i < arguments.size(); ++i )
      {
        Improbability::Variable v = CefBuildVariableFromJsFunc( arguments[i] );
        if(v.GetType() == "Null")
          return false;

        msg.args.push_back( v );
      }
      mClientHandler->AddIDMessage( msg );
      return true;
    }
    else
    {
      //ErrorIf( true, "There was a horrible JavaScript Error." ); 
      return false;
    }
  }

#pragma warning( pop )

  Improbability::String CefStringToIDString( CefString const & rhs )
  {
    Improbability::String retval(rhs.ToString().c_str() );
    return retval;
  }

  Improbability::Variable CefBuildVariableFromJsFunc( CefRefPtr<CefV8Value> const & Argument )
  {
    if ( Argument->IsString() )
    {
      CefString s = Argument->GetStringValue();
      return Improbability::Variable( CefStringToIDString( s ) );
    }
    else if ( Argument->IsInt())
    {
      return Improbability::Variable(Argument->GetIntValue() );
    }
    else if ( Argument->IsDouble())
    {
      return Improbability::Variable( float( Argument->GetDoubleValue() ) );
    }
    else if ( Argument->IsBool() )
    {
      return Improbability::Variable( Argument->GetBoolValue() );
    }
    else if(Argument->HasValue("unsignnum")) // Javascript unsigned values
    {
      unsigned temp = Argument->GetValue("unsignnum")->GetIntValue();
      return Improbability::Variable(temp);
    }
    else
    {
      //CriticalErrorIf( true, "A message argument in JS was Invalid" );
      return Improbability::Variable();
    }
  }

  bool CefBuildJSValueFromVariable( CefV8ValueList & arguments, Improbability::Variable &var )
  {
    if ( var.GetType() == "String" )
    {
      CefRefPtr<CefV8Value> v = CefV8Value::CreateString( ((Improbability::String)var).GetCString() );
      arguments.push_back( v );
    }
    else if ( var.GetType() == "int" )
    {
      CefRefPtr<CefV8Value> v = CefV8Value::CreateInt( (int)var );
      arguments.push_back( v );
    }
    else if ( var.GetType() == "unsigned" )
    {
      CefRefPtr<CefV8Value> v = CefV8Value::CreateInt( (unsigned)var );
      arguments.push_back( v );
    }
    else if ( var.GetType() == "float" )
    {
      CefRefPtr<CefV8Value> v = CefV8Value::CreateDouble( (float)var );
      arguments.push_back( v );
    }
    else if ( var.GetType() == "double" )
    {
      CefRefPtr<CefV8Value> v = CefV8Value::CreateDouble( (double)var );
      arguments.push_back( v );
    }
    else if ( var.GetType() == "bool" )
    {
      CefRefPtr<CefV8Value> v = CefV8Value::CreateInt( (bool)var );
      arguments.push_back( v );
    }
    else
    {
      // Failed to create the JS function
      return false;
    }
    return true;
  }
}
