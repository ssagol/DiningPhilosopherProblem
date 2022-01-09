#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>  //sleep için 


#define NUMP 8  // filzoof sayısı


pthread_mutex_t fork_mutex[NUMP]; 
pthread_mutex_t yemek;

pthread_cond_t space_avail = PTHREAD_COND_INITIALIZER;

int yemek_sira = 0;

int input_al(){ // kaç masa
    int inp;
    printf("grup sayisini giriniz");
    scanf("%d", &inp);
    return inp;
}


void *diner(void *arg){
        
        int philo;  

        philo = (int) arg;
        int eating = 0;  // kaç kez yemek

        printf("ben %d\n", philo);

        while (eating < 8) {  // filozfo
                printf("%d think\n", philo);
                sleep( philo / 2 );
                printf("%d hungry\n", philo );

                pthread_mutex_lock(&yemek); // kilitleme ile
                //sıra kimdeyse o yer diğerleri bekler

                if (yemek_sira == (NUMP - 1)) // boş koltuk kontrol
                    
                        pthread_cond_wait(&space_avail, &yemek);
                        //boş koltuk yoksa bekler

                yemek_sira++;

                pthread_mutex_unlock(&yemek);  // yer boşalması için

                pthread_mutex_lock(&fork_mutex[philo]);
                pthread_mutex_lock(&fork_mutex[( philo + 1) % NUMP]);

                printf("%d eating\n", philo);

                eating++;  // kaç kez yediğini saymak için

                sleep(1);  // askıya alma bekleme için

                printf("%d yemeği bitti\n", philo);
                pthread_mutex_unlock(&fork_mutex[philo]);
                pthread_mutex_unlock(&fork_mutex[(philo + 1) % NUMP]);

               
                pthread_mutex_lock(&yemek);

                if (yemek_sira == (NUMP - 1))   // kişi azaldığında 
                        pthread_cond_signal(&space_avail); // çıkmak için
                yemek_sira--; // kişi sayısı azalı

                pthread_mutex_unlock(&yemek); // yemek yiyeni açmak için
        }

       
        pthread_exit(NULL);
}

int main(){

        input_al();
        int k = input_al();
        int s = k*NUMP;
        uintptr_t i;
       
        pthread_t yemek_bekleme[s];  

        pthread_mutex_init(&yemek, NULL); // sıra bekleeme dışlama
        
        for (i=0; i < s; i++)
                pthread_mutex_init(&fork_mutex[i], NULL);
		// semafor nesnesi oluşturma yani unlock lock
        
        for (i=0; i < s; i++){
                pthread_create(&yemek_bekleme[i], NULL, diner, (void *) i);
                // filozofrların idlerine göre üretilmesi
        }

        
        for (i=0; i < s; i++)
                pthread_join(yemek_bekleme[i], NULL);
                // bir filozofou beklerken diğerlerinn bitimresi için 


        exit(EXIT_SUCCESS);
}

