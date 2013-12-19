#include <gmp.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

unsigned int PREC;
mpf_t pi;

int main(int argc, char * * argv)
{
	int k;
	mpf_t one;
	mpf_t three;
	mpf_t negone;
	mpf_t two;
	mpf_t negthird;
	mpf_t sr12;
	mpf_t num;
	mpf_t denom;
	mpf_t tmp;
	FILE *f;

	if (argc < 2)
	{
		printf("%s <prec> [file]\n", argv[0]);
		return 1;
	}

	PREC = atol(argv[1]);
	if (argc > 2)
		f = fopen(argv[2], "w");

	mpf_set_default_prec(4 * PREC);
	mpf_init(pi);
	mpf_init(one);
	mpf_init(two);
	mpf_init(three);
	mpf_init(negone);
	mpf_init(negthird);
	mpf_init(sr12);
	mpf_init(num);
	mpf_init(denom);
	mpf_init(tmp);

	mpf_set_ui(pi, 0);
	mpf_set_ui(one, 1);
	mpf_set_ui(two, 2);
	mpf_set_ui(three, 3);
	mpf_set_si(negone, -1);
	mpf_sqrt_ui(sr12, 12);
	mpf_div(negthird, negone, three);
	mpf_set_ui(num, 1);
	mpf_set_ui(denom, 1);

	printf("Alloc\r");
	fflush(stdout);
	for (k = 0; k < PREC; k++)
	{
		mpf_pow_ui(num, negthird, k);
		mpf_set_ui(denom, 2*k + 1);
		mpf_div(tmp, num, denom);
		mpf_add(pi, pi, tmp);
		//if (k % 50 == 0)
		{
			double progress = k;
			progress /= PREC;
			progress *= 100;
			printf("\rProgress: %.2lf\r", progress);
			fflush(stdout);
		}
	}

	mpf_mul(pi, pi, sr12);
	gmp_printf("%.*Ff\n", PREC, pi);
	if (f)
		gmp_fprintf(f, "%.*Ff", PREC, pi);
}
