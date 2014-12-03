// main.c
// main file for cachesim project
// ECEN4593 F2010
//
// Danny Gale, Chris Messick
//
// Revision History:
// 11/2/2010   Danny Gale  created
// 11/2/2010   Danny Gale  converted to C++

using namespace std;

// COMPILER INCLUDES
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include <stdlib.h>

// PROJECT INCLUDES
#include "cxxcache.h"
#include "defines.h"

// GLOBAL VARIABLES
cache I(CACHETYPE_L1);
cache D(CACHETYPE_L1);
cache L2(CACHETYPE_L2);
main_memory mem;

unsigned long long cc = 0; // cycle count
unsigned long long ic = 0; // instruction count
unsigned long long branch, compute, load, store = 0;   // keep track of types of instructions
unsigned long long cBranch, cCompute, cLoad, cStore = 0;   // keep track of time for each type of instructions

time_t start, end;

// EXTERNAL FUNCTIONS
extern void get_config (const char * configFileName);
extern void printParameters(ofstream & outFile);
// for parsing command line arguments
extern char *optarg;
extern int optind, opterr, optopt;

// LOCAL FUNCTIONS
unsigned fetch(cache * thisCache, unsigned addr, op_type op);
void output(ofstream & filename);

int main(int argc, char * argv[])
{
   char op, c;
   unsigned addr;
   unsigned exec;
   unsigned timeToFetch = 0;
   ifstream configFile;
   ofstream outFile;
   const char * configFileName, * outFileName = 0;
   string str;


   // get command line arguments
   while ((c = getopt(argc, argv, "c:o:")) != -1)
   {
      switch (c)
      {
	 case 'c':   // specifying a config file
	    configFileName = optarg;
	    break;
	 case 'o':   // specifying an output file
	    outFileName = optarg;
	    break;
	 default:
	    abort();
      }
   }

   outFile.open(outFileName);
   if (configFileName != 0)
   {
      outFile << "Using configuration file: " << configFileName << endl;
      get_config(configFileName);
   }
   else
      cout << "No configuration file provided. Using default parameters." << endl;

   printParameters(outFile);
   
// INITIALIZATION-DEBUGGING CONDITIONAL COMPILATION CODE
#ifdef _DEBUG_L1_STRUCTURE_
#ifdef _DEBUG_L1_STRUCTURE_FAKEDATA_
   unsigned i, j, val;
   // put some fake data in the instruction cache
   cout << endl << "putting some fake data into L1" << endl;
   for (i = 0; i < I.get_numSets(); i++)
   {
      for (j = 0; j < I.get_set(i)->get_associativity(); j++)
      {
	 val = 0xFFFFFFFF - (i + j);
#ifdef _DEBUG_L1_STRUCTURE_VERBOSE_
	 cout << "setting tag for set " << i << " block " << j << " to " << val << endl;
#endif
	 I.get_set(i)->get_block(j)->set_tag(val);
      }
   }
#endif

   cout << endl << "CACHE L1 STRUCTURE" << endl;
   for (i = 0; i < I.get_numSets(); i++)
   {
      cout << endl;
      cout << dec << "Set#: " << setw(4) << i << " | ";
      I.get_set(i)->output_blocks();
   }
   cout << endl;
#endif
#ifdef _DEBUG_L2_STRUCTURE_
   unsigned k, l;
   cout << endl << "CACHE L2 STRUCTURE" << endl;
   for (k = 0; k < L2.get_numSets(); k++)
   {
      cout << endl;
      cout << "Set#: " << setw(4) << k << " | ";
      for (l = 0; l < L2.get_set(k)->get_associativity(); l++)
      {
	 cout << " " << setw(4) << l;
      }
   }
   cout << endl;
#endif

   // BEGIN ACCEPTING TRACE CODE HERE
   time(&start);

   cout << endl << "=============";
   cout << "Begin Trace";
   cout << "==============" << endl;
   ic = 0;
   cc = 0;
   while (!cin.eof())
   {

      // GET AN INSTRUCTION
#ifdef _DEBUG_FETCH_
      cout << "========== FETCHING INSTRUCTION ==========" << endl;
#endif
      cin >> op >> hex >> addr >> hex >> exec;
      if (ic % 1000000 == 0)
      //cout << "\rinst #:\t" << setw(8) << dec << ic/1000000 << "M" << "\tcycles:\t" << setw(8) << dec << cc << "\top: " << op << "\taddr: " << setw(8) << hex << addr << "\texec: " << setw(8) << hex << exec;// << endl;
      cout << "\rinst #:\t" << setw(8) << dec << ic << "\tcycles:\t" << setw(8) << dec << cc << "\top: " << op << "\taddr: " << setw(8) << hex << addr << "\texec: " << setw(8) << hex << exec;// << endl;

      timeToFetch = fetch(&I, addr, READ);
      cc += timeToFetch;

      switch (op)
      {
	 case 'B':   // branch. one cycle to execute
	    cc++;
	    //cout << "cc++ for branch" << endl;
	    branch++;
	    cBranch += timeToFetch;
#ifdef _DEBUG_FETCH_
	    cout << "Branch instruction. No data fetch." << endl;
#endif
	    break;
	 case 'C':   // computation. exec contains latency information
	    cc += exec;
	    //cout << "cc += " << exec << " for compute" << endl;
	    cCompute += exec + timeToFetch;
	    compute++;
#ifdef _DEBUG_FETCH_
	    cout << "Computation instruction. No data fetch." << endl;
#endif
	    break;
	 case 'L':   // load. exec contains address of requested data
#ifdef _DEBUG_FETCH_
      cout << "========== LOAD: FETCHING DATA ==========" << endl;
#endif
	    //cLoad += timeToFetch;
	    timeToFetch = fetch(&D, exec, READ);
	    cc += timeToFetch;
	    //cout << "cc += " << timeToFetch << " for load" << endl;
	    cLoad += timeToFetch;
	    load++;
	    break;
	 case 'S':   // store. exec contains address of data to be written
#ifdef _DEBUG_FETCH_
      cout << "========== STORE: FETCHING DATA =========" << endl;
#endif
	    timeToFetch = fetch(&D, exec, WRITE);
	    cc += timeToFetch;
	    //cout << "cc += " << timeToFetch << " for store" << endl;
	    cStore += timeToFetch;
	    store++;
	    break;
	 default:
	    cout << "ERROR: INVALID INSTRUCTION TYPE: " << op << endl;
	    exit(ES_INVALID_INSTRUCTION);
	    break;
      }
      //cout << "ic = " << dec << ic << "\tcc = " << cc << "\tL2.hits = " << L2.get_hits() << endl;
      ic++; // increment instruction count
   }

   time(&end);
   output(outFile);
   outFile.close();
   cout << endl;

   return 0;
}

// returns time taken to fetch 
unsigned fetch(cache * thisCache, unsigned addr, op_type operation)
{
   unsigned fetchTime = 0;
   cacheLine * ptr = 0;
   cacheLine * requestedBlock = 0;

#ifdef _DEBUG_FETCH_
   //cout << "_DEBUG_FETCH_\tChecking L1 for address " << addr << " using index " << index << " and tag " << tag << endl;
#endif
#ifdef _DEBUG_FETCH_QUIET_
   cout << "_DEBUG_FETCH_ ";
#endif

   ////////////////////////////////////////////////////////
   // V4
   ////////////////////////////////////////////////////////
   // changed to state machine
   // addition of state variable
   enum STATE_T { CHECK_L1, CHECK_L2, READ_MM, WRITE_UP_TO_L2, WRITE_TO_MM1,
      READ_L2, WRITE_UP_TO_L1, WRITE_DOWN_TO_L2, WRITE_TO_MM2, OP, READ_L1,
      WRITE_L1, DONE };

   STATE_T state = CHECK_L1;

   while (state != DONE)
   {
      switch(state)
      {
	 case CHECK_L1:
	    #ifdef _DEBUG_FETCH_QUIET_
	    cout << "CHECK_L1";
	    #endif
	    ptr = thisCache->hit(addr, operation);
	    if (ptr)
	    {
	       fetchTime += thisCache->get_tHit();
	       //thisCache->add_hit(operation);
	       #ifdef _DEBUG_FETCHTIME_
	       cout << "L1 hit (+" << thisCache->get_tHit() << ")" << endl;
	       #endif
	       thisCache->operate_on(operation, addr);
	       state = DONE;
	       break;
	    }
	    else
	    {
	       #ifdef _DEBUG_FETCHTIME_
	       cout << "L1 miss (+" << thisCache->get_tMiss() << ")" << endl;
	       #endif
	       fetchTime += thisCache->get_tMiss();
	       //thisCache->add_miss(operation);
	       state = CHECK_L2;
	       break;
	    }
	    break;
	    
	 case CHECK_L2:
	    #ifdef _DEBUG_FETCH_QUIET_
	    cout << " -> CHECK_L2";
	    #endif
	    ptr = L2.hit(addr, operation);
	    if (ptr)
	    {
	       fetchTime += L2.get_tHit();
	       #ifdef _DEBUG_FETCHTIME_
	       cout << "L2 hit (+" << L2.get_tHit() << ")" << endl;
	       #endif
	       //L2.add_hit(operation);
	       state = WRITE_UP_TO_L1;
	       break;
	    }
	    else
	    {
	       fetchTime += L2.get_tMiss();
	       //L2.add_miss(operation);
	       #ifdef _DEBUG_FETCHTIME_
	       cout << "L2 miss (+" << L2.get_tMiss() << ")" << endl;
	       #endif
	       state = READ_MM;
	       break;
	    }
	    break;
	    
	 case READ_MM:
	    #ifdef _DEBUG_FETCH_QUIET_
	    cout << " -> READ_MM";
	    #endif

	    #ifdef _DEBUG_FETCHTIME_
	    cout << "Reading main mem (+" << mem.get_tSendAddr() + mem.get_tReady() + mem.get_tChunk() * L2.get_blockSize() / mem.get_chunkSize() << ")" << endl;
	    #endif
	    fetchTime += mem.get_tSendAddr() + mem.get_tReady();
	    fetchTime += mem.get_tChunk() * L2.get_blockSize() / mem.get_chunkSize();
	    state = WRITE_UP_TO_L2;
	    break;

	 case WRITE_UP_TO_L2:
	    #ifdef _DEBUG_FETCH_QUIET_
	    cout << " -> WRITE_UP_TO_L2";
	    #endif
	    ptr = L2.write_from_below(addr);
	    if (ptr)
	    {
	       delete ptr;
	       state = WRITE_TO_MM1;
	       break;
	    }
	    else
	    {
	       state = READ_L2;
	       break;
	    }
	    break;

	 case WRITE_TO_MM1:
	    #ifdef _DEBUG_FETCH_QUIET_
	    cout << " -> WRITE_TO_MM1";
	    #endif
	    fetchTime += mem.get_tSendAddr() + mem.get_tReady();
	    fetchTime += mem.get_tChunk() * L2.get_blockSize() / mem.get_chunkSize();
	    #ifdef _DEBUG_FETCHTIME_
	    cout << "Writing main mem (+" << mem.get_tSendAddr() + mem.get_tReady() + mem.get_tChunk() * L2.get_blockSize() / mem.get_chunkSize() << ")" << endl;
	    #endif
	    state = READ_L2;
	    break;

	 case READ_L2:
	    #ifdef _DEBUG_FETCH_QUIET_
	    cout << " -> READ_L2";
	    #endif
	    ptr = L2.hit(addr, READ);
	    if (!ptr)
	    {
	       cerr << "R/W error on L2. exiting." << endl;
	       exit(0);
	       break;
	    }
	    #ifdef _DEBUG_FETCHTIME_
	    cout << "L2 replay (+" << L2.get_tHit() << ")" << endl;
	    #endif
	    fetchTime += L2.get_tHit();
	    state = WRITE_UP_TO_L1;
	    break;

	 case WRITE_UP_TO_L1:
	    #ifdef _DEBUG_FETCH_QUIET_
	    cout << " -> WRITE_UP_TO_L1";
	    #endif
	    fetchTime += L2.get_tTransfer() * (thisCache->get_blockSize() / L2.get_busWidth());
	    #ifdef _DEBUG_FETCHTIME_
	    cout << "write up to L1 (+" << L2.get_tTransfer() * (thisCache->get_blockSize() / L2.get_busWidth()) << ")" << endl;
	    #endif
	    
	    ptr = thisCache->write_from_below(addr);
	    if(ptr)
	    {
	       state = WRITE_DOWN_TO_L2;
	       break;
	    }
	    else
	    {
	       state = OP;
	       break;
	    }
	    break;

	 case WRITE_DOWN_TO_L2:
	    #ifdef _DEBUG_FETCH_QUIET_
	    cout << " -> WRITE_DOWN_TO_L2";
	    #endif
	    fetchTime += L2.get_tTransfer() * (thisCache->get_blockSize() / L2.get_busWidth());
	    #ifdef _DEBUG_FETCHTIME_
	    cout << "write down to L2 (+" << L2.get_tTransfer() * (thisCache->get_blockSize() / L2.get_busWidth()) << ")" << endl;
	    #endif
	    fetchTime += L2.get_tHit();
	    L2.hit(addr, WRITE);
	    #ifdef _DEBUG_FETCHTIME_
	    cout << "L2 hit (+" << L2.get_tHit() << ")" << endl;
	    #endif
	    ptr = L2.write_from_above(addr);
	    //L2.add_hit(operation);
	    if(ptr)
	    {
	       delete ptr;
	       state = WRITE_TO_MM2;
	       break;
	    }
	    else
	    {
	       state = OP;
	       break;
	    }
	    break;

	 case WRITE_TO_MM2:
	    #ifdef _DEBUG_FETCH_QUIET_
	    cout << " -> WRITE_TO_MM2";
	    #endif
	    fetchTime += mem.get_tSendAddr() + mem.get_tReady();
	    fetchTime += mem.get_tChunk() * L2.get_blockSize() / mem.get_chunkSize();
	    
	    #ifdef _DEBUG_FETCHTIME_
	    cout << "Writing main mem (+" << mem.get_tSendAddr() + mem.get_tReady() + mem.get_tChunk() * L2.get_blockSize() / mem.get_chunkSize() << ")" << endl;
	    #endif
	    state = OP;
	    break;

	 case OP:
	    #ifdef _DEBUG_FETCH_QUIET_
	    cout << " -> OP";
	    #endif
	    fetchTime += thisCache->get_tHit();
	    #ifdef _DEBUG_FETCHTIME_
	    //cout << "L1 replay (+" << thisCache->get_tHit() << ")" << endl;
	    #endif
	    thisCache->operate_on(operation, addr);
	    state = DONE;
	    break;
	    
	 default:
	    cerr << "what happened?" << endl;
	    exit(0);
	    break;
      }
   }

   #ifdef _DEBUG_FETCH_QUIET_
   cout << endl << "--------------------------------------------" << endl;
   #endif
   return fetchTime;

}

void output(ofstream & outfile)
{
#ifdef _OUTPUT_
   cout << endl << "=============== END TRACE =============" << endl << endl;
   //ofstream outfile (filename);
#ifdef _OUTPUT_TRACE_STATS_
   outfile << endl << "TRACE STATISTICS" << endl;

   double timeDif = difftime(end, start);
   unsigned hours, minutes, seconds = 0;
   hours = (int)timeDif / 3600;
   minutes = ((int)timeDif - 3600*hours) / 60;
   seconds = ((int)timeDif - 3600*hours - 60*minutes);
   outfile << "actual completion time:\t" << dec << hours <<" : " << dec << minutes << " : " << dec << seconds << endl;

   outfile << "total instructions:\t" << dec << ic << endl;
   outfile << "total cycles:\t\t" << dec << cc << endl;
   outfile << "average CPI:\t\t" << dec << (double)cc/ic << endl;
   outfile << endl;
   outfile << "TYPE\tCOUNT\tPERCENT\t\tCYCLES\tCPI" << endl;
   outfile << "load\t" << dec << load << "\t" << (double)load/ic * 100 << " %" << "\t" << cLoad << "\t" << (double)cLoad/load << endl;
   outfile << "store\t" << dec << store << "\t" << (double)store/ic * 100 << " %" << "\t" << cStore << "\t" << (double)cStore/store << endl;
   outfile << "branch\t" << dec << branch << "\t" << (double)branch/ic * 100 << " %" << "\t" << cBranch << "\t" << (double)cBranch/branch << endl;
   outfile << "compute\t" << dec << compute << "\t" << (double)compute/ic * 100 << " %" << "\t" << cCompute << "\t" << (double)cCompute/compute << endl;
#endif
#ifdef _OUTPUT_CACHE_STATS_
   outfile << endl << "CACHE STATISTICS\t\tL1-I\t\tL1-D\t\t  L2" << endl;
   outfile << "hit rate\t=\t" << setw(12) << (double)I.get_hits() / ( I.get_misses() + I.get_hits() ) * 100 << "\t" << setw(12) << (double)D.get_hits() / ( D.get_misses() + D.get_hits() ) * 100 << "\t" << setw(12) << (double)L2.get_hits() / ( L2.get_misses() + L2.get_hits() ) * 100 << endl; 
   outfile << "miss rate\t=\t" << setw(12) << (double)I.get_misses() / ( I.get_misses() + I.get_hits() ) * 100  << "\t" << setw(12) << (double)D.get_misses() / ( D.get_misses() + D.get_hits() ) * 100 << "\t" << setw(12) << (double)L2.get_misses() / ( L2.get_misses() + L2.get_hits() ) * 100 << endl; 
   outfile << endl;
   outfile << "total requests\t=\t" << dec << setw(12) << I.get_reads() + I.get_writes() << "\t" << setw(12) << D.get_reads() + D.get_writes() << "\t" << setw(12) << L2.get_reads() + L2.get_writes() << endl;
   outfile << "total hits\t=\t" << dec << setw(12) << I.get_hits() << "\t" << setw(12) << D.get_hits() << "\t" << setw(12) << L2.get_hits() << endl;
   //outfile << "L2 hitsR\t=\t\t\t" << L2.get_hitsR() << endl;
   //outfile << "L2 hitsWD\t=\t\t\t" << L2.get_hitsWD() << endl;
   outfile << "total misses\t=\t" << dec << setw(12) << I.get_misses() << "\t" << setw(12) << D.get_misses() << "\t" << setw(12) << L2.get_misses() << endl;
   outfile << endl;
   outfile << "total reads\t=\t" << dec << setw(12) << I.get_reads() << "\t" << setw(12) << D.get_reads() << "\t" << setw(12) << L2.get_reads()<< endl;
   outfile << "total writes\t=\t" << dec << setw(12) << I.get_writes() << "\t" << setw(12) << D.get_writes() << "\t" << setw(12) << L2.get_writes() << endl;
   outfile << endl;
   outfile << "read hits\t=\t" << setw(12) << dec << I.get_readHits() << "\t" << setw(12) << D.get_readHits() << "\t" << setw(12) << L2.get_readHits() << endl;
   outfile << "write hits\t=\t" << setw(12) << dec << I.get_writeHits() << "\t" << setw(12) << D.get_writeHits() << "\t" << setw(12) << L2.get_writeHits() << endl;
   outfile << endl;
   outfile << "read misses\t=\t" << setw(12) << dec << I.get_readMisses() << "\t" << setw(12) << D.get_readMisses() << "\t" << setw(12) << L2.get_readMisses() << endl;
   outfile << "write misses\t=\t" << setw(12) << dec << I.get_writeMisses() << "\t" << setw(12) << D.get_writeMisses() << "\t" << setw(12) << L2.get_writeMisses() << endl;
   outfile << endl;
   outfile << "evictions\t=\t" << setw(12) << I.get_evictions() << "\t" << setw(12) << D.get_evictions() << "\t" << setw(12) << L2.get_evictions() << endl;
   outfile << "dirty evictions\t=\t" << setw(12) << I.get_dirtyEvictions() << "\t" << setw(12) << D.get_dirtyEvictions() << "\t" << setw(12) << L2.get_dirtyEvictions() << endl;
   outfile << endl;
#endif
   unsigned i;
#ifdef _OUTPUT_CACHE_I_
   outfile << endl << "CACHE \"I\" FINAL STATE" << endl;
   outfile << setw(17) << "V D " << setw(6) << "TAG";
   for (i = 0; i < I.get_numSets(); i++)
   {
      /*
      if (L2.get_set(i)->get_size() == 0)
	 continue;
	 */
      outfile << endl;
      outfile << dec << "Set#: " << setw(4) << hex << i << " | ";
      I.get_set(i)->output_blocks(outfile);
   }
   outfile << endl;
#endif
#ifdef _OUTPUT_CACHE_D_
   outfile << endl << "CACHE \"D\" FINAL STATE" << endl;
   outfile << setw(17) << "V D " << setw(6) << "TAG";
   for (i = 0; i < D.get_numSets(); i++)
   {
      /*
      if (L2.get_set(i)->get_size() == 0)
	 continue;
	 */
      outfile << endl;
      outfile << dec << "Set#: " << setw(4) << hex << i << " | ";
      D.get_set(i)->output_blocks(outfile);
   }
   outfile << endl;
#endif
#ifdef _OUTPUT_CACHE_L2_
   outfile << endl << "CACHE \"L2\" FINAL STATE" << endl;
   outfile << setw(17) << "V D " << setw(6) << "TAG";
   for (i = 0; i < L2.get_numSets(); i++)
   {
      /*
      if (L2.get_set(i)->get_size() == 0)
	 continue;
	 */
      outfile << endl;
      outfile << dec << "Set#: " << setw(4) << hex << i << " | ";
      L2.get_set(i)->output_blocks(outfile);
   }
   outfile << endl;
#endif
#endif
}
