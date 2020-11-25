// Nicholas Baldwin
// 2011/10/04
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Test.h"
#include "../core/timer.h"
#include "../containers/Array.h"
#include "../Containers/HashMap.h"
#include "../Math/FuzzyRule.h"

namespace Improbability
{

const int kLargeInteger = 1000000;
const int kMaxString = 40;

void TestFuzzyExpressionTree()
{
  FuzzyRule rule("if blah is very three and fetch is quite near or tank is not bad then khan is very two");
}

std::vector<String> LoadDictionary(const char *filename)
{
  FILE *dict_file;
  fopen_s(&dict_file, filename, "r");
  std::vector<String> ret;

  if (dict_file)
  {
    char word[kMaxString] = {'\0'};

    while (!feof(dict_file))
    {
      fgets(word, kMaxString, dict_file);

      ret.push_back(String(word));
    }
  }

  return ret;
}

void TestArray(const char *dictionary_name)
{
  
  Timer time;

  if (dictionary_name)
  {
    Array<String> dictionary_array;
    std::vector<String> dictionary_vector;

    FILE *dict_file;
    fopen_s(&dict_file, dictionary_name, "r");

    if (dict_file)
    {
      char word[kMaxString] = {'\0'};

      while (!feof(dict_file))
      {
        fgets(word, kMaxString, dict_file);

        String blah(word);

        dictionary_array.PushBack(blah);
        dictionary_vector.push_back(blah);
      }
    }

    printf("testing push back ---------------------\n");

    {
      Array<String> test_array;

      time.GetDt();
      for (unsigned i = 0; i < dictionary_array.GetSize(); ++i)
      {
        test_array.PushBack(dictionary_array[i]);
      }
      printf("Filling up array from array: %f\n", time.GetDt());
    }
    {
      Array<String> test_array;

      time.GetDt();
      for (unsigned i = 0; i < dictionary_vector.size(); ++i)
      {
        test_array.PushBack(dictionary_vector[i]);
      }
      printf("Filling up array from vector: %f\n", time.GetDt());
    }

    {
      std::vector<String> test_vector;

      time.GetDt();
      for (unsigned i = 0; i < dictionary_vector.size(); ++i)
      {
        test_vector.push_back(dictionary_array[i]);
      }
      printf("Filling up vector from array: %f\n", time.GetDt());
    }
    {
      std::vector<String> test_vector;

      time.GetDt();
      for (unsigned i = 0; i < dictionary_vector.size(); ++i)
      {
        test_vector.push_back(dictionary_vector[i]);
      }
      printf("Filling up vector with vector: %f\n", time.GetDt());
    }
    
    printf("testing copy constructor --------------\n");

    {
      time.GetDt();
      Array<String> test_array(dictionary_array);
      printf("Copy constructor array: %f\n", time.GetDt());
    }
    {
      time.GetDt();
      std::vector<String> test_vector(dictionary_vector);
      printf("Copy constructor vector: %f\n", time.GetDt());
    }

    printf("testing assignment --------------------\n");

    {
      Array<String> test_array;
      time.GetDt();
      test_array = dictionary_array;
      printf("Assignment array: %f\n", time.GetDt());
    }
    {
      std::vector<String> test_vector;
      time.GetDt();
      test_vector = dictionary_vector;
      printf("Assignment vector: %f\n", time.GetDt());
    }

    printf("testing iteration ---------------------\n");

    {
      Array<String> test_array;
      time.GetDt();
      for(auto it = dictionary_array.Begin(); it != dictionary_array.End(); ++it)
      {
        test_array.PushBack(*it);
      }
      printf("Array iteration with push back: %f\n", time.GetDt());
    }
    {
      std::vector<String> test_vector;
      time.GetDt();
      for(auto it = dictionary_vector.begin(); it != dictionary_vector.end(); ++it)
      {
        test_vector.push_back(*it);
      }
      printf("Vector iteration with push back: %f\n", time.GetDt());
    }
    {
      time.GetDt();
      for(auto it = dictionary_array.Begin(); it != dictionary_array.End(); ++it)
      {
      }
      printf("Array iteration without push back: %f\n", time.GetDt());
    }
    {
      time.GetDt();
      for(auto it = dictionary_vector.begin(); it != dictionary_vector.end(); ++it)
      {
      }
      printf("Vector iteration without push back: %f\n", time.GetDt());
    }

    Array<unsigned> test_array;
    for (int i = 0; i < 1001; ++i)
    {
      test_array.PushBack(i);
    }

    printf("testing correctness -------------------\n");

    printf("constructors...");
    Array<unsigned> a1;
    Array<unsigned> a2(42);
    Array<unsigned> a3(a2);
    Array<unsigned> a4(test_array.Begin(), test_array.End());

    printf("GOOD!\nassignment...");
    a1 = a4;
    a2.Assign(a4.Begin(), a4.End());

    printf("GOOD!\nsize and capacity...");
    printf("does %d == %d?\n", a1.GetSize(), a4.GetSize());
    printf("is %d less than or equal to %d?\n", a2.GetSize(), a2.GetCapacity());
    printf("should be 0 ... %d\n", a3.IsEmpty());
    a4.Resize(20); printf("should be 20 ... %d\n", a4.GetSize());
    a1.Resize(0);  printf("should be 0 ... %d\n", a1.GetSize());
    a3.Reserve(100); printf("should be 100 ... %d\n", a3.GetCapacity());

    printf("GOOD!\naccessors...");
    a2[4] = a4[2]; ErrorIf(a2[4] != a4[2]);
    a2.GetElement(2) = a4.GetElement(4); ErrorIf(a2[2] != a4[4]);
    a2.GetFront();
    a4.GetBack();

    printf("GOOD!\nelement modifiers...");
    a3.PushBack(4);
    a4.PopBack();
    a2.Insert(2, 7);
    a4.Erase(3);
    
    printf("GOOD!\narray modifiers...");
    a2.Swap(a4);
    a4.Clear();
    printf("GOOD!\n");

    time.GetDt();
  }
  printf("destructor time: %f", time.GetDt());
}

void IntHashMapTest()
{
  std::map<int, int> standard;
  HashMap<int, int> hash;


  for (int i = 0; i < kLargeInteger; ++i)
  {
    hash[i]  = i;
  }
}

void TestHashMap(const char *dictionary_name)
{
  if (dictionary_name)
  {
    std::map<String, int> standard;
    HashMap<String, int> hash;
    std::vector<String> dictionary = LoadDictionary(dictionary_name);

    Timer time;

    printf("*************testing insert*************\n");

    auto begin = dictionary.begin();
    auto it = begin;
    auto end = dictionary.end();
    time.GetDt();
    for (int i = 0; it != end; ++it, ++i)
    {
      hash.Insert(*it, i);
    }
    printf("HashMap: %f\n", time.GetDt());

    it = begin;
    time.GetDt();
    for (int i = 0; it != end; ++it, ++i)
    {
      standard.insert(std::make_pair(*it, i));
    }
    printf("Standard Map: %f\n\n", time.GetDt());

    printf("*************testing correctness*************\n");

    it = begin;
    bool is_correct = true;
    for (int i = 0; it != end; ++it, ++i)
    {
      if (hash[*it] != standard[*it])
      {
        is_correct = false;
        break;
      }
    }
    if (is_correct)
    {
      printf("The HashMap is correct!\n\n");
    }
    else
    {
      printf("The HashMap is incorrect!!!!!!!! Fix it!\n\n");
    }
    
  }
  else
  {
    IntHashMapTest();
  }
}

}