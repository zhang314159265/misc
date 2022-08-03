#include <stdio.h>

/*
 * This example does not generate phi instruction since the value of d is conveyed
 * thru its address from the true/false branches.
 */
int main(void) {
  int a = 1, b = 2, c = 3, d;
  if (a > 0) {
    d = b + c;
  } else {
    d = b - c;
  }
  printf("d is %d\n", d);
  return 0;
}
