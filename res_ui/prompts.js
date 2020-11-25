/*
     Author: Jake Leonard Date: 2011/09/12 
     All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.
*/
var beforePromptHTML = "";

function LeaveGame()
{
  window.DispatchImprobabilityEvent("PushGlobalAudioEvent", "Stop_audio");
  window.DispatchImprobabilityEvent("OnPushSpace", "main-menu-space.spc", false);
}

function RestartGame()
{
  window.DispatchImprobabilityEvent("PushGlobalAudioEvent", "Stop_audio");
  LoadGame();
}

function CreatePrompt(id)
{
  var promptCont = document.getElementById("prompt-container");
  if(promptCont.children.length != 0)
    return;
  
  var acceptAction, declineAction, headerText = "", promptText = "";
  
  switch(id)
  {
    case "main":
      headerText = "Confirm Leave";
      promptText = "Are you sure you want to go to the main menu? All unsaved progress will be lost!";
      acceptAction = 'LeaveGame()';
      declineAction = 'ClosePrompt()';
    break;
    case "reset":
      headerText = "Confirm Restart Game";
      promptText = "Are you sure you want to reset your progress?";
      acceptAction = 'RestartGame()';
      declineAction = 'ClosePrompt()';
    break;
    case "quit":
      headerText = "Confirm Quit Game";
      promptText = "Are you sure you want to end the game?";
      acceptAction = 'window.DispatchImprobabilityEvent("OnExit")';
      declineAction = 'ClosePrompt()';
    break;
  }
  
  var promptWindow = document.createElement("DIV");
  promptWindow.setAttribute("class", "prompt");
  promptWindow.setAttribute("id", "prompt");
  promptWindow.innerHTML = "<h4 class='prompt-header'>"+headerText+"</h4><p class='prompt-text'>"+promptText+"</p><div class='prompt-choices'><button class='text_button' name='prompt' onclick='eval("+acceptAction+");'>Yes</button><button class='text_button' name='prompt' onclick='eval("+declineAction+");'>No</button></div>";
  promptCont.appendChild(promptWindow);
  beforePromptHTML = document.getElementById("menu").innerHTML;
  document.getElementById("menu").innerHTML = "";
  TagButtonOptions();
}

function ClosePrompt()
{
  var thePrompt = document.getElementById("prompt");
  if(!thePrompt)
    return;
  
  var promptContain = document.getElementById("prompt-container");
  promptContain.removeChild(thePrompt);
  document.getElementById("menu").innerHTML = beforePromptHTML;
  beforePromptHTML = "";
  TagButtonOptions();
}

function CloseBrowserPrompt()
{
  window.open('', '_self', '');
  window.close();
}

function TagButtonOptions()
{
 for(var i=0; i < menuOptions.length; ++i)
 {
   menuOptions[i].id = i;
   menuOptions[i].setAttribute("class", "text_button");
 }
 
 if(!document.getElementById("prompt-container"))
 {
   var promptAdder = document.createElement("DIV");
   promptAdder.setAttribute("id", "prompt-container");
   document.body.appendChild(promptAdder);
 }
 selectedItem = 0;
 menuOptions[selectedItem].setAttribute("class", "text_button button_selected");
}

function PromptCoreStart()
{
  document.removeEventListener("DOMContentLoaded", PromptCoreStart, false);
  
  if(!document.getElementById("prompt-container"))
  {
   var promptAdder = document.createElement("DIV");
   promptAdder.setAttribute("id", "prompt-container");
   document.body.appendChild(promptAdder);
  }
  
  document.body.firstElementChild.setAttribute("id", "menu");
}

document.addEventListener("DOMContentLoaded", PromptCoreStart, false);
