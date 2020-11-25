/*
     Author: Jake Leonard Date: 2011/09/12 
     All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.
*/
function PauseTheAudio()
{
  window.DispatchImprobabilityEvent("PushGlobalAudioEvent", "Pause_audio");
}

function ResumeGame()
{
  window.DispatchImprobabilityEvent("PushGlobalAudioEvent", "Resume_audio");
  window.DispatchImprobabilityEvent("OnPopSpace", false);
}

function SaveGame()
{
  window.DispatchImprobabilityEvent("OnGamePauseSave");
}

function ResumeKeyListener(event)
{
  var keyCode = event.keyCode;
  if(keyCode == 27)
  {
    ResumeGame();
    return;
  }
}

document.addEventListener("keydown", ResumeKeyListener, false);
document.addEventListener("DOMContentLoaded", PauseTheAudio, false);
