/* set.h
 * header file for a set class
 * contains a number of cachelines depending on the associativity of the cache
 *
 * ECEN4593, Fall 2010
 * Danny Gale, Chris Messick
 *
 * REVISION HISTORY:
 * 11/8/2010   Danny Gale  created
 *
 */
#ifndef _SET_H_
#define _SET_H_

using namespace std;

// COMPILER INCLUDES

// PROJECT INCLUDES
#include "cacheline.h"

// THE LINKED LIST POINTED TO BY head MUST BE MAINTAINED
// MANUALLY SUCH THAT THE LEAST RECENTLY USED CACHELINE
// IS ALWAYS AT THE TAIL, WITH THE MOST RECENTLY USED BEING
// POINTED TO BY THE HEAD

class set 
{
   public:
      // CONSTRUCTORS AND DESTRUCTOR
      set();
      ~set();
      
      // CONSTANT MEMBER FUNCTIONS
      unsigned get_associativity() const { return associativity; }
      cacheLine * get_block(unsigned i);
      cacheLine * get_head() { return head; }
      unsigned get_size() const { return size; }
      
      // MEMBER FUNCTIONS
      void set_associativity(unsigned assoc);
      cacheLine * update_LRU (cacheLine * mostRecent);

      cacheLine * find_line(unsigned t);
      //cacheLine * find_spot();

      // places the new block into the cache. returns a pointer to 
      // a cacheLine if one had to be evicted.
      // If a line was returned (and thus evicted) it MUST BE WRITTEN
      // BACK TO L2 and MAIN MEMORY
      cacheLine * put_line(cacheLine * newLine); 

      void output_blocks(ofstream & outfile);

   private:
      cacheLine * head;
      unsigned size;
      unsigned associativity;
};

#endif
