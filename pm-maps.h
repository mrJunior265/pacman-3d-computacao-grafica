#ifndef __PM_MAPS_H__
#define __PM_MAPS_H__

//  Library for reading PAC-MAN Maps files
//  (c) Luis Baptista @ ipportalegre.pt 8-january-2022
//
//  A PAC-MAN Maps File has the following structure
//    file :- <numberOfMaps>
//            <map>*
//
//    map :- <numberOfGhost> <numberOfSmartGhosts>
//           <xTabSize> <yTabSize>
//           <0/1-stringLine>*

//  In memory, a PAC-MAN Map is a XY-matrix of char with 0/1 in each position
//    0 - an obstacle
//    1 - a cell in the map

// IMPORTANT: All the memory for the maps is dynamically managed inside the library (malloc and free).
//   WARNING: The map matrix is intended only for copy for your own strutures!
//            When getting the next map, the memory allocated for the previous one is deleted!

void openMapsFile(char* name);
void closeMapsFile();

int numberOfMaps();

char** nextMap();
char** getMap();       // the last map read
int xTabSize();
int yTabSize();
int numGhosts();
int numSmartGhosts();

void printMap();

#endif // !__PM_MAPS_H__
