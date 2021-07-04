#pragma once

#include <glad/glad.h>

class GpuProfiler
{
   enum { FRAMES = 4 };

   GLuint gpuProfilerQueries[FRAMES];
   GLuint64 gpuElapsedTimeNS;
   int currentIndex;
public:
    void init();
    void destroy();

    void begin();

    /// <summary>
    /// Stops profiling and returns result from a few frames ago
    /// </summary>
    /// <returns>The time profiling in milliseconds</returns>
    float end();
};