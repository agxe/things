/*

How to use:
  ./CalculateMismatch.exe cpu.txt gpu.txt 3840 2160

*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{

	if (argc != 5) {
		printf("option: ref.txt tar.txt width height\n");
		exit(1);
	}

	const char *refFile = argv[1];
	const char *tarFile = argv[2];

	int width = atoi(argv[3]);
	int height = atoi(argv[4]);

	FILE *fpRef = fopen(refFile, "r");
	FILE *fpTar = fopen(tarFile, "r");

	float maximumMismatch = 0.0f;
	float maximumMismatchPC = 0.0f;
	int maximumMismatchX = -1;
	int maximumMismatchY = -1;
	float maxValRef = 0.0f;
	float maxValTar = 0.0f;

	float totalMismatchFrac = 0.0f;
	int zeroCnt = 0;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float ref;
			float tar;

			fscanf(fpRef, "%f\n", &ref);
			fscanf(fpTar, "%f\n", &tar);

			float diff = ref - tar;
			if (fabsf(diff) > fabsf(maximumMismatch)) {
				maximumMismatch = diff;
				maximumMismatchPC =
				    (diff / fabsf(ref)) * 100.0f;
				maximumMismatchX = x;
				maximumMismatchY = y;
				maxValRef = ref;
				maxValTar = tar;
			}

			if (ref == 0.0f) {
				zeroCnt++;
			} else {
				totalMismatchFrac += (fabsf(diff) / fabsf(ref));
			}
		}
	}

	fclose(fpRef);
	fclose(fpTar);

	float avgMismatchPc =
	    (totalMismatchFrac / (width * height - zeroCnt)) * 100.0f;

	printf
	    ("ref: %s VS tar: %s \nAvg Mismatch = %.10f%% Maximum Mismatch PC = %.10f%% X = %d Y = %d RefVal = %.10f TarVal = %.10f\n",
	     refFile, tarFile, avgMismatchPc, maximumMismatchPC,
	     maximumMismatchX, maximumMismatchY, maxValRef, maxValTar);

}
