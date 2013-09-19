#include <iostream>
#include <string>

#include <poloka/polokaexception.h>
#include <poloka/lightcurvefile.h>

static void usage(const char *progname) {
  cerr << "Usage: " << progname << " [OPTION]... FILE\n"
       << "Make a light curve of a transient from pixels\n\n"
       << "    -v     :  write vignettes for each epoch\n"
       << "    -m     :  write matrices of the system (debug)\n"
       << "    -N INT :  max number of stars to be fitted\n"
       << "    -f INT :  parameters to fit. 0=gal+flux 1=flux (default is 0)\n"
       << "    -d     :  one output directory per object\n"
       << "    -o FILE:  output catalogue\n"
       << "    -c FILE:  calibration catalogue to use for stars\n\n";
    exit(EXIT_FAILURE);
}


int main(int nargs, char **args) {

  if (argc < 2) usage(args[0]);

  string filename;
  string outputCatalog;
  string calibrationCatalog;
  bool writeVignettes = false;
  bool writeMatrices = false;
  bool oneDirPerObj = false;
  int Nmax = -1 ;
  int fit_type = 0;

  for (int i=1; i< nargs; ++i)  {
    char *arg = args[i];
    if (arg[0] == '-')
      switch(arg[1]) {
      case 'v' : writeVignettes = true; break;
      case 'N' : Nmax = atoi(args[++i]); break;  
      case 'm' : writeMatrices = true; break;  
      case 'd' : oneDirPerObj = true; break;
      case 'o' : outputCatalog=args[++i]; continue; break;
      case 'c' : calibrationCatalog=args[++i]; continue; break;
      case 'f' : fit_type=atoi(args[++i]); continue; break;
      default : 
	cerr << args[0] << ": unknown option " << arg << endl;
	usage(args[0]);
	break;
      }
    else
      filename = arg;
  }

  bool success = true;

  try {
    cout << args[0] << ": fit Type : " << fit_type << endl;
    LightCurveFile lcf(filename);
    if (writeVignettes) lcf.PleaseWriteVignettes();
    if (writeMatrices) lcf.PleaseWriteMatrices();
    if (oneDirPerObj) lcf.PleaseOneDirPerObject();
    if (!calibrationCatalog.empty()) {
      if (fit_type==0) {
	cout << args[0] << ": assuming you mean fit_type = 1\n";
	fit_type = 1;
      }
      success = lcf.SimPhotFitAllCalib(calibrationCatalog, outputCatalog, fit_type, Nmax);
    } else success = lcf.SimPhotFitAll();
  } catch (PolokaException e) {
    p.PrintMessage(cerr);
    success = false;
  }

  return ((success)? EXIT_SUCCESS :  EXIT_FAILURE) ;
}

