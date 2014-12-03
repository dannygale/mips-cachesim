// cacheentry.cxx
// a class for each entry in the cache and main memory
//
// ECEN4593, F2010
// Danny Gale, Chris Messick
//
// REVISION HISTORY:
// 11/4/2010   Danny Gale  created
// 11/4/2010   Danny Gale  removed support for data. not needed

using namespace std;

#include "cacheline.h"
#include "defines.h"

cacheLine::cacheLine()
{
   valid = false;
   dirty = false;
   tag = 0;
   next = 0;
}

/*cacheLine::cacheLine(bool v, bool d, unsigned tagval)
{
   valid = v;
   dirty = d;
   tag = tagval;
   next = 0;
}

cacheLine::cacheLine(bool v, bool d, unsigned tagval, cacheLine * n)
{
   valid = v;
   dirty = d;
   tag = tagval;
   next = n;
}
*/

cacheLine::cacheLine(unsigned tagval, cacheLine * n)
{
   valid = true;
   dirty = false;
   tag = tagval;
   next = n;
}

cacheLine::~cacheLine()
{

}
