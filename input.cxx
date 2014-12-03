// input.c
// a file for reading inputs from the command line
//
// Danny Gale, Chris Messick
//
// REVISION HISTORY
// 11/1/2010   Danny Gale  created
// 11/2/2010   Danny Gale  converted to C++


// COMPILER INCLUDES
#include <string>
#include <iostream>
#include <fstream>

// PROJECT INCLUDES
#include "cxxcache.h"

using namespace std;

// EXTERNAL VARIABLES
extern cache I;
extern cache D;
extern cache L2;
extern main_memory mem;

// LOCAL FUNCTIONS
void printParameters(ofstream & outFile);
void get_config(const char * configFileName);

void get_config(const char * configFileName)
{
   string param;
   unsigned int value;
   ifstream configFile;

#ifdef _DEBUG_GET_CONFIG_
   cout << "Opening config file: " << configFileName;
#endif
   configFile.open(configFileName);
#ifdef _DEBUG_GET_CONFIG_
   cout << ". done" << endl;
#endif

   // check for valid configuration file
   if (!configFile.is_open())
   {
      cout << "Invalid configuration file" << endl << "Using default values" << endl;
   }
   else
   {
#ifdef _DEBUG_GET_CONFIG_
      cout << "Config file is open." << endl;
#endif
      //cout << "Using configuration file " << filename << endl;
      // if valid, set parameters that are present
      while ( !configFile.eof() )
      {
	 configFile >> param >> value;
	 if (param.compare("L1.blockSize") == 0)
	 {
	    I.set_blockSize(value);
	    D.set_blockSize(value);
	 }
	 else if (param.compare("L1.cacheSize") == 0)
	 {
	    I.set_cacheSize(value);
	    D.set_cacheSize(value);
	 }
	 else if (param.compare("L1.assoc") == 0)
	 {
	    I.set_assoc(value);
	    D.set_assoc(value);
	 }
	 else if (param.compare("L1.tHit") == 0)
	 {
	    I.set_tHit(value);
	    D.set_tHit(value);
	 }
	 else if (param.compare("L1.tMiss") == 0)
	 {
	    I.set_tMiss(value);
	    D.set_tMiss(value);
	 }
	 else if (param.compare("L2.blockSize") == 0)
	 {
	    L2.set_blockSize(value);
	 }
	 else if (param.compare("L2.cacheSize") == 0)
	    L2.set_cacheSize(value);
	 else if (param.compare("L2.assoc") == 0)
	    L2.set_assoc(value);
	 else if (param.compare("L2.tHit") == 0)
	    L2.set_tHit(value);
	 else if (param.compare("L2.tMiss") == 0)
	    L2.set_tMiss(value);
	 else if (param.compare("L2.tTransfer") == 0)
	    L2.set_tTransfer(value);
	 else if (param.compare("L2.busWidth") == 0)
	    L2.set_busWidth(value);
	 else if (param.compare("mem.tSendAddr") == 0)
	    mem.set_tSendAddr(value);
	 else if (param.compare("mem.tReady") == 0)
	    mem.set_tReady(value);
	 else if (param.compare("mem.tChunk") == 0)
	    mem.set_tChunk(value);
	 else if (param.compare("mem.chunkSize") == 0)
	    mem.set_chunkSize(value);
	 else
	 {
	    cout << "Invalid parameter: " << param << endl;
	 }

#ifdef _DEBUG_GET_CONFIG_
	 cout << "parameter: " << param << "set to: " << value << endl;
#endif
      }

      cout << endl << "Config file acquired" << endl;
   }
   return;
}

void printParameters(ofstream & outfile)
{
   outfile << endl << "CACHE PARAMETERS:\tL1:\tL2:" << endl;
   outfile << "blockSize\t=\t" << I.get_blockSize() <<"\t" << L2.get_blockSize() << "\tBYTES" << endl;
   outfile << "cacheSize\t=\t" << I.get_cacheSize() << "\t" << L2.get_cacheSize() << "\tBYTES" << endl;
   outfile << "assoc\t\t=\t" << I.get_assoc() << "\t" << L2.get_assoc() << endl;
   outfile << "tHit\t\t=\t" << I.get_tHit() << "\t" << L2.get_tHit() << endl;
   outfile << "tMiss\t\t=\t" << I.get_tMiss() << "\t" << L2.get_tMiss() << endl;
   outfile << "tTransfer\t=\t\t" << L2.get_tTransfer() << endl;
   outfile << "busWidth\t=\t\t" << L2.get_busWidth() << endl;
   outfile << "numSets\t\t=\t" << I.get_numSets() << "\t" << L2.get_numSets() << endl;
   //outfile << "bytesPerBlock\t=\t" << I.get_bytesPerBlock() << "\t" << L2.get_bytesPerBlock() << endl;
   outfile << "indexBits\t=\t" << I.get_indexBits() << "\t" << L2.get_indexBits() << endl;
   outfile << "byteBits\t=\t" << I.get_byteBits() << "\t" << L2.get_byteBits() << endl;
   outfile << "tagBits\t\t=\t" << I.get_tagBits() << "\t" << L2.get_tagBits() << endl;

   outfile << endl << "MEMORY PARAMETERS:" << endl;
   outfile << "tSendAddr\t=\t" << mem.get_tSendAddr() << endl;
   outfile << "tReady\t\t=\t" << mem.get_tReady() << endl;
   outfile << "tChunk\t\t=\t" << mem.get_tChunk() << endl;
   outfile << "chunkSize\t=\t" << mem.get_chunkSize() << endl;
}
