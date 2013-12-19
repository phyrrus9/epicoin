#include <stdio.h>
#include <gmp.h>
#include <mpfr.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

mpfr_t epi;
unsigned int PREC;

int main(int argc, char * * argv)
{
	mpf_t e;
	mpf_t pi;
	mpfr_t er;
	mpfr_t pir;
	FILE *a;
	FILE *b;
	FILE *f;

	a = fopen(argv[1], "r");
	b = fopen(argv[2], "r");
	f = fopen(argv[3], "w");
	PREC = atol(argv[4]);
	mpf_set_default_prec(1024048);
	mpf_init(e);
	mpf_init(pi);
	mpfr_set_default_prec(2048096);
	mpfr_init(er);
	mpfr_init(pir);

	mpfr_init(epi);

	gmp_fscanf(a, "%Ff", &e);
	gmp_fscanf(b, "%Ff", &pi);
	fclose(a);
	fclose(b);

	mpfr_set_f(er, e, GMP_RNDN);
	mpfr_set_f(pir, pi, GMP_RNDN);

	//gmp_printf("e: %.*Ff\np: %.*Ff\n", PREC, e, pi);
//	mpfr_printf("e: %.*Rf\np: %.*Rf\n", PREC, er, PREC, pir);

	mpfr_pow(epi, er, pir, GMP_RNDN);
	mpfr_printf("epi: %.*Rf\n", PREC, epi);
	mpfr_fprintf(f, "%.*Rf", PREC, epi);
}
