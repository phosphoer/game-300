/*
     Author: Jake Leonard Date: 2012/09/12 
     All content (c) 2012 DigiPen (USA) Corporation, all rights reserved.
*/

function unsigned(numVal)
{
  // Conform to unsigned rules
  if(numVal <= 0)
    this.unsignnum = 0; // Can't really be undefined, so just give it zero.
  else
    this.unsignnum = numVal;
}

var lastTool;
function chooseTool(event)
{
  if(event.target != lastTool)
  {
    lastTool.setAttribute("class", "tool");
    lastTool = event.target;
    lastTool.setAttribute("class", "selected-tool");
    window.DispatchImprobabilityEvent("OnEditorChangeTool", lastTool.getAttribute("tool"));
  }
}

function ElementClickCheck(locX, locY)
{
  /* Get the element in that area */
  var object = document.elementFromPoint(locX, locY);
  if(object.tagName == "html" || object.tagName == "body")
    return false;
  else
  {
    var evt = document.createEvent("MouseEvents");
    evt.initMouseEvent("click", false, true, window, 0, 0, 0, 0, 0, false, false, false, false, 0, null);
    object.dispatchEvent(evt);
    return true;
  }
}

function exitEditor(event)
{
  event.preventDefault();
  window.DispatchImprobabilityEvent("OnPushSpace", "main-menu-space.spc", false);
  return false;
}

var curObjSelection = null;
function EyeDropperSet(id)
{
  var newObj = document.getElementById("editor-"+id);
  if(curObjSelection != null)
    curObjSelection.setAttribute("class", "");
    
  curObjSelection = newObj;
  curObjSelection.setAttribute("class", "selected-obj");
}

function setCurrentObject(event)
{
  event.preventDefault();
  if(event.target.hasAttribute("objID") == false)
    return;
  
  var objectID = event.target.getAttribute("objID");
  if(curObjSelection != null)
    curObjSelection.setAttribute("class", "");
    
  curObjSelection = event.target;
  curObjSelection.setAttribute("class", "selected-obj");
  
  window.DispatchImprobabilityEvent("OnEditorObjClick", objectID);
}

var oldImgPath;
var curSub = 0;
function EditorTextureAdd(id, imgPath, imgH, imgW, subdiv)
{
  if(imgPath == oldImgPath && curSub < subdiv)
    subdiv += 1;
  
  if(imgPath != oldImgPath)
  {
    oldImgPath = imgPath;
    curSub = 0;
  }
  
  var newObject = document.createElement("a");
  newObject.setAttribute("id", "editor-"+id);
  newObject.setAttribute("objID", id);
  newObject.style.background-image = imgPath;
  newObject.style.background-position =  ((imgW / subdiv) * curSub * -1) + "px 0px";
  newObject.style.width = imgW/subdiv + "px";
  newObject.style.height = imgH + "px";
  document.getElementById("objectList").appendChild(newObject);
}

function EditorSpriteAdd(id, imgPath, imgH, imgW)
{
  var newObject = document.createElement("a");
  newObject.setAttribute("id", "editor-"+id);
  newObject.setAttribute("objID", id);
  newObject.style.background-image = imgPath;
  newObject.style.width = imgW + "px";
  newObject.style.height = imgH + "px";
  document.getElementById("objectList").appendChild(newObject);
}

/******************************** EVENT LISTENERS ********************************/
function EventBlock(event)
{
  event.preventDefault();
  return false;
}
function KeyBlock(event)
{
  var keyCode = event.keyCode;
  if(event.ctrlKey || event.shiftKey || event.altKey || keyCode == 8 || keyCode == 38 || keyCode == 40 || keyCode == 37 || keyCode == 39)
    event.preventDefault();
    
  return false;
}

function EditorCoreInit()
{
  lastTool = document.getElementById("noSelection");
  document.getElementById("toolsbar").addEventListener("click", chooseTool, false);
  document.getElementById("objectList").addEventListener("click", setCurrentObject, false);
  document.getElementById("exitbutton").addEventListener("click", exitEditor, false);
  window.AddImprobabilityListener("OnEditorTextureAdded", EditorTextureAdd);
  window.AddImprobabilityListener("OnEditorSpriteAdded", EditorSpriteAdd);
  window.AddImprobabilityListener("OnEditorClick", ElementClickCheck);
  window.AddImprobabilityListener("OnEyedropObj", EyeDropperSet);
  window.DispatchImprobabilityEvent("OnEditorReady");
  window.DispatchImprobabilityEvent("OnEditorChangeTool", "selector");
}


document.addEventListener("click", EventBlock, false);
document.addEventListener("mousewheel", EventBlock, false);
document.addEventListener("mousedown", EventBlock, false);
document.addEventListener("keydown", KeyBlock, false);

document.addEventListener("DOMContentLoaded", EditorCoreInit, false);
