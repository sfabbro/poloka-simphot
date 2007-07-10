#include <iostream>
#include <cmath> // asin, sqrt
#include <iomanip>

#include "multimagstar.h"

#include "image.h"
#include "fitsimage.h"
#include "wcsutils.h"
#include "gtransfo.h"
#include "globalval.h"

#include "fastifstream.h"


void
MultiMagSEStar::SetToZero()
{

  alpha = 0. ;
  delta = 0. ;
  x_orig = 0. ;
  y_orig = 0. ;
  ell_dist = 0. ;
  norm_dist = 0. ;
  dist = 0. ;
}



// CONSTRUCTORS
MultiMagSEStar::MultiMagSEStar(const SEStar &sestar) : SEStar(sestar)
{
  SetToZero();
  x_orig = x ;
  y_orig = y;
  // magbox et l'ellipse qu'elle contient sont crees et initialisees a la demande
}






std::string MultiMagSEStar::WriteHeader_(ostream & pr, 
				     const char* i) const
{
  std::string sestarFormat = SEStar::WriteHeader_(pr, i);
  if (!i) i="";
  pr << "#ra" << i  << " : " << endl;
  pr << "#dec" << i  << " : " << endl;
  pr << "#x_orig" << i  << " : " << endl;
  pr << "#y_orig" << i  << " : " << endl;
  pr << "#nm"<<i<< " : number of magboxes " << endl;



  for (unsigned k=0; k < magboxes.size(); ++k)
    {
      string kk = magboxes[k].calib.band ;
      //pr << "#band" << kk << i  << " : " << endl;
      pr << "#zp" << kk << i  << " : " << endl;
      pr << "#sigzp" << kk << i  << " : " << endl;
      pr << "#seeing" << kk << i  << " : " << endl;
      pr << "#fc" << kk << i  << " : " << endl;
      pr << "#efc" << kk << i  << " : " << endl;
      pr << "#f" << kk << i  << " : " << endl;
      pr << "#ef" << kk << i  << " : " << endl;
      pr << "#m" << kk << i  << " : " << endl;
      pr << "#em" << kk << i  << " : " << endl;
    }
  return sestarFormat+ "MultiMagSEStar 3"; 
}



void
MultiMagSEStar::dumpn(ostream& s) const
{
  SEStar::dumpn(s);
  s << "alpha : " << alpha << " " 
    << "delta : " << delta << " " 
    << "x_orig : " << x_orig << " " 
    << "y_orig : " << y_orig << " " ;

   s << "size magboxes : " << magboxes.size() << ' ';
  for (unsigned k=0; k < magboxes.size(); ++k)
    {
      const CalibBox &cal =  magboxes[k].calib;
      s << "band : " << cal.band << " ZP : " << cal.ZP 
	<< " sigZP :  " << cal.sigZP << " seeing : " << cal.seeing << " " ;
      s <<  " flux circ : " << magboxes[k].f_circ 
	<< " err. flux circ : " <<  magboxes[k].ef_circ 
       <<  " flux auto : " << magboxes[k].f_auto 
	<< " err. flux auto : " <<  magboxes[k].ef_auto 
	<< " mag auto : " <<  magboxes[k].m_auto 
	<< " err. mag auto : " <<  magboxes[k].em_auto << " " ;
    }
}

void MultiMagSEStar::writen(ostream& s) const
{
  SEStar::writen(s); 
  ios::fmtflags  old_flags =  s.flags();
  int oldprec = s.precision(); 
  s<< setiosflags(ios::fixed) ;
  s  << setiosflags(ios::scientific) ;
  s << setprecision(14);
  s << alpha << " " ;
  s << delta << " " ;
  s << x_orig << " " ;
  s << y_orig << " " ;  
    
 s << magboxes.size() << ' ';
  for (unsigned k=0; k < magboxes.size(); ++k)
    {
     
       const CalibBox &cal =  magboxes[k].calib;
       //s << cal.band << " " ; //!! attention, pas lisible par paw du coup
       s << cal.ZP << " " ;
       s << cal.sigZP << " " ;
       s << cal.seeing << " " ;
       s << magboxes[k].f_circ << " " ;
       s << magboxes[k].ef_circ << " " ;
       s << magboxes[k].f_auto << " " ;
       s << magboxes[k].ef_auto << " " ;
       s << magboxes[k].m_auto << " " ;
       s << magboxes[k].em_auto << " " ;
    }
 s.flags(old_flags);
 s << setprecision(oldprec);
    
}



void MultiMagSEStar::read_it(fastifstream& r, const char* Format)
{
  SEStar::read_it(r, Format);
  int format = DecodeFormat(Format, "MultiMagSEStar");
  r >> alpha  ;
  r >> delta  ;
  r >> x_orig  ;
  r >> y_orig  ;
  
  if (format <= 1)
    {
      double undouble ;
      for(int i = 0 ; i < 35 ; i++)
	r >> undouble ;
    }

  unsigned nmag;
  r >> nmag; 
  for (unsigned k=0; k < nmag; ++k)
    {
      magboxes.push_back(ShortMagBox());
      ShortMagBox &mb = magboxes.back();
      CalibBox &cal = mb.calib;
      if (format <= 2)
	r >> cal.band ;
      r >> cal.ZP  ;
      r >> cal.sigZP  ;
      r >> cal.seeing  ;
      r >> mb.f_circ ;
      r >> mb.ef_circ ;
      r >> mb.f_auto ;
      r >> mb.ef_auto ;
      r >> mb.m_auto ;
      r >> mb.em_auto ;
   }
}

MultiMagSEStar* MultiMagSEStar::read(fastifstream& r, const char* Format)
{
  MultiMagSEStar *pstar = new MultiMagSEStar();  
  pstar->read_it(r, Format);
  return(pstar);
}
  
void  MultiMagSEStar::ComputeMag(int kbox, string band, double ZP, double eZP)
{
  ShortMagBox &mb = magboxes[kbox];
  CalibBox &cal = mb.calib;
  cal.band = band ;
  cal.ZP = ZP;
  cal.sigZP = eZP;
  if (mb.f_auto > 0 )
    {			 			  
      mb.m_auto = -2.5*log10(mb.f_auto)+ZP ;
      mb.em_auto = fabs(mb.ef_auto/mb.f_auto);
    }
  else
    {
      mb.m_auto = 99 ;
      mb.em_auto = -1 ;
    }
}



#include "ellipticaper.h"

// equation de l'ellipse : cxx . X^2 + cyy . Y^2 + cxy XY = 1, ellipse de 1/2 grd axe A, de 1/2 petit axe B,  et d'inclinaison theta
// d^2 = cxx x^2 + cyy y^2 + cxy y^2 = distance elliptique en nombre de A() ou B(), ca vaut 1 sur le pourtour de l'ellipse.
// le flux dans SExtractor est calcule sur l'ellipse:
// cxx . X^2 + cyy . Y^2 + cxy XY = R^2
// R = rayon d'integration = kron FACTOR  = 2.5 * kron RADIUS
// qui est ce qu'on recupere de SExtractor et malhencontreusement appele >Kronradius() chez nous.
// donc pour retranscrire d en "rayon de l'ellipse de photom en pixels", il faut x par R*sqrt(AB)
// si R * sqrt(AB) < RadMin, alors on suit la procedure de SExtractor : reset les cxx,cyy,cxy a 1,1,0 (cercle) et R =  Radius, en pixel donc.
double MultiMagSEStar::SqEllipticDistance(double xx, double yy, double dilatation, double RadMin,double Radius) const
{
  Elliptic_Aperture ell_aper;
  ell_aper.SetParameters(*this,dilatation,RadMin,Radius);
  double dist2 =  ell_aper.SqEllipticDistance(xx,yy);
  return dist2;
}
// est -on ou non dans l'ellipse (ou le cercle) de photometrie ?
// d/Radius si cercle, d/kron factor ie KronRadius() si ellipse.

double MultiMagSEStar::NormalizedDistance(double xx, double yy, double dilatation, double RadMin,double Radius) const
{
  Elliptic_Aperture ell_aper;
  ell_aper.SetParameters(*this,dilatation,RadMin,Radius);
  double dist2 =  ell_aper.SqEllipticDistance(xx,yy);
  if (dist2 > 0)
    dist2 = sqrt(dist2);
  else
    return(-1);
  if (ell_aper.IsCircle() && (Radius>0)  )  
    return (dist2*1./Radius);
  else
    {
      if (Kronradius() > 0)
	return (dist2*1./Kronradius());
      else
	{
	  cerr << "##Pb: Kronradius <=0 !!!!" << endl ;
	}
    }
  return(-1);
}




bool IncSqEllipticDist(const MultiMagSEStar *S1, const MultiMagSEStar *S2)
{
  return (S1->ell_dist < S2->ell_dist );
}

bool IncNormalizedDistance(const MultiMagSEStar *S1, const MultiMagSEStar *S2)
{
  return (S1->norm_dist < S2->norm_dist );
}




#include "starlist.cc"

//instantiate all template routines from StarList
template class StarList<MultiMagSEStar>;
MultiMagSEStarList::MultiMagSEStarList(const string &FileName)
{
  read(FileName);
  int nbox = front()->magboxes.size() ;
  // 2 cas :
  // 1) old list : pas de globals, mais etiquette cal.band lue

  // 2) new list : globals contenant les etiquettes a mettre dans cal.band
  // verification des globals
   if (nbox >  0 )
    {
      bool new_version = ( (front()->magboxes[0].calib).band == "" );
      // verification homogeneite
      for (int ii = 0 ; ii < nbox ; ii++)
	{
	  CalibBox &cal =  front()->magboxes[ii].calib ;
	  if ( (( cal.band == "") && ! new_version ) || ( ( cal.band != "") && new_version ) )
	    cerr << "Error in reading filename : " << FileName << "  cal bands partially empty" << endl ;
	}
      if ( ! new_version ) // on mets les globals
	{
	  for (int ii = 0 ; ii < nbox ; ii++)
	    {
	      CalibBox &cal =  front()->magboxes[ii].calib ;
	      char  kkey[20];
	      sprintf(kkey,"MAG_%-d",ii);
	      GlobVal().AddKey(kkey,cal.band) ;
	    }
	}
      else // on mets les etiquettes
	{
	  // verification autant de magboxes que d'etiquettes
	  for(int ii = 0 ; ii < nbox ; ii++)
	    {
	      char  kkey[20];
	      sprintf(kkey,"MAG_%-d",ii);
	      if ( ! GlobVal().HasKey(kkey) )
		cerr << "Error in reading filename : " << FileName 
		     << "  no " << kkey << endl;
	    }
	  // on remets les etiquettes
	  for (MultiMagSEStarIterator mit = this->begin();mit != this->end();mit++)
	    {
	      for (int ii = 0 ; ii < (*mit)->magboxes.size(); ii++ )
		{
		  CalibBox &cal =  (*mit)->magboxes[ii].calib ;
		  char  kkey[20];
		  sprintf(kkey,"MAG_%-d",ii);
		  cal.band = GlobVal().getStringValue(kkey) ;
		}
	    }
	}
    }
}


// just copy and initialize
MultiMagSEStarList::MultiMagSEStarList(const SEStarList &L)
{
  for (SEStarCIterator i = L.begin(); i != L.end(); ++i)
    push_back(new MultiMagSEStar(**i));
}



void
MultiMagSEStarList::CopySEStarList(const SEStarList &L)
{
  for (SEStarCIterator i = L.begin(); i != L.end(); ++i)
    push_back(new MultiMagSEStar(**i));
}



void Check(MultiMagSEStarList &LM, SEStarList &L)
{
  cerr << "Dans liste SE : " << endl ;
  for (SEStarIterator i = L.begin(); i != L.end() ; ++i)
    {
      SEStar *star  = *i;
      MultiMagSEStar *magstar = LM.FindClosest(star->x, star->y);
      if (magstar != NULL)
	{
	  double d2 = (star->x-magstar->x)*(star->x-magstar->x) + (star->y-magstar->y)*(star->y-magstar->y) ;
	  if (d2 > 1)
	    {
	      cerr << "MAL RETROUVEE " << endl ;
	      cerr << "SESTAR: " ;
	      star->dump();
	      cerr << "MAGSTAR: " ;
	      magstar->dump();
	    }
	}
      else
	{ 
	  star->dump();
	 cerr << "PAS RETROUVEE " << endl ; 
	}
    }


  cerr << "Dans liste MULTIMAGSE : " << endl ;
  for (MultiMagSEStarIterator i = LM.begin(); i != LM.end(); ++i)
    {
      MultiMagSEStar *magstar  = *i;
      SEStar *star = L.FindClosest(magstar->x, magstar->y);
      if (star != NULL)
	{
	  double d2 = (star->x-magstar->x)*(star->x-magstar->x) + (star->y-magstar->y)*(star->y-magstar->y) ;
	  if (d2 > 1)
	    {
	      cerr << "MAL RETROUVEE " << endl ;
	      cerr << "MAGSTAR: " ;
	      magstar->dump();
	      cerr << "SESTAR: " ;
	      star->dump();
	    }
	}
      else
	{
	  magstar->dump();
	 cerr << "PAS RETROUVEE " << endl ; 
	}
    }

}

void
MultiMagSEStarList::ComputeMag(int kbox, string band, double ZP, double eZP)
{
  for (MultiMagSEStarIterator mit = this->begin();mit != this->end();mit++)
    {
      (*mit)->ComputeMag(kbox, band, ZP, eZP);
    }
  return ;
}


      
bool
MultiMagSEStarList::UpDate(SEStarList &L, string *bands, int nband)
{ 
  int ng = GlobVal().NKey();
  for (int ii = 0 ; ii < nband ; ii++)
    {
      char  kkey[20];
      sprintf(kkey,"MAG_%-d",ii);
      GlobVal().AddKey(kkey,bands[ii]);
    }


  if (size() != L.size())
    {
      cerr << "Taille des listes differentes " << endl ;
      Check(*this, L);
      return(false);
    }
  int nn = 0 ;
  MultiMagSEStarIterator mit = this->begin();
  for (SEStarCIterator i = L.begin(); i != L.end() && mit != this->end(); ++i, ++mit)
    {
      MultiMagSEStar *magstar = *mit ;
      const SEStar *star = *i ;
      double d2 = (magstar->x -star->x)*(magstar->x -star->x)+(magstar->y -star->y)*(magstar->y -star->y);
      if (d2 > 1.e-5)
	{	 
	  cerr << "etoiles differentes dans les listes " ;
	  for (int ii = 0 ; ii < nband ; ii++)
	    {cerr << bands[ii] << " "  <<  endl ;}
	  return(false);
	} 

      if ( nn < 5 )
	{
	  cerr << "Nbre de MagBoxes: " << magstar->magboxes.size() << endl ;
	}

      for (int ii = 0 ; ii < nband ; ii++)
	{
	  magstar->magboxes.push_back(ShortMagBox());
	  ShortMagBox &mb = magstar->magboxes.back();
	  CalibBox &cal = mb.calib;
	  cal.band = bands[ii] ;// pour info de provenance, peut changer apres avec la calib effectivement utilisee
	  // NB dans le cas ou le seeing est calcule par ailleurs et 
	  // stocke dans Fwhm()
	  cal.seeing =  star->Fwhm(); 
	  mb.f_auto = star->Flux_aper(); // toujours cette vieille "convention" avec sextractorbox
	  mb.ef_auto = star->Eflux_aper(); 
	  mb.f_circ = star->Flux_fixaper();// doit etre calcule par ailleurs
	  mb.ef_circ = star->Eflux_fixaper();
	}

      if ( nn < 5 )
	{
	  cerr << "New Nbre de MagBoxes: " << magstar->magboxes.size() << endl ;
	  nn++;
	}
    }
  return(true);
}


MultiMagSEStar*  MultiMagSEStarList::FindEllipticClosest(double xx, double yy, double dilatation, double RadMin, double Radius) const
{

  double min_dist2 = 1e30;
  const MultiMagSEStar *minstar = NULL;
  double dist2;
  for (MultiMagSEStarCIterator si = begin(); si!= end(); ++si) 
    { 
      const MultiMagSEStar *s = *si;
      dist2 =  s->SqEllipticDistance(xx, yy, dilatation, RadMin,Radius);
      if (dist2 < min_dist2) { min_dist2 = dist2; minstar = s;}
    }
  return (MultiMagSEStar *) minstar; // violates constness


}




void MultiMagSEStarList::check() const
{
  if (!empty())
    {
      unsigned nmag = front()->magboxes.size();
      for (MultiMagSEStarCIterator i = begin(); i !=end(); ++i)
	{
	  if ((*i)->magboxes.size() != nmag)
	    {
	      cerr << " writing a MultiMagSEStarList  with different number of apertures" << endl;
	      cerr << " hope you know what you are doing " << endl;
	      (*i)->dump(cerr);
	    }
	}
    }
}


int MultiMagSEStarList::write(const std::string &FileName) const
{
  check();
  return StarList<MultiMagSEStar>::write(FileName);
}



void 
MultiMagSEStarList::ComputeAlphaDelta(const FitsHeader & head)
{
  Gtransfo *wcs;
  WCSFromHeader(head, wcs);
  for (MultiMagSEStarIterator i = this->begin(); i != this->end(); ++i)
    {
      MultiMagSEStar *star = *i ;
      wcs->apply(star->x,star->y,star->alpha ,star->delta); 
    }
}


/************ converters *************/
BaseStarList* MultiMagSE2Base(MultiMagSEStarList * This)
{ return (BaseStarList *) This;}

const BaseStarList* MultiMagSE2Base(const MultiMagSEStarList * This)
{ return (const BaseStarList *) This;}

BaseStarList& MultiMagSE2Base(MultiMagSEStarList &This)
{ return (BaseStarList &) This;}

const BaseStarList& MultiMagSE2Base(const MultiMagSEStarList &This)
{ return (const BaseStarList &) This;}

/* ================================== */

SEStarList* MultiMagSE2SE(MultiMagSEStarList * This)
{ return (SEStarList *) This;}

const SEStarList* MultiMagSE2SE(const MultiMagSEStarList * This)
{ return (const SEStarList *) This;}

SEStarList& MultiMagSE2SE(MultiMagSEStarList &This)
{ return (SEStarList &) This;}

const SEStarList& MultiMagSE2SE(const MultiMagSEStarList &This)
{ return (const SEStarList &) This;}

/* ================================== */
AperSEStarList* MultiMagSE2AperSE(MultiMagSEStarList * This)
{ return (AperSEStarList *) This;}

const AperSEStarList* MultiMagSE2AperSE(const MultiMagSEStarList * This)
{ return (const AperSEStarList *) This;}

AperSEStarList& MultiMagSE2AperSE(MultiMagSEStarList &This)
{ return (AperSEStarList &) This;}

const AperSEStarList& MultiMagSE2AperSE(const MultiMagSEStarList &This)
{ return (const AperSEStarList &) This;}


// on prend les neighbors plus proche que dist (pixels), mais on les ordonne
// par leur distance elliptique.
// ell_dist : distance elliptique en nombre de a 
// dist : distance en arc sec
void FindEllipticNeighb(double xsn, double ysn, double dist,
			MultiMagSEStarList & stlin, 
			MultiMagSEStarList & stl_neighb)
{
  //double n2lim = nrad_lim*nrad_lim;
  double d2lim = dist*dist;
  for(MultiMagSEStarIterator itse = stlin.begin(); itse!= stlin.end();itse++)
    {
      MultiMagSEStar *sestar = *itse;
      double n2 = sestar->SqEllipticDistance(xsn,ysn,1,8.,8.);
      double d2 = (xsn - sestar->x)*(xsn - sestar->x) +
		(ysn  - sestar->y)*(ysn  - sestar->y);
      if (d2 < d2lim)
	{
	  sestar->ell_dist = sqrt(n2) ; 
	  sestar->dist = sqrt(d2) ;
	  stl_neighb.push_back(sestar);
	}
    }
  stl_neighb.sort(&IncSqEllipticDist);
  return;
}

void CheckNeighbFinders(string name, double xsn, double ysn, double dist,
			MultiMagSEStarList & stlin)
{
  //double n2lim = nrad_lim*nrad_lim;
  double d2lim = dist*dist;
  MultiMagSEStarList  stl_neighb;
  for(MultiMagSEStarIterator itse = stlin.begin(); itse!= stlin.end();itse++)
    {
      MultiMagSEStar *sestar = *itse;
      double n2 = sestar->SqEllipticDistance(xsn,ysn,1,8.,8.);
      double d2 = (xsn - sestar->x)*(xsn - sestar->x) +
		(ysn  - sestar->y)*(ysn  - sestar->y);
      if (d2 < d2lim)
	{
	  sestar->ell_dist = sqrt(n2) ; 
	  sestar->dist = sqrt(d2) ;
	  stl_neighb.push_back(sestar);
	}
    }
  stl_neighb.sort(&IncSqEllipticDist);
  int ii =0;
  int ifirst=-1;
  double d_min_norm=1000.;
  for(MultiMagSEStarIterator itse = stl_neighb.begin(); itse!= stl_neighb.end();itse++)
    {
      MultiMagSEStar *neighb = *itse;
      double d_norm = neighb->NormalizedDistance(xsn,ysn,1,8.,8.);
      if (d_norm  < d_min_norm)
	{
	  ifirst=ii;
	  d_min_norm = d_norm  ;
	}
      cerr << name << "_" << ii << "  " << neighb->dist << " " << neighb->ell_dist << " " << d_norm << endl ;
      ii++;
    }
  if (ifirst != 0)
    cerr << "Normalized Distance : l'hote de " << name << " a change : " << ifirst << endl ;
  return;
}

void FindNormalizedDistNeighb(double xsn, double ysn, double dist,
			      MultiMagSEStarList & stlin, 
			      MultiMagSEStarList & stl_neighb)
{
  //double n2lim = nrad_lim*nrad_lim;
  double d2lim = dist*dist;
  for(MultiMagSEStarIterator itse = stlin.begin(); itse!= stlin.end();itse++)
    {
      MultiMagSEStar *sestar = *itse;
      double d2_ell = sestar->SqEllipticDistance(xsn,ysn,1,8.,8.);
      double d_norm = sestar->NormalizedDistance(xsn,ysn,1,8.,8.);
      if (d_norm < 0)
	cerr << "##Pb dans calcul NormalizedDistance" << endl ;
      double d2 = (xsn - sestar->x)*(xsn - sestar->x) +
		(ysn  - sestar->y)*(ysn  - sestar->y);
      if (d2 < d2lim)
	{
	  sestar->ell_dist = sqrt(d2_ell) ; 
	  sestar->norm_dist = d_norm ;
	  sestar->dist = sqrt(d2) ;
	  stl_neighb.push_back(sestar);
	}
    }
  stl_neighb.sort(&IncNormalizedDistance);
  return;
}





