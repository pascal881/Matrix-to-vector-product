#include <stdio.h>
#include "mpi.h"

int main( int argc, char *argv[] ){
    
    /* Dichiarazione delle variabili per il calcolo del tempo,speed-up ed efficienza */
	double t0, t1, time;
	
    /* Variabile principali del programma */
    int menum, nproc, n, cloc, i, j;
    
	/* Inizializzo l'ambiente MPI */
	MPI_Init(&argc,&argv);
    
	MPI_Comm_size(MPI_COMM_WORLD,&nproc); //Ricevo nella variabile nproc il numero totale di processori utilizzati per l'algoritmo
	MPI_Comm_rank(MPI_COMM_WORLD,&menum);  // Inserisco nella variabile menum l'identificativo corrente del processore che esegue l'algoritmo
    
    /* Controllo di eventuali input da linea di comando per il size della matrice */
	
    /* Valore di default dimesione */
    n = 128;
	 
    /* Controllo se c'è input da tastiera */
    if(argc==2)
		n = atoi(argv[1]);
    
    /* Controllo che il numero inserito sia un multiplo di nproc */
    if(n%nproc!=0){
        /* Se non è un multiplo termina il programma */
        
        printf("\nAttenzione n deve essere un multiplo del numero di processi utilizzati ! \n");
        MPI_Abort(MPI_COMM_WORLD,0);
    }
    
    else{
        
    /*Altrimenti viene calcolato il numero di colonne per ogni processore */
	cloc = n/nproc;
        
	}
	if(menum==0){
		printf("Numero di processori: %d\n", nproc);
		printf("Dimensioni matrice A : %dx%d\n", n, n);
        printf("Dimensioni array b: %d\n", n);
        printf("Dimensioni array c: %d\n", n);
	}
    
	double A[n][n], b[n], y[n];
	double C[n][cloc];
	double c[n];
	double b_loc[cloc];
    
    /* Azzeramento del vettore per le somme locali */
    for(i=0; i<n; i++){
		b[i] = 0;
	}
    
	if(menum==0){
        /* Riempimento array locale con numeri sequenziali */
        for(i=0; i<n; i++){
            b[i] = i+1;
    		//printf("a[%d]= %d\n", i,i+1);
        }
        
            /* Riempimento matrice con numeri sequenziali */
        
        for(i=0; i<n; i++){
			for(j=0; j<n; j++){
				A[i][j]=j+1;
			}
		}
    }
    
    /* Distribuzione vettore per ogni processore */
	MPI_Scatter(b,cloc,MPI_DOUBLE,b_loc,cloc,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
    
    /* Distribuzione matrice  passando per la trasposta */
    double At[n][n], D[cloc][n];
    
    /* Eseguo la trasposta della matrice */
	if(menum==0){
		for(i=0; i<n; i++){
			for(j=0; j<n; j++){
				At[i][j] = A[j][i];
			}
		}
        
	}
	MPI_Scatter(At,n*cloc,MPI_DOUBLE,D,n*cloc,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	
	for(i=0; i<n; i++){
		for(j=0; j<cloc; j++){
			C[i][j] = D[j][i];
		}
	}
    if (menum==0) {
        t0 = MPI_Wtime(); //start al tempo
    }
    
	/* Eseguo il prodotto in modo parallelo (poichè¨ questa parte di codice verrà  chiamata da tutti i processori */
	for(i=0; i<n; i++){
		for(j=0; j<cloc; j++){
            c[i] = c[i] + C[i][j]*b_loc[j];
		}
	}
	
    /* Raccolta dei risultati parziali nel vettore c */
    for(i=0; i<n; i++){
		MPI_Reduce(&c[i],&y[i],1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
    
	
	if(menum==0){
		t1 = MPI_Wtime();
		//calcolo e stampa del tempo trascorso
		time = (t1-t0);
		printf("Tempo: %f\n", time);
        
        printf("\nIl vettore finale c inizia con : %.3f\n",y[0]);
        
    }
    
	MPI_Finalize();
	return 0;
}

