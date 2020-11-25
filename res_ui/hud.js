/*
     Author: Jake Leonard Date: 2011/09/12 
     All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.
*/

var hudObject = "hud";
var numCoinCollected;

function CreateHudObject(name, cssType, text)
{
  if(document.getElementById(name)) 
    return;
    
  var container = (cssType.match("centertext") != null) ? document.createElement("CENTER") : document.createElement("DIV");
  container.setAttribute("id", name);
  container.setAttribute("class", "hudObject "+cssType);
  var textHolder = document.createElement("P");
  textHolder.innerHTML = text;
  container.appendChild(textHolder);
  
  document.getElementById(hudObject).insertBefore(container, document.getElementById("afterMe"));
}

function CreateCoins(numCoins)
{
  numCoinCollected = 0;
  // Remove previous coins.
  var prevCoins = document.getElementsByClassName("coin");
  while(prevCoins[0] && prevCoins.length != 0)
    document.getElementById("coinHolder").removeChild(prevCoins[0]);
    
  for(var i = 0; i < numCoins; ++i)
  {
    var newCoin = document.createElement("a");
    newCoin.setAttribute("class", "coin unlit");
    newCoin.innerHTML = "<!-- Coin Body -->";
    document.getElementById("coinHolder").appendChild(newCoin);
  }
}

function UpdateCoinNumber(newNumber)
{
  if(newNumber == numCoinCollected)
    return;
  
  var numLit = 0;
  var coins = document.getElementsByClassName("coin");
  for(var i = 0; i < coins.length; ++i)
  {
    if(numLit != newNumber)
    {
      coins[i].setAttribute("class", "coin lit");
      ++numLit;
    }    
    else
      coins[i].setAttribute("class", "coin unlit");
  }
  numCoinCollected = newNumber;
}

function ToggleCoinVisible(displayCoins)
{
  if(displayCoins == false)
    document.getElementById("coinHolder").setAttribute("style", "display: none !important;");
  else
    document.getElementById("coinHolder").setAttribute("style", "");
}

function MoveHudObject(name, x_pos, y_pos)
{
  if(!document.getElementById(name)) 
    return;
  
  document.getElementById(name).setAttribute("style", "left: "+x_pos+"%; top: "+y_pos+"%;");
  //document.getElementById(name).setAttribute("style", "left: 0%; top: 50%;");
}

function UpdateHudText(name, text)
{
  if(!document.getElementById(name)) return;
  document.getElementById(name).firstChild.innerHTML = text;
}

function CloseHudObject(name)
{
  if(!document.getElementById(name))
    return;
    
  document.getElementById(hudObject).removeChild(document.getElementById(name));
}

function KeyListener(event)
{
  var keyCode = event.keyCode;
  if(event.ctrlKey || event.shiftKey || event.altKey || keyCode == 8)
  {
    event.preventDefault();
    return;
  }
}

function HUDCoreStart()
{
  numCoinCollected = 0;
  window.AddImprobabilityListener("OnCreateCoins", CreateCoins);
  window.AddImprobabilityListener("OnUpdateCoinNumber", UpdateCoinNumber);
  window.AddImprobabilityListener("OnToggleCoinVisible", ToggleCoinVisible);
  window.AddImprobabilityListener("HUDCreate", CreateHudObject);
  window.AddImprobabilityListener("HUDMove", MoveHudObject);
  window.AddImprobabilityListener("HUDTextUpdate", UpdateHudText);
  window.AddImprobabilityListener("HUDClose", CloseHudObject);
  window.DispatchImprobabilityEvent("OnHUDReady");
}
document.addEventListener("keydown", KeyListener, false);
document.addEventListener("DOMContentLoaded", HUDCoreStart, false);
