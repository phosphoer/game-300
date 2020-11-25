// David Evans
// 2011/06/17
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "ResourceManager.h"
#include "../FileIO/FileIO.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(ResourceManager);

void ResourceManager::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnLoadPass);
}

void ResourceManager::Load(char const *root_dir)
{
  m_root_directory = root_dir;

  // Get our list of supported extensions
  Serializer serializer(m_root_directory + "\\extensions.ext");
  ErrorIf(!serializer.IsGood(), "Failed to open %s", "extensions.ext");
  serializer.Begin();
  while (serializer.IsGood())
  {
    String name = serializer.GetName();
    if (name == "Extension")
    {
      String ext, resource, pass_name;
      Read(serializer, ext, "Ext");
      Read(serializer, resource, "Resource");
      Read(serializer, pass_name, "PassName");
      AddExtension(ext, resource, pass_name);
    }
    serializer.Iterate();
  }
}

void ResourceManager::Unload()
{
  auto it = m_resources.Begin();
  auto end = m_resources.End();
  for (; it != end; ++it)
  {
    it->second->Unload();
    delete it->second;
  }
  m_resources.Clear();

  auto it_arr = m_resource_arrays.Begin();
  auto it_arr_end = m_resource_arrays.End();
  for (; it_arr != it_arr_end; ++it_arr)
  {
    delete it_arr->second;
  }
  m_resource_arrays.Clear();
}

void ResourceManager::AddExtension(char const *ext, char const *class_name, char const *pass_name)
{
  auto it = m_loaders.Find(ext);
  ErrorIf(it != m_loaders.End(), "The extension %s is already registered.", ext);
  Loader loader;
  loader.m_name = class_name;
  loader.m_pass = pass_name;
  m_loaders[ext] = loader;
}

Array<Resource *> const * ResourceManager::GetResourcesOfType(char const *extension)
{
  auto it = m_resource_arrays.Find(extension);
  if (it == m_resource_arrays.End())
    return 0;

  return it->second;
}

void ResourceManager::OnLoadPass(char const *name)
{
  RecursivelyLoadDirectory(m_root_directory, name);
}

void ResourceManager::RecursivelyLoadDirectory(String const &dir, String const &message)
{
  std::vector<String> directories;
  GetDirectoryList(dir, directories);
  String current_path = dir;

  int num_directories = directories.size();
  for (int i = 0; i < num_directories; ++i)
  {
    current_path = dir + "\\" + directories[i];
    RecursivelyLoadDirectory(current_path, message);
  }

  LoadDirectory(dir, message);
}

void ResourceManager::LoadDirectory(String const &dir, String const &message)
{
  String current_path = dir;
  std::vector<String> files;
  GetFileList(dir, files);

  // Iterate over every file in the directory
  int num_files = files.size();
  for (int i = 0; i < num_files; ++i)
  {
    // Get the extension of this file and look up a loader for it
    String ext = GetFileExtension(files[i]);
    auto it = m_loaders.Find(ext);
    if (it != m_loaders.End())
    {
      // Skip this resource if it doesn't belong to this pass
      if (it->second.m_pass.GetLength() == 0 || it->second.m_pass != message)
        continue;

      // Make sure this isn't a duplicate resource
      auto res_it = m_resources.Find(files[i]);
      ErrorIf(res_it != m_resources.End(), "%s is a duplicate resource!", res_it->first.GetCString());

      // Clone the loader as a new resource
      // and load the file
      Resource *res = Meta::Create<Resource>(it->second.m_name);
      res->m_name = it->second.m_name;
      res->m_filename = res_it->first;
      res->SetPassName(it->second.m_pass);
      res->Load(current_path + "\\" + files[i]);

      // Add the resource to the resource map
      m_resources[files[i]] = res;

      // Add the resource to the map of arrays
      auto array_it = m_resource_arrays.Find(ext);
      Array<Resource*> * resource_array = 0;
      if (array_it == m_resource_arrays.End())
      {
        resource_array = new Array<Resource*>();
        m_resource_arrays[ext] = resource_array;
      }
      else
        resource_array = array_it->second;

      resource_array->PushBack(res);
    }
  }
}

}
