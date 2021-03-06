//-*-c++-*-
#ifndef _cs1300bmp_h_
#define _cs1300bmp_h_

//
// Maximum image size
//
#define MAX_DIM 8192

//
// The specific colors
//
#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2
#define MAX_COLORS 3

//separate struct to reduce size of array *****
struct rgb {
  int r;
  int g;
  int b;
};

struct cs1300bmp {
  //
  // Actual width used by this image
  //
  int width;
  //
  // Actual height used by this image
  //
  int height;
  //
  // R/G/B fields
  // 
  // 2d array instead of 3d *****
  rgb color[MAX_DIM][MAX_DIM];
};

//
// routines to read and write BMP images
//

#ifdef __cplusplus
extern "C" {
#endif

int cs1300bmp_readfile(char *filename, struct cs1300bmp *image);
int cs1300bmp_writefile(char *filename, struct cs1300bmp *image);

#ifdef __cplusplus
}
#endif


#endif
