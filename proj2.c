/************************************/
/* * *	  VUT/FIT/IZP/proj2.c	* * */
/* * *							* * */
/* * *		  verze: 1			* * */
/* * *							* * */
/* * *		  David Rubý		* * */
/* * *		listopad 2018		* * */
/************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

//definice barev použitých pro nápovědu a chybová hlášení.
#define RED		"\x1B[31m"
#define CYAN	"\x1b[36m"
#define RESET	"\x1b[0m"


//Vypisuje nápovědu.
void help(){
	printf(CYAN "\n==help==\n"
				"A program for calculations of natural logarithms and exponential functions.\n\n"
				"--log X N\tX... number, N... number of iterations.\n"
				"--pow X Y N\tY... number, X... general basis, N... number of iterations.\n" RESET);
}

//Převádí char na double.
double my_lf_atoi(char *charnumber){
	double result;
	sscanf(charnumber, "%lf", &result);
	return result;
}

//Převádí char na unsigned int.
int my_u_atoi(char *charnumber){
	int result;
	sscanf(charnumber, "%u", &result);
	return result;
}

//Počítá přirozený logaritmus pomocí Taylorova polynomu.
double taylor_log(double x, unsigned int n){
	double result = 0.0, result_minus_one;
	
	if (x > 0.0 && x <= 1.0){										//Pokud je vstupní hodnota X vetší než 0 a zároveň menší nebo rovna 1, Použije se první způsob.
		double y = 1 - x, y_pow = y;
		result = -y;
		for(unsigned int i = 0; i < n; i++){
			if(isinf(result)){
				return result_minus_one;
			}
			if(isnan(result)){
				return result_minus_one;
			}
			result_minus_one = result;
			y_pow *= y;
			result -= y_pow / (double)(i + 2);
		}
	}
	else if (x > 1){												//Pokud je vstupní hodnota X větší než 1, použije se druhý způsob.
		double n_pow = 1;
		for(unsigned int i = 0; i < n; i++){
			if(isinf(result)){
				return result_minus_one;
			}
			if(isnan(result)){
				return result_minus_one;
			}
			result_minus_one = result;
			result += (((x - 1) / x) * n_pow) / (i + 1);
				n_pow *= (x - 1) / x;
		}
	}
	else{
		fprintf(stderr, RED "Warning. Incorrect X. X must be a positive number.\n" RESET);
		help();
		exit(1);
	}
	return result;
}
//Počítá přirozený logaritmus pomocí řetězeného zlomku.
double cfrac_log(double x, unsigned int n){
	unsigned int n1 = n;
	double z = (x - 1) / (x + 1), z_pow = z*z, result = 2 * n1 + 1, result_minus_one;

	if(x > 0.0){
		for(unsigned int i = n - 1; i > 0; i--){
			if(isinf(result)){
				return result_minus_one;
			}
			if(isnan(result)){
				return result_minus_one;
			}
			result_minus_one = result;
			n1--;
			result = (2 * n1 - 1) - ((z_pow * n1 * n1) / result);
		}
		result = 2 * z / result;
	}
	else{
		fprintf(stderr, RED "Warning. Incorrect X. X must be a positive number.\n" RESET);
		help();
		exit(1);
	}
	return result;
}

//Počítá exponenciální funkci ve funkci s prototypem pomocí zvoleného logaritmu.
double my_pow(double a, double x, unsigned n, int method){

	double ln;
	if(method == 1)
		ln = taylor_log(a, n);
	else if(method == 2)
		ln = cfrac_log(a, n);

	double result = 1, x_pow = x, /*ln = taylor_log(a, n),*/ ln_pow = ln, fac = 1, fac1 = fac, result_minus_one;
	
	if(a > 0.0){
		for (unsigned int i = 0; i < n; i++){
			if(isinf(result)){
				return result_minus_one;
			}
			if(isnan(result)){
				return result_minus_one;
			}
			result_minus_one = result;
			fac *=  fac1;
			result += x_pow * ln_pow / fac;
			x_pow *= x;
			ln_pow *= ln;
			fac1++;
		}
	}
	else{
		fprintf(stderr, RED "Warning. Incorrect a. a must be a positive number\n" RESET);
		help();
		exit(1);
	}
	return result;
}

//volá funkci na výpočet exponenciální funkce za použití logarytmu vypočteného pomocí Taylorova polynomu.
double taylor_pow(double a, double x, unsigned n){
	return my_pow(a, x, n, 1);
}

//volá funkci na výpočet exponenciální funkce za použití logarytmu vypočteného pomocí řetězeného zlomku.
double cfrac_pow(double a, double x, unsigned n){
	return my_pow(a, x, n, 2);
}


int main(int argc, char *argv[]){
	
	//Pokud je některý z argumentů "--help", "-help", "--h" nebo "-h", vypíše se nápověda.
	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--h") == 0 || strcmp(argv[i], "-h") == 0){
			help();
			return 0;
		}
	}
	
	//Ověřuje, pokud je k zadané funkci odpovídající počet argumentů a zda je použita platná funkce.
	if(strcmp(argv[1], "--log") == 0 && argc != 4){
		fprintf(stderr, RED "Warning. Wrong input. You must input two values, while using --log function.\n" RESET);
		help();
		return 1;
	}
	else if(strcmp(argv[1], "--pow") == 0 && argc != 5){
		fprintf(stderr, RED "Warning. Wrong input. You must input three values, while using --pow function.\n" RESET);
		help();
		return 1;
	}
	else if(strcmp(argv[1], "--log") != 0 && strcmp(argv[1], "--pow") != 0){
		fprintf(stderr, RED "Warning. Wrong input.\n" RESET);
		help();
		return 1;
	}

	//Pokud argumenty po argumentu určující funkcionalitu nejsou v číselné podobě, vypíše se chybová hláška.
	for(int i = 2; i < argc; i++){
		for(unsigned int j = 0; j < strlen(argv[i]); j++){
			if((argv[i][j] < 48 || argv[i][j] > 57) && argv[i][j] != 44 && argv[i][j] != 46){						//Porovnává znaky pomocí ASCII tabulky.
				fprintf(stderr, RED "Warning. Wrong input. The 3rd, 4th and 5th argument must be a number.\n" RESET);
				help();
				return 1;
			}
		}
	}


	//Rozděluje běh programu podle zadaného argumentu a vypisuje výsledky.
	switch(argv[1][2]){
		case 'h':
			help();
			break;
		case 'l':
			printf("       log(%g) = %.12g\n", my_lf_atoi(argv[2]), log(my_lf_atoi(argv[2])));
			printf(" cfrac_log(%g) = %.12g\n", my_lf_atoi(argv[2]), cfrac_log(my_lf_atoi(argv[2]), my_u_atoi(argv[3])));
			printf("taylor_log(%g) = %.12g\n", my_lf_atoi(argv[2]), taylor_log(my_lf_atoi(argv[2]), my_u_atoi(argv[3])));
			break;
		case 'p':
			printf("       pow(%g, %g) = %.12g\n", my_lf_atoi(argv[2]), my_lf_atoi(argv[3]), pow(my_lf_atoi(argv[2]), my_lf_atoi(argv[3])));
			printf(" cfrac_pow(%g, %g) = %.12g\n", my_lf_atoi(argv[2]), my_lf_atoi(argv[3]), cfrac_pow(my_lf_atoi(argv[2]), my_lf_atoi(argv[3]), my_u_atoi(argv[4])));
			printf("taylor_pow(%g, %g) = %.12g\n", my_lf_atoi(argv[2]), my_lf_atoi(argv[3]), taylor_pow(my_lf_atoi(argv[2]), my_lf_atoi(argv[3]), my_u_atoi(argv[4])));
			break;
		default:
			fprintf(stderr, RED "Warning. Incorect function.\n" RESET);
			help();
			return 1;
	}
	return 0;
}