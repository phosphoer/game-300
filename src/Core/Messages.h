// Register messages in this file

// Object events
NewMessage(OnObjectInitialized)
NewMessage(OnObjectDeleted)

// Core Events
NewMessage(OnPushSpace)
NewMessage(OnPopSpace)
NewMessage(OnChangeSpace)
NewMessage(OnResetSpace)
NewMessage(OnSpacePushed)
NewMessage(OnSpaceReady)
NewMessage(OnSpaceReturn)
NewMessage(OnExit)

// Resource events
NewMessage(OnLoadPass)

// Tile events
NewMessage(OnNewGame)
NewMessage(OnNewGameStart)
NewMessage(OnWorldUnloading)
NewMessage(OnWorldLoaded)
NewMessage(SetActiveWorld)
NewMessage(SetWorldAxis)
NewMessage(OnAxisInitalize)
NewMessage(OnAnchorTileEnter)
NewMessage(OnAnchorTileExit)
NewMessage(OnRotateToTarget)
NewMessage(OnTopToggle)
NewMessage(OnSideToggle)
NewMessage(OnFrontToggle)
NewMessage(OnAnchorToggle)
NewMessage(OnRotateStart)
NewMessage(OnRotateComplete)
NewMessage(OnAnchorSmushBegin)
NewMessage(OnAnchorSmushComplete)
NewMessage(OnPlayerDeathToggle)
NewMessage(OnPlayerDrop)
NewMessage(OnUpdateLayerCount)
NewMessage(GetTileComponent)
NewMessage(ReturnTC)
NewMessage(SetPosition)

// Window Events
NewMessage(OnWindowClose)
NewMessage(OnDeviceReset)
NewMessage(OnQueryCurrentResolution)
NewMessage(OnRecieveCurrentResolution)
NewMessage(OnQueryResolutions)
NewMessage(OnRecieveResolution)
NewMessage(OnQueryFullscreen)
NewMessage(OnRecieveFullscreen)
NewMessage(OnChangeResolution)
NewMessage(OnChangeResolutionIndex)
NewMessage(OnChangeFullscreen)

// Coin events
NewMessage(OnCreateCoins)
NewMessage(OnUpdateCoinNumber)
NewMessage(OnToggleCoinVisible)
NewMessage(OnCoinCollection)
NewMessage(CoinCollected)

// Game events
NewMessage(OnAxisChange)
NewMessage(OnAxisChangeComplete)
NewMessage(OnPlayerGrounded)
NewMessage(OnPlayerNotGrounded)
NewMessage(OnHelpActivated)
NewMessage(OnPlayerHeadCollision)
NewMessage(OnPlayerSideCollision)
NewMessage(OnAnchorEvent)

// Input events
NewMessage(OnActionPress)
NewMessage(OnActionDown)
NewMessage(OnActionRelease)
NewMessage(OnCharPress)
NewMessage(OnKeyPress)
NewMessage(OnKeyDown)
NewMessage(OnKeyRelease)
NewMessage(OnMousePress)
NewMessage(OnMouseDown)
NewMessage(OnMouseRelease)
NewMessage(OnMouseMove)
NewMessage(OnMouseDrag)
NewMessage(OnMouseScroll)
NewMessage(OnQueryControllerConfig)
NewMessage(ActionTest)
NewMessage(OnChangeControllerConfig)
NewMessage(OnRecieveControllerConfig)

// Sound stuff
NewMessage(OnRecieveAudioMute)
NewMessage(OnQueryAudioMute)
NewMessage(OnChangeAudioMute)
NewMessage(PushAudioEvent)
NewMessage(PushGlobalAudioEvent)

// Animation events
NewMessage(OnStartAnimation)
NewMessage(OnAnimationInitialize)

// Don't use these events
NewMessage(OnCharDownWin)
NewMessage(OnKeyDownWin)
NewMessage(OnKeyUpWin)
NewMessage(OnMouseMoveWin)
NewMessage(OnMouseScrollWin)
NewMessage(OnMouseLeftButtonDownWin)
NewMessage(OnMouseMiddleButtonDownWin)
NewMessage(OnMouseRightButtonDownWin)
NewMessage(OnMouseLeftButtonUpWin)
NewMessage(OnMouseMiddleButtonUpWin)
NewMessage(OnMouseRightButtonUpWin)

// Dev console
NewMessage(OnToggleDevConsole)
NewMessage(OnPyConsoleSend)
NewMessage(OnPyConsoleRecv)

// HUD
NewMessage(OnHUDReady)
NewMessage(HUDCreate)
NewMessage(HUDMove)
NewMessage(HUDTextUpdate)
NewMessage(HUDClose)

NewMessage(DisplayHudItem)
NewMessage(IsHudItemDisplayed)
NewMessage(UpdateHudText)
NewMessage(HideHudItem)
NewMessage(ChangeHudItemPos)


// Editor
NewMessage(OnEditorChangeTool)
NewMessage(OnEditorObjClick)
NewMessage(OnEyedropObj)
NewMessage(OnEditorTextureAdded)
NewMessage(OnEditorSpriteAdded)
NewMessage(OnEditorClick)
NewMessage(OnEditorReady)
NewMessage(OnEditorRequestObjects)


// Saving and Loading
NewMessage(OnGameSave)
NewMessage(OnGamePauseSave)
NewMessage(QueryHasSaves)
NewMessage(HasGameSaves)
NewMessage(OnLoadRequested)
NewMessage(SaveSuccessful)
NewMessage(OnLoadRequesteded)
NewMessage(GameReadyToLoad)
