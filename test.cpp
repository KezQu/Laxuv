#include <cmath>
#include <cstdint>
#include <iostream>

uint32_t constexpr MeshSize = 1000;
uint32_t granularity[MeshSize];

int main(int argc, char const* argv[])
{
  for (int gl_LocalInvocationIndex = 0; gl_LocalInvocationIndex < MeshSize;
       gl_LocalInvocationIndex++)
  {
    granularity[gl_LocalInvocationIndex] = 0;
  }
  granularity[0] = 1;
  granularity[123] = 1;
  for (int gl_LocalInvocationIndex = 0; gl_LocalInvocationIndex < MeshSize;
       gl_LocalInvocationIndex++)
  {
    for (int i = 0; i < granularity[gl_LocalInvocationIndex]; i++)
    {
      printf("%d\n", i * MeshSize + gl_LocalInvocationIndex);
    }
  }
}