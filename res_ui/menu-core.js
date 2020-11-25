/*
     Author: Jake Leonard Date: 2011/09/12 
     All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.
*/
var menuOptions = document.getElementsByClassName("text_button");
var selectedItem = 0;
var canPlaySounds;

function unsigned(numVal)
{
  // Conform to unsigned rules
  if(numVal <= 0)
    this.unsignnum = 0; // Can't really be undefined, so just give it zero.
  else
    this.unsignnum = numVal;
}

function playSound(soundName)
{
  if(!canPlaySounds)
    return;
  
  document.getElementById(soundName).play();
}

function LaunchMenuItem(id)
{
  playSound("audioClick");
  var evt = document.createEvent("MouseEvents");
  evt.initMouseEvent("click", true, true, window, 0, 0, 0, 0, 0, false, false, false, false, 0, null);
  menuOptions[id].dispatchEvent(evt);
}
function KeyListener(event)
{
  var keyCode = event.keyCode;
  if(event.ctrlKey || event.shiftKey || event.altKey || keyCode == 8)
  {
    event.preventDefault();
    return;
  }
  switch(keyCode)
  {
    case 37:
    case 38:
    case 39:
    case 40:
      menuOptions[selectedItem].setAttribute("class", "text_button");
      if(keyCode == 37 || keyCode == 38)
        --selectedItem;
      else if(keyCode == 39 || keyCode == 40)
        ++selectedItem;
      playSound("audioSelect");
    break;
  }
  
  if(selectedItem < 0)
    selectedItem = menuOptions.length-1;
  else if(selectedItem >= menuOptions.length)
    selectedItem = 0;
    
  menuOptions[selectedItem].setAttribute("class", "text_button button_selected");
  
  if(keyCode == 13)
    LaunchMenuItem(selectedItem);
    
  if(keyCode == 220)
    CreatePrompt("quit");
}

function ClickWatcher(event)
{
  if(event.target.className == "text_button button_selected")
    playSound("audioClick");
}

function MouseListener(event)
{
  if(event.target.className == "text_button" && typeof(menuOptions[selectedItem]) != "undefined")
  {
    menuOptions[selectedItem].setAttribute("class", "text_button");
    selectedItem = event.target.id;
    menuOptions[selectedItem].setAttribute("class", "text_button button_selected");
    playSound("audioSelect");
  }
}
function TagButtonOptions()
{
 for(var i=0; i < menuOptions.length; ++i)
 {
   menuOptions[i].id = i;
   menuOptions[i].setAttribute("class", "text_button");
 }
}

function loadAudio()
{
  if(document.getElementById("audioSelect"))
    return;
    
  var audioSelect = document.createElement("AUDIO");
  audioSelect.setAttribute("id", "audioSelect");
  audioSelect.setAttribute("src","audio/menu_blip.wav");
  
  var audioClick = document.createElement("AUDIO");
  audioClick.setAttribute("id", "audioClick");
  audioClick.setAttribute("src","audio/menu_click.wav");
  
  document.body.appendChild(audioSelect);
  document.body.appendChild(audioClick);
}

function GetAudioMuteSoundsOption(internalSettin)
{
  canPlaySounds = (internalSettin == 1) ? false : true;
  if(document.getElementById("audiomute"))
    setAudioOption(internalSettin);
}

function startGameNao()
{
  if(document.location.pathname.substring(document.location.pathname.lastIndexOf('/') + 1) == "pause-menu.html")
    window.DispatchImprobabilityEvent("OnResetSpace", "Tutorial-space.spc", false);
  else
  {
    document.getElementById("mainMenuBGM").muted = true;
    window.DispatchImprobabilityEvent("OnPushSpace", "Tutorial-space.spc", false);
  }
}

function LoadGame()
{
  window.DispatchImprobabilityEvent("OnNewGame");
}

function MenuCoreStart()
{
  window.AddImprobabilityListener("OnNewGameStart", startGameNao);
  window.AddImprobabilityListener("OnRecieveAudioMute", GetAudioMuteSoundsOption);
  window.DispatchImprobabilityEvent("OnQueryAudioMute");
  
  document.removeEventListener("DOMContentLoaded", MenuCoreStart, false);
  loadAudio();
  TagButtonOptions();
}

document.addEventListener("mousedown", ClickWatcher, true);
document.addEventListener("DOMContentLoaded", MenuCoreStart, false);
document.addEventListener("keydown", KeyListener, false);
document.addEventListener("mousemove", MouseListener, false);
