// cxxcache.h
// holds classes for the L1 and L2 caches, and main memory
// ECEN4593 cache simulator project
// F2010
// Danny Gale, Chris Messick
//
// Revision History:
// 11/2/2010   Danny Gale  created    
// 11/2/2010   Danny Gale  converted from C structs to C++ classes
// 11/4/2010   Danny Gale  added dynamic cacheLine array to L1 and L2 caches
//			   added some numLines and wordsPerLine, calculated from given params
//
#ifndef _CXXCACHE_H_
#define _CXXCACHE_H_

using namespace std;

#include "set.h"
#include "defines.h"

enum cache_t { CACHETYPE_L1, CACHETYPE_L2 };

class cache
{
   public:
      // CONSTRUCTOR AND DESTRUCTOR
      cache();
      cache(cache_t cacheType);
      ~cache();

      // CONSTANT MEMBER FUNCTIONS
      unsigned get_blockSize() const { return blockSize; }
      unsigned get_cacheSize() const { return cacheSize; }
      unsigned get_assoc() const { return assoc; }
      unsigned get_tHit() const { return tHit; }
      unsigned get_tMiss() const { return tMiss; }
      unsigned get_tTransfer() const { return tTransfer; }
      unsigned get_busWidth() const { return busWidth; }
      unsigned get_numSets() const { return numSets; }
      unsigned get_blocksPerSet() const { return blocksPerSet; }
      //unsigned get_bytesPerBlock() const { return bytesPerBlock; }
      unsigned get_indexBits() const { return indexBits; }
      unsigned get_byteBits() const { return byteBits; }
      unsigned get_tagBits() const { return tagBits; }
      set * get_set(unsigned i) const { return &sets[i]; }


      // MEMBER FUNCTIONS
      //void write(unsigned addr);
      //void read(unsigned addr);
      cacheLine * hit(unsigned addr, op_type operation);
      cacheLine * write_from_below(unsigned addr);
      cacheLine * write_from_above(unsigned addr);
      void operate_on(op_type operation, unsigned addr);


      unsigned make_tag(unsigned addr);
      unsigned make_index(unsigned addr); 
 
      void set_blockSize(unsigned newSize);
      void set_cacheSize(unsigned newSize);
      void set_assoc(unsigned newAssoc); 
      void set_tHit(unsigned newtHit) { tHit = newtHit; }
      void set_tMiss(unsigned newtMiss) {tMiss = newtMiss; }
      void set_tTransfer(unsigned newtTransfer) {tTransfer = newtTransfer; }
      void set_busWidth(unsigned newbusWidth) { busWidth = newbusWidth; }
      void set_numSets(unsigned n);
      //void set_bytesPerBlock(unsigned n);


      // STATISTICS
      unsigned get_writes() const {return writes;}
      unsigned get_writeRequests() const {return writeRequests;}
      unsigned get_writeMisses() const {return writeMisses;}
      unsigned get_writeHits() const {return writeHits;}

      unsigned get_reads() const {return reads;}
      unsigned get_readRequests() const {return readRequests;}
      unsigned get_readMisses() const {return readMisses;}
      unsigned get_readHits() const {return readHits;}

      unsigned get_requests() const {return requests;}
      unsigned get_misses() const {return misses;}
      unsigned get_hits() const {return hits;}
      //unsigned get_hitsR() const {return hitsR;}
      //unsigned get_hitsWD() const {return hitsWD;}
      
      unsigned get_evictions() const {return evictions;}
      unsigned get_dirtyEvictions() const { return dirtyEvictions;}

      void add_readHit() {readHits++; hits++; reads++;}
      //void add_readHitR() {readHits++; hitsR++; reads++;}
      void add_writeHit() {writeHits++; hits++; writes++;}
      //void add_writeHitWD() {writeHits++; hitsWD++; writes++;}
      void add_readMiss() {readMisses++; misses++; reads++;}
      void add_writeMiss() {writeMisses++; misses++; writes++;}

      void add_hit(op_type operation);
      void add_miss(op_type operation);
   private:
      set * sets; 

      // given cache parameters
      unsigned blockSize;
      unsigned cacheSize;
      unsigned assoc;
      unsigned tHit;
      unsigned tMiss;
      unsigned tTransfer;
      unsigned busWidth;
      // calculated cache parameters
      unsigned numSets;
      unsigned blocksPerSet;
      //unsigned bytesPerBlock;
      unsigned indexBits;
      unsigned byteBits;
      unsigned tagBits;

      // statistics
      unsigned writes;
      unsigned writeRequests;
      unsigned writeMisses;
      unsigned writeHits;

      unsigned reads;
      unsigned readRequests;
      unsigned readMisses;
      unsigned readHits;

      unsigned requests;
      unsigned misses;
      unsigned hits;
      //unsigned hitsR;
      //unsigned hitsWD;

      unsigned evictions;
      unsigned dirtyEvictions;

};

class main_memory
{
   public:
      // CONSTRUCTOR AND DESTRUCTOR
      main_memory();
      ~main_memory();

      // MEMBER FUNCTIONS
//      bool write();
//      bool read();

      unsigned get_tSendAddr() const { return tSendAddr; }
      unsigned get_tReady() const { return tReady; }
      unsigned get_tChunk() const { return tChunk; }
      unsigned get_chunkSize() const { return chunkSize; }

      void set_tSendAddr(unsigned newval) { tSendAddr = newval; }
      void set_tReady(unsigned newval) { tReady = newval; }
      void set_tChunk(unsigned newval) { tChunk = newval; }
      void set_chunkSize(unsigned newval) { chunkSize = newval; }

   private:
      unsigned tSendAddr;
      unsigned tReady;
      unsigned tChunk;
      unsigned chunkSize;
};

#endif
