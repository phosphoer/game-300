// Jake Leonard
// 2012/01/23
// All content (c) 2012 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PyMessaging.h"
#include "PyClass.h"
#include "../Utilities/DynamicValue.h"
#include "../Core/EngineCore.h"

/************************************************************************/
/* Python Binding                                                       */
/************************************************************************/
static PyObject* PyDispatchEvent(PyObject* self, PyObject* args)
{
  Improbability::PyGeneralObj *pyObjInfo = (Improbability::PyGeneralObj*)self;
  Improbability::Variable varArgs[5];
  int passedArgs = PyTuple_GET_SIZE(args);

  PyObject *arg;

  for(int i = 0; i < passedArgs; ++i)
  {
    arg = PyGetItemFromTuple(args, i);

    if (arg == NULL)
    {
      varArgs[i] = NULL;
      continue;
    }
    if (PyLong_CheckExact(arg)) // int
      varArgs[i] = Improbability::Variable(PyGetInt(arg));
    else if (PyBool_Check(arg)) // bool
      varArgs[i] = Improbability::Variable(PyGetBool(arg));
    else if (PyLong_Check(arg)) // unsigned
      varArgs[i] = Improbability::Variable(PyGetUInt(arg));
    else if (PyFloat_CheckExact(arg)) // float
      varArgs[i] = Improbability::Variable(PyGetFloat(arg));
    else if (PyFloat_Check(arg)) // double
      varArgs[i] = Improbability::Variable(PyGetDouble(arg));
    else if (PyUnicode_Check(arg)) // string
      varArgs[i] = Improbability::Variable(Improbability::String(PyGetString(arg)));
    else // Check
    {
      Improbability::PyGeneralObj* pyObj = reinterpret_cast<Improbability::PyGeneralObj*>(arg);
      Improbability::DynamicValue tempObj(pyObj->obj, pyObj->className);
      varArgs[i] = tempObj;
    }

  }

  Improbability::String eventType(varArgs[0]);
  /* You want to ignore the event type one */
  switch(passedArgs - 1)
  {
    default:
    case 0:
      Improbability::PyMessaging::DispatchEvent(eventType);
    break;
    case 1:
      Improbability::PyMessaging::DispatchEvent(eventType, varArgs[1]);
    break;
    case 2:
      Improbability::PyMessaging::DispatchEvent(eventType, varArgs[1], varArgs[2]);
    break;
    case 3:
      Improbability::PyMessaging::DispatchEvent(eventType, varArgs[1], varArgs[2], varArgs[3]);
    break;
    case 4:
      Improbability::PyMessaging::DispatchEvent(eventType, varArgs[1], varArgs[2], varArgs[3], varArgs[4]);
    break;
  }

  Py_RETURN_NONE;
}

static PyObject* PyRegisterEvent(PyObject* self, PyObject* args)
{
  Improbability::String eventType, onEvent, arbCode;
  int passedArgs = PyTuple_GET_SIZE(args);
  eventType = PyGetString(PyGetItemFromTuple(args, 0));

  switch(passedArgs)
  {
    default:
    case 1:
      Improbability::PyMessaging::RegisterEvent(eventType);
    break;
    case 2:
      onEvent = PyGetString(PyGetItemFromTuple(args, 1));

      /* Check if member is in the object's method dictionary */
      if(PyObject_HasAttrString(self, onEvent.GetCString()) == 0)
        Improbability::PyMessaging::RegisterEvent(eventType, onEvent);
      else
        Improbability::PyMessaging::RegisterEvent(eventType, onEvent, self);
    break;
    case 3:
      onEvent = PyGetString(PyGetItemFromTuple(args, 1));
      arbCode = PyGetString(PyGetItemFromTuple(args, 2));
      Improbability::PyMessaging::RegisterEvent(eventType, onEvent, self, arbCode);
    break;
  }

  Py_RETURN_NONE;
}

static PyMethodDef MessagingMethods[] = {
  {"RegisterEvent",  PyRegisterEvent, METH_VARARGS, "Registers an Event Listener"},
  {"DispatchEvent",  PyDispatchEvent, METH_VARARGS, "Dispatches a system message"},
  {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef messagingModule = {
   PyModuleDef_HEAD_INIT,
   "Message",
   "Allows Messaging cross communication between Python and C++",
   -1,
   MessagingMethods
};

PyMODINIT_FUNC PyInit_messaging(void)
{
  return PyModule_Create(&messagingModule);
}

/************************************************************************/
/* Python Messaging                                                     */
/************************************************************************/

namespace Improbability
{
  void PyMessaging::MakePyModule()
  {
    PyImport_AppendInittab("Message", PyInit_messaging);
  }

  void PyMessaging::Update()
  {
    sm_IsFiring = true;
    while(!sm_pyEventQueue.empty())
    {
      PyEvent top = sm_pyEventQueue.front();
      switch(top.args.size())
      {
        case 0:
          g_engine_core->DispatchEvent(top.eventName);
        break;
        case 1:
          g_engine_core->DispatchEvent(top.eventName, top.args[0]);
        break;
        case 2:
          g_engine_core->DispatchEvent(top.eventName, top.args[0], top.args[1]);
        break;
        case 3:
          g_engine_core->DispatchEvent(top.eventName, top.args[0], top.args[1], top.args[2]);
        break;
        case 4:
          g_engine_core->DispatchEvent(top.eventName, top.args[0], top.args[1], top.args[2], top.args[3]);
        break;
      }
      sm_pyEventQueue.pop();
    }
    sm_IsFiring = false;
  }

  /************************************************************************/
  /* PyMessaging CaptureEvents                                            */
  /************************************************************************/
  void PyMessaging::CaptureEvent(const String &msg)
  {
    if(sm_pyEvents.Find(msg) == sm_pyEvents.End())
      return;

    ListenerList listeners = (sm_pyEvents[msg]);

    /* We'll need to just get it from the runtime */
    for(auto it = listeners.begin(); it != listeners.end(); ++it)
      it->Execute();
  }

  void PyMessaging::CaptureEvent(const String &msg, Variable &var1)
  {
    if(msg == "OnPyConsoleSend")
    {
      String pyCode = var1;
      PyRun_SimpleString(pyCode.GetCString());
      return;
    }
    
    if(sm_pyEvents.Find(msg) == sm_pyEvents.End())
      return;

    ListenerList listeners = (sm_pyEvents[msg]);

    for(auto it = listeners.begin(); it != listeners.end(); ++it)
      it->Execute(var1);
  }

  void PyMessaging::CaptureEvent(const String &msg, Variable &var1, Variable &var2 )
  {
    if(sm_pyEvents.Find(msg) == sm_pyEvents.End())
      return;

    ListenerList listeners = (sm_pyEvents[msg]);

    for(auto it = listeners.begin(); it != listeners.end(); ++it)
      it->Execute(var1, var2);
  }

  void PyMessaging::CaptureEvent(const String &msg, Variable &var1, Variable &var2, Variable &var3 )
  {
    if(sm_pyEvents.Find(msg) == sm_pyEvents.End())
      return;

    ListenerList listeners = (sm_pyEvents[msg]);

    for(auto it = listeners.begin(); it != listeners.end(); ++it)
      it->Execute(var1, var2, var3);
  }

  void PyMessaging::CaptureEvent(const String &msg, Variable &var1, Variable &var2, Variable &var3, Variable &var4 )
  {
    if(sm_pyEvents.Find(msg) == sm_pyEvents.End())
      return;

    ListenerList listeners = (sm_pyEvents[msg]);
    for(auto it = listeners.begin(); it != listeners.end(); ++it)
      it->Execute(var1, var2, var3, var4);
  }

  /************************************************************************/
  /* PyMessaging DispatchEvents                                           */
  /************************************************************************/
  void PyMessaging::DispatchEvent(const String &msg)
  {
    PyEvent newEvent;
    newEvent.eventName = msg;
    sm_pyEventQueue.push(newEvent);
  }
  void PyMessaging::DispatchEvent(const String &msg, Variable const &var1 )
  {
    PyEvent newEvent;
    newEvent.eventName = msg;
    newEvent.args.push_back(var1);
    sm_pyEventQueue.push(newEvent);
  }
  void PyMessaging::DispatchEvent(const String &msg, Variable const &var1, Variable const &var2 )
  {
    PyEvent newEvent;
    newEvent.eventName = msg;
    newEvent.args.push_back(var1);
    newEvent.args.push_back(var2);
    sm_pyEventQueue.push(newEvent);
  }
  void PyMessaging::DispatchEvent(const String &msg, Variable const &var1, Variable const &var2, Variable const &var3 )
  {
    PyEvent newEvent;
    newEvent.eventName = msg;
    newEvent.args.push_back(var1);
    newEvent.args.push_back(var2);
    newEvent.args.push_back(var3);
    sm_pyEventQueue.push(newEvent);
  }
  void PyMessaging::DispatchEvent(const String &msg, Variable const &var1, Variable const &var2, Variable const &var3, Variable const &var4 )
  {
    PyEvent newEvent;
    newEvent.eventName = msg;
    newEvent.args.push_back(var1);
    newEvent.args.push_back(var2);
    newEvent.args.push_back(var3);
    newEvent.args.push_back(var4);
    sm_pyEventQueue.push(newEvent);
  }

  /************************************************************************/
  /* PyMessaging Registering Events                                       */
  /************************************************************************/
  void PyMessaging::RegisterEvent(const String &eventName)
  {
    ListenerList& eventList = sm_pyEvents[eventName];
    eventList.push_back(PyListener(eventName));
  }

  void PyMessaging::RegisterEvent( const String &eventName, String pyCode )
  {
    ListenerList& eventList = sm_pyEvents[eventName];
    eventList.push_back(PyListener(eventName, pyCode));
  }

  void PyMessaging::RegisterEvent( const String &eventName, String pyFunc, PyObject* obj )
  {
    ListenerList& eventList = sm_pyEvents[eventName];
    eventList.push_back(PyListener(eventName, pyFunc, obj));
  }

  void PyMessaging::RegisterEvent( const String &eventName, String pyFunc, PyObject* obj, String code )
  {
    ListenerList& eventList = sm_pyEvents[eventName];
    eventList.push_back(PyListener(eventName, pyFunc, obj, code));
  }

  /************************************************************************/
  /*  PyMessaging Queues and Containers                                   */
  /************************************************************************/
  bool PyMessaging::sm_IsFiring = false;
  std::queue<PyMessaging::PyEvent> PyMessaging::sm_pyEventQueue;
  HashMap<String, PyMessaging::ListenerList> PyMessaging::sm_pyEvents;


  /************************************************************************/
  /* PyListener Constructors                                              */
  /************************************************************************/
  PyMessaging::PyListener::PyListener( String eName )
  {
    eventType = Log;
    eventName = eName;
    object = NULL;
  }

  PyMessaging::PyListener::PyListener( String eName, String code )
  {
    eventName = eName;
    eventType = ExecCode;
    pyCode = code;
    object = NULL;
  }

  PyMessaging::PyListener::PyListener( String eName, String pyFunc, PyObject* obj )
  {
    Py_INCREF(obj);
    object = obj;
    eventType = ExecMember;
    pyFunction = pyFunc;
    eventName = eName;
  }

  PyMessaging::PyListener::PyListener( String eName, String pyFunc, PyObject* obj, String code )
  {
    Py_INCREF(obj);
    object = obj;
    eventType = ExecMemberWFunc;
    pyFunction = pyFunc;
    eventName = eName;
    pyCode = code;
  }

  /************************************************************************/
  /* PyListener Deconstructor                                             */
  /************************************************************************/
  PyMessaging::PyListener::~PyListener()
  {
    if(eventType == ExecMember || eventType == ExecMemberWFunc)
      Py_DECREF(object);
  }

  /************************************************************************/
  /* PyListener Executes                                                  */
  /************************************************************************/
  void PyMessaging::PyListener::Execute()
  {
    //std::cout << "Event Fired: " << eventName.GetCString() << std::endl;
    if(eventType == Log)
      return;

    PyObject* args = NULL, *returnVal = NULL;

    if(eventType == ExecMember || eventType == ExecMemberWFunc)
    {
      PyObject* funcObject = PyObject_GetAttrString(object, pyFunction);
      
      if(funcObject && PyCallable_Check(funcObject))
      {
        PyTuple_SetItem(args, 0, object);

        returnVal = PyObject_CallObject(funcObject, args);
      }

      Py_DECREF(args);
    }

    if(eventType == ExecCode || eventType == ExecMemberWFunc)
    {
      String relatedArgs = "event = '" + eventName + "';";
      PyRun_SimpleString(relatedArgs);
      PyRun_SimpleString(pyCode);
    }

    if (returnVal)
      Py_DECREF(returnVal);
  }

  void PyMessaging::PyListener::Execute( Variable var1 )
  {
    //std::cout << "Event Fired: " << eventName.GetCString() << std::endl;
    if(eventType == Log)
      return;

    PyObject* args = NULL, *returnVal = NULL;

    if(eventType == ExecMember || eventType == ExecMemberWFunc)
    {
      PyObject* funcObject = PyObject_GetAttrString(object, pyFunction);

      if(funcObject && PyCallable_Check(funcObject))
      {
        PyTuple_SetItem(args, 0, object);
        PyTuple_SetItem(args, 1, var1);

        returnVal = PyObject_CallObject(funcObject, args);
      }

      Py_DECREF(args);
    }

    if(eventType == ExecCode || eventType == ExecMemberWFunc)
    {
      String relatedArgs = "event = '" + eventName + "';";
      PyRun_SimpleString(relatedArgs);
      PyRun_SimpleString(pyCode);
    }

    if (returnVal)
      Py_DECREF(returnVal);
  }

  void PyMessaging::PyListener::Execute( Variable var1, Variable var2 )
  {
    //std::cout << "Event Fired: " << eventName.GetCString() << std::endl;
    if(eventType == Log)
      return;

    PyObject* args = NULL, *returnVal = NULL;

    if(eventType == ExecMember || eventType == ExecMemberWFunc)
    {
      PyObject* funcObject = PyObject_GetAttrString(object, pyFunction);

      if(funcObject && PyCallable_Check(funcObject))
      {
        PyTuple_SetItem(args, 0, object);
        PyTuple_SetItem(args, 1, var1);
        PyTuple_SetItem(args, 2, var2);

        returnVal = PyObject_CallObject(funcObject, args);
      }

      Py_DECREF(args);
    }

    if(eventType == ExecCode || eventType == ExecMemberWFunc)
    {
      String relatedArgs = "event = '" + eventName + "';";
      PyRun_SimpleString(relatedArgs);
      PyRun_SimpleString(pyCode);
    }

    if (returnVal)
      Py_DECREF(returnVal);
  }

  void PyMessaging::PyListener::Execute( Variable var1, Variable var2, Variable var3 )
  {
    //std::cout << "Event Fired: " << eventName.GetCString() << std::endl;
    if(eventType == Log)
      return;

    PyObject* args = NULL, *returnVal = NULL;

    if(eventType == ExecMember || eventType == ExecMemberWFunc)
    {
      PyObject* funcObject = PyObject_GetAttrString(object, pyFunction);

      if(funcObject && PyCallable_Check(funcObject))
      {
        PyTuple_SetItem(args, 0, object);
        PyTuple_SetItem(args, 1, var1);
        PyTuple_SetItem(args, 2, var2);
        PyTuple_SetItem(args, 3, var3);

        returnVal = PyObject_CallObject(funcObject, args);
      }

      Py_DECREF(args);
    }

    if(eventType == ExecCode || eventType == ExecMemberWFunc)
    {
      String relatedArgs = "event = '" + eventName + "';";
      PyRun_SimpleString(relatedArgs);
      PyRun_SimpleString(pyCode);
    }

    if (returnVal)
      Py_DECREF(returnVal);
  }

  void PyMessaging::PyListener::Execute( Variable var1, Variable var2, Variable var3, Variable var4 )
  {
    //std::cout << "Event Fired: " << eventName.GetCString() << std::endl;
    if(eventType == Log)
      return;

    PyObject* args = NULL, *returnVal = NULL;

    if(eventType == ExecMember || eventType == ExecMemberWFunc)
    {
      PyObject* funcObject = PyObject_GetAttrString(object, pyFunction);

      if(funcObject && PyCallable_Check(funcObject))
      {
        PyTuple_SetItem(args, 0, object);
        PyTuple_SetItem(args, 1, var1);
        PyTuple_SetItem(args, 2, var2);
        PyTuple_SetItem(args, 3, var3);
        PyTuple_SetItem(args, 4, var4);

        returnVal = PyObject_CallObject(funcObject, args);
      }

      Py_DECREF(args);
    }

    if(eventType == ExecCode || eventType == ExecMemberWFunc)
    {
      String relatedArgs = "event = '" + eventName + "';";
      PyRun_SimpleString(relatedArgs);
      PyRun_SimpleString(pyCode);
    }

    if (returnVal)
      Py_DECREF(returnVal);
  }

}
