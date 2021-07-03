#pragma once

const static float cubeVertsBuffer[] = {
   1,0,1, 1,0,0,   1,1,1, 1,0,0,   1,1,0, 1,0,0,   1,0,0, 1,0,0,  // east
   1,1,1, 0,1,0,   0,1,1, 0,1,0,   0,1,0, 0,1,0,   1,1,0, 0,1,0,  // up
   0,1,1, -1,0,0,  0,0,1, -1,0,0,  0,0,0, -1,0,0,  0,1,0, -1,0,0, // west
   0,0,1, 0,-1,0,  1,0,1, 0,-1,0,  1,0,0, 0,-1,0,  0,0,0, 0,-1,0, // down
   0,1,1, 0,0,1,   1,1,1, 0,0,1,   1,0,1, 0,0,1,   0,0,1, 0,0,1, // north
   0,0,0, 0,0,-1,  1,0,0, 0,0,-1,  1,1,0, 0,0,-1,  0,1,0, 0,0,-1, // south
};

const static short cubeIndices[] = {
   0, 2, 1, 0, 3, 2,
   4, 6, 5, 4, 7, 6,
   8, 10, 9, 8, 11, 10,
   12, 14, 13, 12, 15, 14,
   16, 18, 17, 16, 19, 18,
   20, 22, 21, 20, 23, 22
};