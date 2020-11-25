// David Evans
// 2011/06/17
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

// Gets a list of files from the specified directory and fills the vector with strings
bool GetFileList(String const &dir, std::vector<String> &files);

// Gets a list of files from the specified directory with the specified extension and fills the vector
// with strings
bool GetFilesOfExtension(String const &dir, String const &ext, std::vector<String> &files);

// Gets a list of directories from the specified directory and fills the vector with strings
bool GetDirectoryList(String const &dir, std::vector<String> &directories);

// Gets the extension of a filename
String GetFileExtension(String const &filename);

// Gets the file name from a path
String GetFilenameFromPath(String const &filepath);

// Reads a file into a buffer
char* ReadFromFile(String const &file_path, int &size);

// Writes a buffer to a file
bool WriteToFile(String const &file_path, char const *buffer, int size);

}
