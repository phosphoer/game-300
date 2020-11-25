# Author: Jake Leonard Date: 2011/09/12 
# All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

import Improb
import Message
import os
import sys

class DevOutRedir(object):
    def __init__(self):
        self.terminal = open('CONOUT$', 'wt')
    def closeTerminal(self):
        self.terminal.close();
    def write(self, message):
        Message.DispatchEvent("OnPyConsoleRecv", str(message))
        self.terminal.write(message)

def setUpConsole():
  sys.stdout = DevOutRedir()
  sys.stderr = DevOutRedir()

def closeConsoleHook():
  sys.stdout.closeTerminal();
  sys.stderr.closeTerminal();
  del sys.stdout
  del sys.stderr
  
  