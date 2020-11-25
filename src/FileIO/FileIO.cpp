// David Evans
// 2011/06/17
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "FileIO.h"

#pragma warning (disable : 4996)

namespace Improbability
{

bool GetFileList(String const &dir, std::vector<String> &files)
{
  String path = dir;
  char back = path.GetBack();
  if (back != '\\' && back != '*')
    path += "\\*";
  else if (back == '\\')
    path += "*";

  WIN32_FIND_DATA find_data;
  HANDLE find_session = FindFirstFile(path.GetCString(), &find_data);

  if (find_session == INVALID_HANDLE_VALUE)
    return false;

  if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    files.push_back(find_data.cFileName);
  while (FindNextFile(find_session, &find_data))
  {
    if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      files.push_back(find_data.cFileName);
  }

  return true;
}

bool GetFilesOfExtension(String const &dir, String const &ext, std::vector<String> &files)
{
  std::vector<String> all_files;
  if (!GetFileList(dir, all_files))
    return false;

  int size = all_files.size();
  for (int i = 0; i < size; ++i)
  {
    if (all_files[i].Find(ext) != String::k_end)
      files.push_back(all_files[i]);
  }

  return true;
}

bool GetDirectoryList(String const &dir, std::vector<String> &directories)
{
  String path = dir;
  char back = path[path.GetLength()-1];
  if (back != '\\' && back != '*')
    path += "\\*";
  else if (back == '\\')
    path += "*";

  WIN32_FIND_DATA find_data;
  HANDLE find_session = FindFirstFile(path.GetCString(), &find_data);

  if (find_session == INVALID_HANDLE_VALUE)
    return false;

  if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    if (std::strcmp(find_data.cFileName, ".") != 0 && std::strcmp(find_data.cFileName, "..") != 0)
      directories.push_back(find_data.cFileName);
  while (FindNextFile(find_session, &find_data))
  {
    if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      if (std::strcmp(find_data.cFileName, ".") != 0 && std::strcmp(find_data.cFileName, "..") != 0)
        directories.push_back(find_data.cFileName);
  }

  return true;
}

String GetFileExtension(String const &filename)
{
  int dot_pos = filename.FindLastOf(".");
  if (dot_pos == String::k_end)
    return "";

  return String(filename, dot_pos, filename.GetLength());
}

String GetFilenameFromPath(String const &filepath)
{
  int pos = filepath.FindLastOf("\\");
  if (pos == String::k_end)
    return "";

  return String(filepath, pos, filepath.GetLength());
}

char* ReadFromFile(String const &file_path, int &size)
{
  // Open the file
  std::FILE *file = 0;
  file = std::fopen(file_path.GetCString(), "rb");
  if (!file)
    return 0;

  // Get file size
  std::fseek(file, 0, SEEK_END);
  size = std::ftell(file);
  std::fseek(file, 0, SEEK_SET);

  // Read file into buffer
  char *file_buffer = new char[size + 1];
  std::fread(file_buffer, size, 1, file);
  file_buffer[size] = 0;

  // Close the file
  std::fclose(file);

  return file_buffer;
}

bool WriteToFile(String const &file_path, char const *buffer, int size)
{
  std::FILE *file = 0;
  file = std::fopen(file_path.GetCString(), "wb");
  if (!file)
    return false;

  std::fwrite(buffer, 1, size, file);
  std::fclose(file);

  return true;
}

}