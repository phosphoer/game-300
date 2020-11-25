/*
    Author: Jake Leonard Date: 2011/09/12 
    All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.
*/
function setFullscreenOption(newSetting)
{
  //alert(newSetting);
  if(newSetting == 1 || newSetting == true)
  {
    document.getElementById("FullscreenToggle").innerHTML = "True";
    document.getElementById("fullscreen").setAttribute("value", "true");
  }
  else
  {
    document.getElementById("FullscreenToggle").innerHTML = "False";
    document.getElementById("fullscreen").setAttribute("value", "false");
  }
}
function setAudioOption(newSetting)
{
  if(newSetting == true || newSetting == 1)
  {
    document.getElementById("audioToggle").innerHTML = "True";
    document.getElementById("audiomute").setAttribute("value", "true");
  }
  else
  {
    document.getElementById("audioToggle").innerHTML = "False";
    document.getElementById("audiomute").setAttribute("value", "false");
  }
}
function GetResolutionOption(width, height, num)
{
  var resList = document.getElementById("resolutions");
  var resolutionOption = document.createElement("OPTION");
  resolutionOption.setAttribute("res_width", width);
  resolutionOption.setAttribute("res_height", height);
  resolutionOption.value = num;
  resolutionOption.innerHTML = width+"x"+height;
  resList.appendChild(resolutionOption);
}
function GetCurrentResolution(curWidth, curHeight)
{
  var list = document.getElementById("resolutions").children;
  var curChild;
  for(var i = 0; i < list.length; ++i)
  {
    curChild = list[i];
    if(curChild.getAttribute("res_width") == curWidth && curChild.getAttribute("res_height") == curHeight)
    {
      document.getElementById("resolutions").selectedIndex = i;
      return;
    }
  }
}
function handlerFS(event)
{
  //alert(document.getElementById("fullscreen").getAttribute("value"));
  setFullscreenOption((document.getElementById("fullscreen").getAttribute("value") == "false") ? true : false);
}
function handlerAudio(event)
{
  setAudioOption((document.getElementById("audiomute").getAttribute("value") == "false") ? true : false);
}
function OptionsCoreStart()
{
  document.getElementById("audiomute").setAttribute("value", "false");
  document.getElementById("fullscreen").setAttribute("value", "false");
  
  /* Resolution */
  window.AddImprobabilityListener("OnRecieveResolution", GetResolutionOption);
  window.DispatchImprobabilityEvent("OnQueryResolutions");
  
  /* Get the current resolution */
  window.AddImprobabilityListener("OnRecieveCurrentResolution", GetCurrentResolution);
  window.DispatchImprobabilityEvent("OnQueryCurrentResolution");
  
  /* Fullscreen */
  window.AddImprobabilityListener("OnRecieveFullscreen", setFullscreenOption);
  window.DispatchImprobabilityEvent("OnQueryFullscreen");

  /* Set the buttons */
  document.getElementById("FullscreenToggle").addEventListener("click", handlerFS, false);
  document.getElementById("audioToggle").addEventListener("click", handlerAudio, false);
}
function OptionsSave()
{
  //var resolutionSelected = document.getElementById("resolutions").selectedIndex;
  //window.DispatchImprobabilityEvent("OnChangeResolutionIndex", new unsigned(resolutionSelected));
  
  var fullScreen = document.getElementById("fullscreen").getAttribute("value") == "true";
  //alert(fullScreen == "false" ? false : true);
  window.DispatchImprobabilityEvent("OnChangeFullscreen", fullScreen);
  
  var muteAudio = document.getElementById("audiomute").getAttribute("value") == "true";
  window.DispatchImprobabilityEvent("OnChangeAudioMute", muteAudio);

  window.DispatchImprobabilityEvent("OnChangeResolutionIndex", document.getElementById("resolutions").selectedIndex);
  
  if(!muteAudio)
    document.getElementById("audioClick").play();
  
  setTimeout("window.history.back();", 100);
}    
function GoBackToLast()
{
  var muteAudio = document.getElementById("audiomute").getAttribute("value");
  if(!muteAudio)
    document.getElementById("audioClick").play();
  window.history.back();
}
document.addEventListener("DOMContentLoaded", OptionsCoreStart, false);
