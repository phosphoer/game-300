// David Evans
// 2011/06/17
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Resource.h"
#include "../Meta/Meta.h"
#include "../Containers/HashMap.h"
#include "../Containers/Array.h"

namespace Improbability
{

class MetaComposition;

class ResourceManager
{
public:
  class Loader
  {
  public:
    String m_name;
    String m_pass;
  };

  typedef Array<Resource*> ResourceArray;
  typedef HashMap<String, Resource*> ResourceMap;
  typedef HashMap<String, ResourceArray *> ResourceArrayMap;
  typedef HashMap<String, Loader> LoaderMap;

  static void BindMeta(MetaComposition*);

  // Recursively load a directory of resources
  // root_dir - The directory to load
  void Load(char const *root_dir);

  // Unloads all data from the resource manager
  void Unload();

  // Register an extension (e.g. ".ext") with a particular type of resource
  // ext - The extension of the resource you are registering
  // class_name - The resource type you are registering
  void AddExtension(char const *ext, char const *class_name, char const *pass_name);

  // Get a resource by its filename, and casts to the proper type for you
  // T - Object type of the file
  // filename - Filename of the resource you want
  // return - Pointer to the resource
  template <typename T>
  T* GetResource(char const *filename)
  {
    auto it = m_resources.Find(filename);
    ErrorIf(it == m_resources.End(), "Resource %s not found!", filename);
    return reinterpret_cast<T*>(it->second);
  }

  // Get an array of all resources for a single extension
  // extension - String extension of the resource type, e.g., ".txt"
  Array<Resource *> const * GetResourcesOfType(char const *extension);

  // Messages
  void OnLoadPass(char const *name);

  ResourceMap* GetResourceMap() { return &m_resources;}

private:
  // This helper function recursively loads all files in a directory
  // dir - The directory to recursively load
  void RecursivelyLoadDirectory(String const &dir, String const &message);

  // Another helper function which actually loads files in a directory
  // Called by RecursivelyLoadDirectory on each folder
  // dir - The folder to load files from
  void LoadDirectory(String const &dir, String const &message);

private:
  // The directory which the resource manager begins recursive loading from
  String m_root_directory;

  // A map of extensions to resource names
  LoaderMap m_loaders;

  // A map of filenames to the respective loaded resource
  ResourceMap m_resources;

  // Map of extensions to all resources of that type
  ResourceArrayMap m_resource_arrays;
};

}
