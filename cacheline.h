// cacheentry.h
// a class for each entry in the cache and main memory
//
// ECEN4593, F2010
// Danny Gale, Chris Messick
//
// REVISION HISTORY:
// 11/4/2010   Danny Gale  created
// 11/4/2010   Danny Gale  removed support for data. not needed
//

#ifndef _CACHELINE_H_
#define _CACHELINE_H_

using namespace std;

class cacheLine
{
   public:
      // CONSTRUCTORS AND DESTRUCTOR
      cacheLine();
      /*
      cacheLine(bool v, bool d, unsigned tagval);
      cacheLine(bool v, bool d, unsigned tagval, cacheLine * n);
      */
      cacheLine(unsigned tagval, cacheLine * n);
      ~cacheLine();

      // CONSTANT MEMBER FUNCTIONS
      bool get_valid() const { return valid; }
      bool get_dirty() const { return dirty; }
      unsigned get_tag() const { return tag; }
      cacheLine * get_next() const { return next; }

      // MEMBER FUNCTIONS
      void set_valid(bool value) { valid = value; }
      void set_dirty(bool value) { dirty = value; }
      void set_tag(unsigned value) { tag = value; }
      void set_next(cacheLine * nextLine) { next = nextLine; }
      void set_all(bool v, bool d, unsigned t, cacheLine * n) { valid = v; dirty = d; tag = t; next = n; }

   private:
      bool valid;
      bool dirty;
      unsigned tag;

      cacheLine * next;

};

#endif
