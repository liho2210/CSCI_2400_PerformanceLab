#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "Filter.h"

using namespace std;

#include "rdtsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

class Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    for (int i=0; i < size; i++) {
      for (int j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  } else {
    cerr << "Bad input in readFilter:" << filename << endl;
    exit(-1);
  }
}
//*****************

double
applyFilter(class Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

  long long cycStart, cycStop;

  cycStart = rdtscll();

  // declare variables outside of loops *****
  // call getSize and getDivisor functions outside of loops *****
  output -> width = input -> width;
  output -> height = input -> height;
  int col, row, i, j, rcoll, gcoll, bcoll;
  int width = input->width - 1;
  int height = input->height -1;
  int size = filter->getSize();
  char divisor = filter->getDivisor();

  // flip row and col to consider spatial locality *****
  for(row = 1; row < height; row++) 
  {
    for(col = 1; col < width; col++) 
    {
      // use separate containers to maintain values instead of calling output-> each time *****
      rcoll = 0;
      gcoll = 0;
      bcoll = 0;

      for (i = 0; i < size; i++) 
      {
        for (j = 0; j < size; j++) 
        {
          rcoll = rcoll
          + (input -> color[row + i - 1][col + j - 1].r
          * filter -> get(i, j) );

          gcoll = gcoll
          + (input -> color[row + i - 1][col + j - 1].g
          * filter -> get(i, j) );

          bcoll = bcoll
          + (input -> color[row + i - 1][col + j - 1].b
          * filter -> get(i, j) );
        }
      }

      rcoll = rcoll / divisor;
      gcoll = gcoll / divisor;
      bcoll = bcoll / divisor;

      if ( rcoll  < 0 ) 
      {
        rcoll = 0;
      }
      if ( gcoll  < 0 ) 
      {
        gcoll = 0;
      }
      if ( bcoll  < 0 ) 
      {
        bcoll = 0;
      }

      if ( rcoll > 255 ) 
      { 
        rcoll = 255;
      }
      if ( gcoll > 255 ) 
      { 
        gcoll = 255;
      }
      if ( bcoll > 255 ) 
      { 
        bcoll = 255;
      }

      output -> color[row][col].r = rcoll;
      output -> color[row][col].g = gcoll;
      output -> color[row][col].b = bcoll;
    }
  }

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}
