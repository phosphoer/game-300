/*------------------------------------------------------------------------------
Filename: CefHandler.h
Author  : Ryan Rohrer
Purpose : handles the CEF interface
Date    : 2011/11/29
      All content (C) 2011 DigiPen (USA) Corporation, all rights reserved.
------------------------------------------------------------------------------*/
#pragma once
#pragma warning( push )
#pragma warning( disable : 4100 )
#pragma warning( disable : 4481 )
#include <cef.h>
#include "../Graphics/Texture.h"

#define REQUIRE_UI_THREAD()
//#define REQUIRE_UI_THREAD()   ErrorIf(!CefCurrentlyOn(TID_UI), "This call should be on the UI Thread")
#define REQUIRE_IO_THREAD()   ErrorIf(!CefCurrentlyOn(TID_IO), "This Call Should be on the IO Thread" )
#define REQUIRE_FILE_THREAD() ErrorIf(!CefCurrentlyOn(TID_FILE), "This call should be on the FILE Thread" )

namespace Improbability
{
  struct JSMessage
  {
    String name;
    std::vector<Variable> args;
  };

  String CefStringToIDString( CefString const & rhs );
  Variable CefBuildVariableFromJsFunc( CefRefPtr<CefV8Value> const & Argument );
  bool CefBuildJSValueFromVariable( CefV8ValueList & arguments, Variable &var );

  struct JSListener
  {
    CefRefPtr<CefV8Context> jsContext;
    CefRefPtr<CefV8Value> jsFunction;
  };
  class ClientHandler : public CefClient,
                        public CefLifeSpanHandler,
                        public CefLoadHandler,
                        public CefRequestHandler,
                        public CefDisplayHandler,
                        public CefJSBindingHandler,
                        public CefRenderHandler
  {
  public:
    ClientHandler(int Width, int Height );
    virtual ~ClientHandler();

    // CefClient methods
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefJSBindingHandler> GetJSBindingHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE { return this; }

    // CefLifeSpanHandler methods
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> parentBrowser,
                               const CefPopupFeatures& popupFeatures,
                               CefWindowInfo& windowInfo,
                               const CefString& url,
                               CefRefPtr<CefClient>& client,
                               CefBrowserSettings& settings) OVERRIDE;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    // CefLoadHandler methods
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame) OVERRIDE;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           int httpStatusCode) OVERRIDE;
    virtual bool OnLoadError(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             ErrorCode errorCode,
                             const CefString& failedUrl,
                             CefString& errorText) OVERRIDE;

    // CefRequestHandler methods
    virtual bool OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefRequest> request,
                                      CefString& redirectUrl,
                                      CefRefPtr<CefStreamReader>& resourceStream,
                                      CefRefPtr<CefResponse> response,
                                      int loadFlags) OVERRIDE;


    // CefDisplayHandler methods
    virtual void OnNavStateChange(CefRefPtr<CefBrowser> browser,
                                  bool canGoBack,
                                  bool canGoForward) OVERRIDE;
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 const CefString& url) OVERRIDE;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                               const CefString& title) OVERRIDE;
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                  const CefString& message,
                                  const CefString& source,
                                  int line) OVERRIDE;

    // CefJSBindingHandler methods
    virtual void OnJSBinding(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefV8Value> object) OVERRIDE;

    // CefRenderHandler methods
    virtual bool GetViewRect(CefRefPtr<CefBrowser> browser,
                             CefRect& rect) OVERRIDE;
    virtual bool GetScreenRect(CefRefPtr<CefBrowser> browser,
                               CefRect& rect) OVERRIDE;
    virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
                                int viewX,
                                int viewY,
                                int& screenX,
                                int& screenY) OVERRIDE;
    virtual void OnPopupShow(CefRefPtr<CefBrowser> browser,
                             bool show) OVERRIDE;
    virtual void OnPopupSize(CefRefPtr<CefBrowser> browser,
                             const CefRect& rect) OVERRIDE;
    virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                         PaintElementType type,
                         const CefRect& dirtyRect,
                         const void* buffer) OVERRIDE;
    virtual void OnCursorChange(CefRefPtr<CefBrowser> browser,
                                CefCursorHandle cursor) OVERRIDE;

    //helpers
    CefRefPtr<CefBrowser> GetBrowser() {return mBrowser; }
    CefWindowHandle GetBrowserHwnd() {return mBrowserHwnd; }
    void GetTexture(Texture * FillBuffer);
    void ResizeHandler(int Width, int Height);

    //sent messages GOES TO javascript, recieved comes FROM
    void SyncronizeMessages(std::vector<JSMessage> & SentMsgs,
                            std::vector<JSMessage> & RecievedMsgs );

    bool IsClickOnHTMLElement(int x, int y);

  protected:
    friend class MsgV8Handler;
    void AddListener( String const & Str, JSListener const & Listen ); 
    void AddIDMessage( JSMessage const & Msg );
    CefRefPtr<CefBrowser> mBrowser;

    // The main frame window handle
    CefWindowHandle mBrowserHwnd;
    int mWidth, mHeight;
    // Checking for when resolution or fullscreen switch
    int mcefSizeW, mcefSizeH;
    unsigned char * mBuffer;
    unsigned char * mPopupBuffer;
    unsigned mPopupBufferSize;
    CefRect mPopupRect;
    bool mDirtyTexture;

    //stores the messages that are going to get send TO c++
    std::vector<JSMessage> mMessageQueue;
    HashMap<String,JSListener> mListenerMap;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(ClientHandler);
    // Include the default locking implementation.
    IMPLEMENT_LOCKING(ClientHandler);
  };

  class MsgV8Handler : public CefV8Handler
  {
  public:
    MsgV8Handler( ClientHandler * h):mClientHandler(h) {}

    virtual bool Execute(const CefString& name,
                         CefRefPtr<CefV8Value> object,
                         const CefV8ValueList& arguments,
                         CefRefPtr<CefV8Value>& retval,
                         CefString& exception) OVERRIDE;
    // Provide the reference counting implementation for this class.
    IMPLEMENT_REFCOUNTING(MsgV8Handler);
  private:
    ClientHandler *mClientHandler;
  };
}

#pragma warning( pop )