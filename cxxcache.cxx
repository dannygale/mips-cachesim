// cxxcache.cxx
// implementation of the L1 and L2 caches, and main memory
// ECEN4593 cache simulator project
// F2010
// Danny Gale and Chris Messick
//
// REVISION HISTORY:
// 11/2/2010   Danny Gale  created
// 11/5/2010   Danny Gale  consolidated L1 and L2 cache classes into single class
//

using namespace std;

#include <iostream>
#include <cmath>
#include <stdlib.h>

#include "cxxcache.h"
#include "cacheline.h"
#include "set.h"
#include "defines.h"

cache::cache()
{
   cout << endl << "ERROR: UNINITIALIZED CACHE TYPE" << endl;
   exit(ES_UNINITIALIZED_CACHE);
}

cache::cache(cache_t cacheType)
{
   unsigned i;

   if (cacheType == CACHETYPE_L1)
   {
      blockSize = L1_DEFAULT_BLOCK_SIZE;
      cacheSize = L1_DEFAULT_CACHE_SIZE;
      assoc = L1_DEFAULT_ASSOC;
      tHit = L1_DEFAULT_tHIT;
      tMiss = L1_DEFAULT_tMISS;
      tTransfer = 0;
      busWidth = 0;
      
      numSets = cacheSize / (assoc * blockSize);
      //bytesPerBlock = blockSize;

      sets = new set[numSets];

      for (i = 0; i < numSets; i++)
	 sets[i].set_associativity(assoc);

      indexBits = log2(numSets);
      byteBits = log2(blockSize);
      tagBits = 32 - byteBits - indexBits;
   }
   else if (cacheType == CACHETYPE_L2)
   {
      blockSize = L2_DEFAULT_BLOCK_SIZE;
      cacheSize = L2_DEFAULT_CACHE_SIZE;
      assoc = L2_DEFAULT_ASSOC;
      tHit = L2_DEFAULT_tHIT;
      tMiss = L2_DEFAULT_tMISS;;
      tTransfer = L2_DEFAULT_tTRANSFER;
      busWidth = L2_DEFAULT_BUSWIDTH;

      numSets = cacheSize / (assoc * blockSize);
      //bytesPerBlock = blockSize / 8;

      sets = new set[numSets];

      for (i = 0; i < numSets; i++)
	 sets[i].set_associativity(assoc);

      indexBits = log2(numSets);
      byteBits = log2(blockSize);
      tagBits = 32 - byteBits - indexBits;
   }
   else
   {
      cout << "ERROR: invalid cache type: " << cacheType << endl;
      exit(ES_INVALID_CACHE_TYPE);
   }

   writes = 0;
   reads = 0;
   writeRequests = 0;
   readRequests = 0;
   requests = 0;
   writeMisses = 0;
   readMisses = 0;
   misses = 0;
   writeHits = 0;
   readHits = 0;
   hits = 0;
}

cache::~cache()
{
   // free dynamic array
   delete [] sets;
}

// remove and resize dynamic array
void cache::set_assoc(unsigned value)
{
#ifdef _DEBUG_CACHE_SETASSOC_
   cout << "Inside set_assoc(). setting associativity to " << value << endl;
#endif
   assoc = value;
   if (assoc == 0)
   {  // FULLY ASSOCIATIVE
      // only a single set
      set_numSets(1); 
   }
   else
   {
      set_numSets(cacheSize / (assoc * blockSize));
   }

#ifdef _DEBUG_CACHE_SETASSOC_
   cout << "numSets = " << numSets << endl;
#endif
   for (unsigned i = 0; i < numSets; i++)
   {
#ifdef _DEBUG_CACHE_SETASSOC_
   cout << "i = " << i << endl;
#endif
      sets[i].set_associativity(cacheSize / blockSize);
   }
}

void cache::set_blockSize(unsigned newSize)
{
   blockSize = newSize;
   set_numSets(cacheSize / (assoc * blockSize));
   //set_bytesPerBlock(blockSize);
}

void cache::set_cacheSize(unsigned newSize)
{
   cacheSize = newSize;
   set_numSets(cacheSize / (assoc * blockSize) );
}

void cache::set_numSets(unsigned n)
{
   numSets = n;
   indexBits = log2(numSets);
   tagBits = 32 - indexBits - byteBits;
}

/*
void cache::set_bytesPerBlock(unsigned n)
{
   bytesPerBlock = n;
   byteBits = log2(bytesPerBlock);
   tagBits = 32 - indexBits - byteBits;
}
*/

// searches the cache for a given address
// returns a pointer to the block if it's found
// also maintains statistics
cacheLine * cache::hit(unsigned addr, op_type operation)
{
#ifdef _DEBUG_CACHE_HIT_
   cout << "\t_DEBUG_CACHE_HIT_\tInside cache::hit using index " << index << " and tag " << tag << endl;
#endif
   unsigned tag = make_tag(addr);
   unsigned index = make_index(addr);

   // index to the set, go through the set and see if the data is there
   // find_line() returns 0 if the line is not found or is not valid, 
   // or a pointer to it if it is found & valid
   cacheLine * ptr = sets[index].find_line(tag);
#ifdef _DEBUG_CACHE_HIT_
   cout << "\t_DEBUG_CACHE_HIT_\tptr has been set to " << ptr << endl << "_DEBUG_CACHE_HIT_\tReturning ptr" << endl;
#endif
   if (operation == READ)
      reads++;
   else if (operation == WRITE)
      writes++;
   else
   {
      cout << "_CACHE_HIT_\tInvalid operation: " << operation << endl;
      exit(0);
   }

   if (ptr)
   {
      hits++;
      if (operation == READ)
	 readHits++;
      else if (operation == WRITE)
	 writeHits++;
      else
      {
	 // problem
	 cout << "_CACHE_HIT_\tInvalid operation: " << operation << endl;
	 exit(0);
      }
   }
   else
   {
      misses++;
      if (operation == READ)
	 readMisses++;
      else if (operation == WRITE)
	 writeMisses++;
      else
      {
	 // problem
	 cout << "_CACHE_HIT_\tInvalid operation: " << operation << endl;
	 exit(0);
      }
   }

   return ptr;
}

void cache::operate_on(op_type operation, unsigned addr)
{
   cacheLine * ptr, * evicted = 0;
   unsigned index = make_index(addr);
   unsigned tag = make_tag(addr);

   ptr = sets[index].find_line(tag);

   if (ptr)
   {
      evicted = sets[index].update_LRU(ptr);

      if (operation == WRITE)
      {
	 ptr->set_dirty(true);
      }
   }
   else
   {
      cerr << "_CACHE_OPERATE_ON_\toperating on a block that's not there." << endl;
      cerr << "_CACHE_OPERATE_ON_\toperation = " << operation << endl;
      cout << "Exiting." << endl;
      exit(0);
   }

   if (evicted)
   {
      cerr << "_CACHE_OPERATE_ON_\tevicting a block on final operation. " << endl;
      cerr << "_CACHE_OPERATE_ON_\toperation = " << operation << endl;
      cerr << "_CACHE_OPERATE_ON_\taddr = " << addr << "\tindex = " << index << "\ttag = " << tag << endl;
      exit(0);
   }

}

// updates the cacheLine so that it is in true LRU order
// ASSUMES THAT ptr HAS ALREADY BEEN SET TO THE BLOCK BY hit()
//void cache::read(unsigned addr)
   /*
{
   cacheLine * ptr = 0;
   cacheLine * evicted = 0;
   unsigned index = make_index(addr);
   unsigned tag = make_tag(addr);

   //cout << endl << "_READ_\tlooking for index: " << index << " and tag: " << tag << endl;

   // go find the line in the set
   ptr = sets[index].find_line(tag);

   if (ptr) // ptr will be non-zero if the tag was found
   {  
      // update the LRU
      evicted = sets[index].update_LRU(ptr);
   }
   else
   {  // didn't find it
      cerr << "_CACHE_READ_\treading null on a read. exiting." << endl;
      exit(0);
   }

   if (evicted)
   {
      // should not evict anything on a read
      cerr << "_CACHE_READ_\tevicting on a read. exiting." << endl;
      exit(0);
   }
}
*/

// NOT A WRITE REQUEST, JUST A WRITE
// do not search for valid block with the correct tag,
// just write into the cache, replacing the least recently
// used line
//void cache::write(unsigned addr)
   /*
{
   cacheLine * ptr = 0;
   cacheLine * evicted = 0;
   unsigned index = make_index(addr);
   unsigned tag = make_tag(addr);

   // go find the line in the set
   ptr = sets[index].find_line(tag);

   if (ptr) // ptr will be non-zero if the tag was found
   {  
      // update the LRU
      ptr->set_dirty(true);
      evicted = sets[index].update_LRU(ptr);
   }
   else
   {  // didn't find it
      cerr << "_CACHE_WRITE_\twriting null on a write. exiting." << endl;
      cout << "_CACHE_WRITE_\ttag: "<<tag<<"\tindex: "<<index<<"\tptr: "<<ptr<<endl;
      exit(0);
   }

   if (evicted)
   {
      // should not evict anything on a read
      cerr << "_CACHE_WRITE_\tevicting on a write update. exiting." << endl;
      exit(0);
   }
}
*/

cacheLine * cache::write_from_below(unsigned addr)
{
   cacheLine * ptr = 0;
   cacheLine * evicted = 0;

   unsigned tag = make_tag(addr);
   unsigned index = make_index(addr);

   //cout << endl << "tag: " << tag << "\tindex: " << index << endl;
   ptr = sets[index].find_line(tag);
   if (!ptr)
   {
      ptr = new cacheLine;
      ptr->set_tag(tag);
      ptr->set_valid(true);
   }

   evicted = sets[index].update_LRU(ptr);
   if (evicted && evicted->get_valid())
      evictions++;
   if (evicted && evicted->get_valid() && evicted->get_dirty())
      dirtyEvictions++;

   if (evicted && evicted->get_dirty())
      return evicted;
   else
      return 0;
}

// SHOULD be dirty
cacheLine * cache::write_from_above(unsigned addr)
{
   cacheLine * ptr = 0;
   cacheLine * evicted = 0;

   unsigned tag = make_tag(addr);
   unsigned index = make_index(addr);

   ptr = sets[index].find_line(tag);
   if (ptr == 0)
   {
      // SHOULD be there, but isn't
      cerr << "non-inclusive cache. exiting" << endl;
      exit(0);
   }

   evicted = sets[index].update_LRU(ptr);
   if (evicted)
      evictions++;
   if (evicted && evicted->get_dirty())
      dirtyEvictions++;

   ptr->set_dirty(true);

   return evicted;
}

void cache::add_hit(op_type operation)
{
   if (operation == READ)
      add_readHit();
   else if (operation == WRITE)
      add_writeHit();
   else
   {
      // problem
      cout << "invalid operation in cache::add_hit()" << endl;
      exit(0);
   }
}

void cache::add_miss(op_type operation)
{
   if (operation == READ)
      add_readMiss();
   else if (operation == WRITE)
      add_writeMiss();
   else
   {
      // problem
      cout << "invalid operation in cache::add_hit()" << endl;
      exit(0);
   }
}

unsigned cache::make_tag(unsigned addr)
{
   unsigned t;

   t = addr >> (indexBits + byteBits);
   return t;
}

unsigned cache::make_index(unsigned addr)
{
   unsigned i;

   i = (addr << tagBits) >> (tagBits + byteBits);
   return i;
}

main_memory::main_memory()
{
   tSendAddr = MEM_DEFAULT_tSENDADDR;
   tReady = MEM_DEFAULT_tREADY;
   tChunk = MEM_DEFAULT_tCHUNK;
   chunkSize = MEM_DEFAULT_CHUNKSIZE;
}

main_memory::~main_memory()
{

}
