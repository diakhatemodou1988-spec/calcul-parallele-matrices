	#include <stdio.h>
			#include <stdlib.h>
			#include <time.h>
			
			/*
			* Allocation dynamique d'une matrice n x n
			*/
			static double *alloc_matrix(int n)
			{
				double *m = malloc((size_t)n * n * sizeof(double));
				
				if (m == NULL)
				{
					fprintf(stderr, "Erreur : allocation memoire impossible.\n");
					exit(EXIT_FAILURE);
				}
				
				return m;
			}
			
			/*
			* Initialisation d'une matrice avec des valeurs aleatoires
			* comprises entre 0 et 1.
			*/
			static void init_random(double *m, int n, unsigned int seed)
			{
				srand(seed);
				
				for (int i = 0; i < n * n; i++)
				{
					m[i] = (double)rand() / RAND_MAX;
				}
			}
			
			/*
			* Transposition de la matrice B.
			*
			* BT[j][k] = B[k][j]
			*
			* Cela permet d'effectuer ensuite des acces contigus
			* en memoire lors de la multiplication.
			*/
			static void transpose_matrix(const double *B, double *BT, int n)
			{
				for (int i = 0; i < n; i++)
				{
					for (int j = 0; j < n; j++)
					{
						BT[j * n + i] = B[i * n + j];
					}
				}
			}
			
			/*
			* Multiplication matricielle sequentielle :
			*
			* C = A x B
			*
			* Formule :
			* C[i][j] = somme(A[i][k] * B[k][j])
			*
			* Ici, on utilise BT = transpose(B) afin d'ameliorer
			* les acces memoire.
			*/
			static void matmul_seq(const double *A,
			const double *BT,
			double *C,
			int n)
			{
				for (int i = 0; i < n; i++)
				{
					for (int j = 0; j < n; j++)
					{
						double sum = 0.0;
						
						for (int k = 0; k < n; k++)
						{
							sum += A[i * n + k] * BT[j * n + k];
						}
						
						C[i * n + j] = sum;
					}
				}
			}
			
			/*
			* Calcul d'une somme de controle permettant de verifier
			* le resultat obtenu.
			*/
			static double checksum(const double *C, int n)
			{
				double sum = 0.0;
				
				for (int i = 0; i < n * n; i++)
				{
					sum += C[i];
				}
				return sum;
			}
			int main(int argc, char *argv[])
			{
				/*
				* Taille de la matrice :
				*
				* ./seq 1000
				*
				* Si aucune taille n'est fournie, N = 1000.
				*/
				int n = (argc > 1) ? atoi(argv[1]) : 1000;
				
				if (n <= 0)
				{
					fprintf(stderr, "Erreur : N doit etre strictement positif.\n");
					return EXIT_FAILURE;
				}
				double *A  = alloc_matrix(n);
				double *B  = alloc_matrix(n);
				double *BT = alloc_matrix(n);
				double *C  = alloc_matrix(n);
				/*
				* Initialisation
				*/
				init_random(A, n, 42);
				init_random(B, n, 123);
				/*
				* Transposition de B
				*
				* Cette operation n'est pas incluse dans le temps
				* de multiplication afin de mesurer uniquement
				* le calcul matriciel.
				*/
				transpose_matrix(B, BT, n);
				/*
				* Mesure du temps de multiplication
				*/
				struct timespec start, end;
				clock_gettime(CLOCK_MONOTONIC, &start);
				matmul_seq(A, BT, C, n);
				clock_gettime(CLOCK_MONOTONIC, &end);
				/*
				* Calcul du temps ecoule
				*/
				double elapsed =
				(end.tv_sec - start.tv_sec)
				+ (end.tv_nsec - start.tv_nsec) / 1e9;
				/*
				* Verification du resultat
				*/
				double check = checksum(C, n);
				printf("============================================================\n");
				printf("                 CALCUL MATRICIEL SEQUENTIEL                \n");
				printf("============================================================\n");
				printf("  Taille de la matrice       : %d x %d\n", n, n);
				printf("  Temps d'execution          : %.6f secondes\n", elapsed);
				printf("  Somme de controle          : %.10f\n", check);
				printf("------------------------------------------------------------\n");
				printf("  Statut                     : EXECUTION TERMINEE\n");
				printf("============================================================\n");
				free(A);
				free(B);
				free(BT);
				free(C);
				return EXIT_SUCCESS;
			}
