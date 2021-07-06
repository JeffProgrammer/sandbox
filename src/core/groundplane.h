#pragma once

// render as GL_TRIANGLE_STRIP
// position, normal x4
 const static float groundPlaneVertexBuffer[] = {
   0.5,0.5,0.5, 0,1,0,   -0.5,0.5,0.5, 0,1,0,   -0.5,0.5,-0.5, 0,1,0,   0.5,0.5,-0.5, 0,1,0,  // up
 };

 const static short groundPlaneIndexBuffer[] = {
     0, 2, 1, 0, 3, 2,
 };

 const static int groundPlaneNumberOfVerts = 4;