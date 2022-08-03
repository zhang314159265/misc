#include <stdio.h>

int main(void) {
	float mat[10][10];
	#if 1
	for (int i = 0; i < 10; ++i) {
	  for (int j = 0; j < 10; ++j) {
		  mat[i][j] = 0;
		}
	}
	#endif
	for (int i = 0; i < 10; ++i) {
	  mat[i][i] = 1;
	}
	float sum = 0;
	#if 1
	for (int i = 0; i < 10; ++i) {
	  for (int j = 0; j < 10; ++j) {
			sum += mat[i][j];
		}
	}
	#endif
	printf("sum is %f\n", sum);
	return 0;
}
