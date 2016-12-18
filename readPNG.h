/*
 * Mostly Based on https://gist.githubusercontent.com/niw/5963798/raw/c5cb453332e18a2e6ea9cd9b42c56546b0394f4e/libpng_test.c
 */

#ifndef __READ_PNG__
#define __READ_PNG__

/** Reads a PNG picture from the specified file.
 * @param filename [in] the path of the file to process
 * @param width [out] the width of the read picture
 * @param height [out] the height of the read picture
 * @param pixels [out] an array of pixels that will be allocated
 * @return 0 if success, -1 if failed
 */
int pngReadFile(const char *filename, int *width, int *height, void** pixels);

#endif
