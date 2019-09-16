#define MRC2 "mrc2.dct"
#include <stdio.h>;

main()
{
int i, flag;
FILE *fp, *fopen();
char line[200], c;

		if ((fp = fopen (MRC2, "r")) == NULL) {
			fprintf(stderr, " can't open  %s  as data file \n", MRC2);
			exit(1);
		}


	while (fgets(line, 200, fp) != NULL) {
		flag = 0;
		for( i=5; i < 42; i++ ){
			if (line[i] > '0')
				flag = 1;
		}
	if (flag > 0)
		printf("%s", line);
	}
}
