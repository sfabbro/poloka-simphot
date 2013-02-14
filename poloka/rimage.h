#ifndef RIMAGE__H
#define RIMAGE__H

#include <poloka/reducedimage.h>
#include <poloka/frame.h>

//! ReducedImage reference with memory of fits keywords
class RImageRef
{
  
  ReducedImageRef ri;
  double   backLevelNoSub;
  string band;
  double exposure;
  double mjd;
  double seeing;
  double sigmaBack;
  Frame usablePart;

 public :
 RImageRef(const ReducedImage *Ri) : ri(Ri) 
  {
    if (ri) {
    backLevelNoSub = ri->BackLevelNoSub();
    band = ri->Band();
    exposure = ri->Exposure();
    mjd = ri->ModifiedJulianDate();
    seeing = ri->Seeing();
    sigmaBack = ri->SigmaBack();
    usablePart = ri->UsablePart();
    }
  };

 RImageRef(): ri((ReducedImage *)NULL)  {};
  double BackLevelNoSub() const { return backLevelNoSub;}
  string Band() const { return band;}
  double Exposure() const { return exposure;}
  double  ModifiedJulianDate()  const { return mjd;}
  double  Seeing()  const { return seeing;}
  double  SigmaBack()  const { return sigmaBack;}
  Frame  UsablePart() const  { return usablePart;}

  //  ReducedImage* operator->() { return ri;}
  const ReducedImage* operator->() const { return ri;}

  operator const ReducedImage*() const { return ri;}

  operator const ReducedImageRef() const { return ri;}

  //  const ReducedImage& operator*() const { return *ri;}


};

class RImageList : public list<RImageRef>
{
 public :
  RImageList::const_iterator Find(const std::string& Name) const
    {
      RImageList::const_iterator i=begin();
      for ( ; i != end(); ++i)
	if ((*i)->Name() == Name) break;
      return i;  
    }
};

typedef RImageList::iterator RImageIterator;
typedef RImageList::const_iterator RImageCIterator;



#endif /* RIMAGE__H */
