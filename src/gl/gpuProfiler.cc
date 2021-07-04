#include "gl/gpuProfiler.h"

void GpuProfiler::init()
{
   glGenQueries(FRAMES, gpuProfilerQueries);
   currentIndex = 0;
   gpuElapsedTimeNS = 0;

   // Prime queries so they all have a value
   for (int i = 0; i < FRAMES; i++)
   {
      glBeginQuery(GL_TIME_ELAPSED, gpuProfilerQueries[i]);
      glEndQuery(GL_TIME_ELAPSED);
   }
}

void GpuProfiler::destroy()
{
   glDeleteQueries(FRAMES, gpuProfilerQueries);
}

void GpuProfiler::begin()
{
   int qId = currentIndex++;
   if (currentIndex == FRAMES)
      currentIndex = 0;

   glBeginQuery(GL_TIME_ELAPSED, gpuProfilerQueries[qId]);
}

float GpuProfiler::end()
{
   glEndQuery(GL_TIME_ELAPSED);

   // We always read the farthest back of frames behind.
   int toReadId = (currentIndex + (FRAMES - 1)) % FRAMES;
   glGetQueryObjectui64v(gpuProfilerQueries[toReadId], GL_QUERY_RESULT, &gpuElapsedTimeNS);

   return (float)((double)gpuElapsedTimeNS / 1000000.0);
}