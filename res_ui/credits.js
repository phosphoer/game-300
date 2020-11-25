/*
     Author: Jake Leonard Date: 2011/09/12 
     All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.
*/

function goBack()
{
  window.history.back();
}

function KeyListener(event)
{
  var keyCode = event.keyCode;
  if(event.ctrlKey || event.shiftKey || event.altKey || keyCode == 8)
  {
    event.preventDefault();
    return;
  }
  if(keyCode == 32 || keyCode == 27)
    window.history.back();
}

function stopScroll()
{
  document.getElementById("scrollingCredits").stop();
  setTimeout("goBack();", 150);
}

document.addEventListener("keydown", KeyListener, false);
