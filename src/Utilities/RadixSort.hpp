// Nicholas Baldwin
// 2011/11/01
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

//THIS IS A HELPER TO NOT CALL, it is the function that does that radix
template <typename T>
void Radix( unsigned short ByteIndex, T * Source, T * Dest, unsigned SourceSize, unsigned intOffset )
{
  unsigned ByteCounter[256];
  //set the counter array to zero
  
  std::memset( ByteCounter, 0, 1024 );

  //start at the propper byte index
  unsigned char * curByte = reinterpret_cast<unsigned char*>(Source) + ByteIndex + intOffset;
  unsigned const incSize = sizeof( T );
  //loop over all of the bytes in the sorce, incrementing the index
  for ( unsigned i = 0; i < SourceSize; ++i, curByte += incSize )
  {
    ++ByteCounter[*curByte];
  }

  //use the byte counts that I have to make indexes
  unsigned indexStart = 0;//this where the specific byte starts in the dest array
  for ( unsigned i = 0; i < 256; ++i )
  {
    unsigned tempCount = ByteCounter[i];
    ByteCounter[i] = indexStart;
    indexStart += tempCount; //this will effectively set up the gaps needed in the 
                            //final array to fill with bytes
  }

  //not that you have indexes, copy the values over
  curByte = reinterpret_cast<unsigned char*>(Source) + ByteIndex + intOffset;
  for ( unsigned i = 0; i < SourceSize; ++i, ++Source, curByte += incSize )
  {
    unsigned * countPtr = ByteCounter + *curByte;
    Dest[*countPtr] = *Source;
    ++(*countPtr);
  }
}

//THIS IS YOUR RADIX SORT, READ THE NOTICE AT THE TOP OF THE FILE
template <typename T>
void RadixSort( T * data, unsigned intOffset, unsigned size )
{
  //allocate a temp buffer
  T * tempData = new T[size];
  
  Radix( 0, data, tempData, size, intOffset );
  Radix( 1, tempData, data, size, intOffset );
  Radix( 2, data, tempData, size, intOffset );
  Radix( 3, tempData, data, size, intOffset );
  
  delete [] tempData;
}

}