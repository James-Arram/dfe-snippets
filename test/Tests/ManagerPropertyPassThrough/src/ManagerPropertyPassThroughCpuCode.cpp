/***
    This is a simple demo project that you can copy to get started.
    Comments blocks starting with '***' and subsequent non-empty lines
    are automatically added to this project's wiki page.
*/

#include <stdio.h>

#include <vector>
#include <iostream>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const int inSize = 384;

  std::vector<int> a(inSize), b(inSize), expected(inSize), out(inSize, 0);

  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
    b[i] = i - 1;
    expected[i] = 2 * i;
  }

  std::cout << "Running on DFE." << std::endl;
  ManagerPropertyPassThrough(inSize, &a[0], &b[0], &out[0]);


  /***
      Note that you should always test the output of your DFE
      design against a CPU version to ensure correctness.
  */
  for (int i = 0; i < inSize; i++)
    if (out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
        i, out[i], expected[i]);
      return 1;
    }

  std::cout << "Test passed!" << std::endl;
  return 0;
}
