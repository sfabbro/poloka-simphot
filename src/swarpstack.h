// This may look like C code, but it is really -*- C++ -*-
#ifndef SWARPSTACK__H
#define SWARPSTACK__H



#include <vector>
#include <string>
#include "reducedimage.h"
#include "stringlist.h"


//! enables to provide a datacards file name (a la swarp) that superseeds both swarp and toads internal defaults. syntax is the one from swarp (see doc or run swarp -d)
bool SetSwarpCardsName(const std::string &Name);

//! enables to overwrite by program defaults in the swarpstack module
void AddSwarpDefaultKey(const std::string &Key, const std::string &Value);

class Frame;

/*! prepares images and uses swarp to coadd and shift

*/

class SwarpStack : public ReducedImage {
   void init();
private:

  // vector<Component> components;
  // StackingMethod stackingMethod;
  // WeightingMethod weightingMethod;
  ReducedImageList images;
  ReducedImageRef photomAstromReference;
  Frame photomAstromReferenceFrame;

  

  const std::string SwarpPermDir() const { return Dir()+"swarp_work/";};

  const std::string SwarpTmpDir();

  std::string tmpDir;

  
public :
  SwarpStack(const string &Name, const ReducedImageList &Images,
	     const ReducedImage *PhotomAstromReference,
	     const Frame &SubFrame);

  //	  const ReducedImage *PhotomReference = NULL,
  //     const WeightingMethod AWMethod = WUnSet, 
  //     const StackingMethod ASMethod = SUnSet);

  SwarpStack(const string &Name);
  SwarpStack() {};


  virtual const string  TypeName() const { return "SwarpStack";}
  //  ReducedImageList Components() const;

  bool MakeFits() ;
  bool MakeDead();
  bool MakeSatur();
  bool MakeWeight();
  bool MakeCatalog();
  // ReducedImage *Clone() const;

  // void dump(ostream & s = cout) const;

  //  typedef vector<Component>::iterator ComponentIterator;
  // typedef vector<Component>::const_iterator ComponentCIterator;

  // ~SwarpStack();

private:
  void FitsHeaderFill();

};



#endif /* SWARPSTACK__H */
