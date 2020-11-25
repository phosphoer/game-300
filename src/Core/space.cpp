// David Evans
// 2011/05/19
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Space.h"
#include "Timer.h"
#include "../FileIO/Serializer.h"
#include "../Graphics/DebugDraw.h"

namespace Improbability
{

HashMap<String, int> Space::s_initialize_count;

MakeMeta(Space);

void Space::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

Space::Space():
  m_profile_update(0)
{
}

void Space::Initialize(String const & space_name)
{
  // We don't want to perform the one-time initialization of systems
  // if it has already been done

  // Initialize all systems
  auto it = m_system_array.begin();
  auto end_it = m_system_array.end();
  for (; it != end_it; ++it)
  {
    System *system = *it;

    if (s_initialize_count[system->GetClassName()] == 0)
      system->InitializeGlobal();
  }

  // Post initialize all systems
  it = m_system_array.begin();
  for (; it != end_it; ++it)
  {
    System *system = *it;

    if (s_initialize_count[system->GetClassName()] == 0)
      system->PostInitializeGlobal();
  }

  // We always initalize the current space
  // Initialize all systems
  it = m_system_array.begin();
  end_it = m_system_array.end();
  for (; it != end_it; ++it)
  {
    System *system = *it;
    system->InitializeInstance();
  }

  // Post initialize all systems
  it = m_system_array.begin();
  for (; it != end_it; ++it)
  {
    System *system = *it;
    system->PostInitializeInstance();

    s_initialize_count[system->GetClassName()] += 1;
  }
}

void Space::Update(float dt)
{
  Timer timer;
  m_profile_update += dt;

  // Update all systems
  auto it = m_system_array.begin();
  auto end_it = m_system_array.end();
  for (; it != end_it; ++it)
  {
    // Start the timer before updating system
    timer.GetDt();

    // Update the system
    System *system = *it;
    system->UpdateInstance(dt);

    // Store the update time
    float system_time = timer.GetDt();
    m_profiler.UpdateSystemTime(system->GetClassName(), system_time);
  }

  // Draw our timings
  DebugDraw::AddArrow(Vector3(-0.5f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), dt / 0.016f);

  // Print out timings
//   if (m_profile_update > 5.0f)
//   {
//     m_profile_update = 0.0f;
//     float total_time = 0.0f;
//     std::printf("\n");
//     it = m_system_array.begin();
//     for (; it != end_it; ++it)
//     {
//       System *system = *it;
//       char const *name = system->GetClassName().GetCString();
//       float sys_time = m_profiler.GetSystemTime(name);
//       total_time += sys_time;
//       std::printf("Profiler: %s: %f seconds\n", name, sys_time);
//     }
//     std::printf("Profiler: Total Time: %f seconds (%f fps)\n", total_time, 1.0f / total_time);
//     std::printf("\n");
//   }
}

void Space::Uninitialize()
{
  // We always uninitalize the current space
  // Uninitialize all systems
  auto end_it = m_system_array.rend();
  for (auto it = m_system_array.rbegin(); it != end_it; ++it)
  {
    System *system = *it;
    system->UninitializeInstance();
  }

  // We want to wait until the last space before performing a global
  // uninitialization

  // Uninitialize all systems
  end_it = m_system_array.rend();
  for (auto it = m_system_array.rbegin(); it != end_it; ++it)
  {
    System *system = *it;
    String name = system->GetClassName();

    if (s_initialize_count[name] == 1)
    {
      system->UninitializeGlobal();
      delete system;
    }

    s_initialize_count[name] -= 1;
  }
}

void Space::AddSystem(System *system)
{
  ErrorIf(m_system_map.Find(system->GetClassName()) != m_system_map.End(), "Space::AddSystem: Duplicate System");
  m_system_map[system->GetClassName()] = system;
  m_system_array.push_back(system);
  
  auto it = s_initialize_count.Find(system->GetClassName());
  if (it == s_initialize_count.End())
    s_initialize_count[system->GetClassName()] = 0;
}

Space* Space::Clone(bool full_copy)
{
  // Make a new space
  Space *new_space = new Space();

  // Clone all existing systems into the new space
  auto it = m_system_array.begin();
  auto end = m_system_array.end();
  for (; it != end; ++it)
  {
    // Clone this system and add it to the space
    System *system = *it;
    System *new_system = Meta::Create<System>(system->GetClassName());
    new_space->AddSystem(new_system);
  }

  return new_space;
}

void Space::Copy(Space *dest_space, bool full_copy)
{
  // Iterate over existing systems
  auto it = m_system_array.begin();
  auto end = m_system_array.end();
  for (; it != end; ++it)
  {
    // Attempt to find this system in the other space and copy data back to it
    System *system = *it;
    System *dest_system = dest_space->GetSystem(system->GetClassName());
    if (dest_system)
    {
      system->Copy(dest_system, full_copy);
    }
  }
}

void Space::Deserialize(Serializer &serializer)
{
  m_name = serializer.GetName();

  serializer.Begin();

  while (serializer.IsGood())
  {
    String name = serializer.GetName();
    System *system = Meta::Create<System>(name);
    system->SetClassName(name.GetCString());
    Read(serializer, *system);
    AddSystem(system);
    serializer.Iterate();
  }
}

System* Space::GetSystem(String name)
{
  auto it = m_system_map.Find(name);

  if (it != m_system_map.End())
  {
    System *system = it->second;
    return system;
  }

  return 0;
}

}