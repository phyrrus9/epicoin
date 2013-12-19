#include <gmp.h>
#include <stdio.h>
#include <iostream>
#include <math.h>


unsigned int PREC = 0;

mpf_t e;

void find_denom(mpf_t &denom)
{
	//gmp_printf("%Ff!=", denom);
	mpf_t newdenom;
	mpf_t olddenom;
	mpf_t one;
	mpf_t i;

	mpf_init(newdenom);
	mpf_init(olddenom);
	mpf_init(one);
	mpf_init(i);

	mpf_set_ui(newdenom, 1);
	mpf_set(olddenom, denom);
	mpf_set_ui(one, 1);
	mpf_set_ui(i, 0);

	for (mpf_set_ui(i, 0); mpf_cmp(i, denom) < 0; mpf_add(i, i, one))
	{
		mpf_mul(newdenom, newdenom, olddenom);
		mpf_sub(olddenom, olddenom, one);
	}
	mpf_set(denom, newdenom);

	mpf_clear(newdenom);
	mpf_clear(olddenom);
	mpf_clear(one);
	mpf_clear(i);
	//gmp_printf("%Ff\n", denom);
}

int main(int argc, char * * argv)
{
	mpf_t i;
	mpf_t end;
	mpf_t iter;
	mpf_t numer;
	mpf_t denom;
	mpf_t one;
	mpf_t progress;
	mpf_t hundred;
	FILE *f;
	int count;

	if (argc < 2)
	{
		printf("%s <precision> [file]\n", argv[0]);
		return 1;
	}

	mpf_set_default_prec(1048576);
	if (argc > 2)
		f = fopen(argv[2], "w");
	else
		f = fopen("/tmp/e", "w");
	PREC = atol(argv[1]);

	mpf_init(i);
	mpf_init(end);
	mpf_init(iter);
	mpf_init(numer);
	mpf_init(denom);
	mpf_init(one);
	mpf_init(e);
	mpf_init(progress);
	mpf_init(hundred);

	mpf_set_ui(i, 0);
	mpf_set_ui(end, PREC * 1.5);
	mpf_set_ui(iter, 0);
	mpf_set_ui(numer, 1);
	mpf_set_ui(denom, 1);
	mpf_set_ui(one, 1);
	mpf_set_ui(e, 0);

	for (count = 0, mpf_set_ui(i, 0); mpf_cmp(i, end) < 0; mpf_add(i, i, one))
	{
		mpf_set(denom, i);
		find_denom(denom);
		mpf_div(iter, numer, denom);
		mpf_add(e, e, iter);
		count++;
		if (count >= 50)
		{
			printf("\r%50s\r", " ");
			printf("Precision: %-5d (%-5d) Progress: %.2lf\r", PREC, (int)(PREC * 1.5), (count / (PREC * 1.5)) * 100);
			fflush(stdout);
		}
	}
	gmp_printf("%.*Ff\n", PREC, e);
	gmp_fprintf(f, "%.*Ff\n", PREC, e);
	fclose(f);
}
