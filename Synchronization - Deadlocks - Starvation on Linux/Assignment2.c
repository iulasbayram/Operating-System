//Student Name: Ismail Ulaş Bayram//
//Student ID: 220201040//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//Char arrays are our shared variables and they represent their own text files.
char filenameN1[] = "numbers1.txt";
char filenameN2[] = "numbers2.txt";
char filenameR[] = "resource.txt";


//There are 3 mutexes that refers to file names. lockN1, lockN2 and lockR represent filenameN1, filenameN2 and filenameR respectively.
pthread_mutex_t lockN1;
pthread_mutex_t lockN2;
pthread_mutex_t lockR;

//These are flags that determine working of threads.
//flagN1 and flagN2 respectively refers that number1.txt and number2.txt are full or empty.
//If at least one of flagN1 and flagN2 is 1, then all of threads work.
//If all threads are done its work, one last time flagR_negative and flagR_prime checks whether there are any prime or negative numbers in resorce list or not.
int flagN1 = 1;
int flagN2 = 1;
int flagR_negative = 1;
int flagR_prime = 1;

//These are threadwork methods that refers to our threads (thread A, thread B, thread C).
void *threadwork_A(void *parameters);
void *threadwork_B(void *parameters);
void *threadwork_C(void *parameters);

//These function checks number is prime or not. If num is prime, this method returns 1. If not, this method returns 0.
int prime_or_not(int num);

//These fuction deletes all prime numbers in list at a time.
void delete_prime_number(char *filename);

//These fuction deletes all negative numbers in list at a time.
void delete_negative_number(char *filename);

//These fuction cuts a single line from one list and pastes the line to another list. (Our cutted_filename is number1.txt or number2.txt, pasted_filename is always resource.txt)
void cut_and_past_number(char *cutted_filename, char *pasted_filename);

//THREAD_PARAMETERS takes thread number for thread A.
typedef struct
{
	int thread_No;

}THREAD_PARAMETERS;

int main()
{
	THREAD_PARAMETERS* lpParameter;
	lpParameter = malloc(sizeof(THREAD_PARAMETERS)* 4);

	//I defined all threads.
	pthread_t thread_A_Handler[4], thread_B, thread_C;

	//I initialized all mutexes.
	pthread_mutex_init(&lockN1,NULL);
  	pthread_mutex_init(&lockN2,NULL);
  	pthread_mutex_init(&lockR,NULL);

  	//I created all threads
	for (int i = 0; i < 4; ++i){
    	lpParameter[i].thread_No = i;
    	pthread_create(&thread_A_Handler[i],NULL,threadwork_A,&lpParameter[i]);
  	}

  	pthread_create(&thread_B,NULL,threadwork_B ,NULL);
  	pthread_create(&thread_C,NULL,threadwork_C ,NULL);

  	//I destroyed all threads after finishing their works.
 	for (int i = 0; i <4; ++i)
  	{
    	pthread_join(thread_A_Handler[i],NULL);
  	}

  	pthread_join(thread_B,NULL);
  	pthread_join(thread_C,NULL);

  	//I destroyed all mutexes after finishing their works.
  	pthread_mutex_destroy(&lockN1);
  	pthread_mutex_destroy(&lockN2);
  	pthread_mutex_destroy(&lockR);

	int line_Num = 0;
	int num = 0;
  	FILE *resource_file = fopen(filenameR, "r");
  	while(fscanf(resource_file, "%d", &num) > 0){
		line_Num ++;
	}
	fclose(resource_file);

  	printf("\nThe resource file is created successfully!\n\n");
  	printf("The number of lines of resource file is: %d\n\n", line_Num);

}

//Below that, these function checks number is prime or not. If num is prime, this method returns 1. If not, this method returns 0.
int prime_or_not(int num) {
    int i;
    int prime = 1;

    for(i = 2; i < num; i++) {

        if(num % i == 0){

            prime = 0;
            return prime;
        }

    }
    return prime;

}

//Below that these function deletes all prime numbers in list at a time.
void delete_prime_number(char *filename){
    int num;
    int flag_Controller = 0; //flag_Controller checks if there is no number in source file (filename) , then it sets flagN1 or flagN2 to 0.
    int resource_Controller = 0; //resource_Controller check if there is any prime number in source file or not.

    char buff[50];

    //I open two lists. One of them (filename) is to read numbers. Other (temp2.txt) is to append numbers that are not prime.
    FILE *file = fopen(filename, "r");
    FILE *file_temp = fopen("temp2.txt", "a");

    while(fgets(buff,50, file) != NULL) {
		
		num = atoi(buff);
		flag_Controller ++; //flag_Controller is updated if there is any number.

    	if (prime_or_not(num) == 0 || num <= 1)
    	{	
    		fprintf(file_temp, "%s", buff);  //If the number is not prime, the number is appended to temp2.txt.

    	}else{
    		resource_Controller ++; //resorce_Controller is updated if there is any prime number.
    	}
    	
    }

	fclose(file);
	fclose(file_temp);

	//If flag_Controller is positive, it means that there is any number in list and we can append elements of temporary textfile (temp2.txt) to source file (filename).
	if(flag_Controller > 0) {
		
		remove(filename);
		file = fopen(filename, "a");
		file_temp = fopen("temp2.txt", "r");
		
		while(fgets(buff,50, file_temp) != NULL){
			
			fprintf(file, "%s", buff);
		}
		fclose(file);
		fclose(file_temp);
	}

	//After all operations, we can remove temporary text file.
	remove("temp2.txt");


	//If our source file (filename) is equal to filenameN1 (numbers1.txt) or filenameN2 (numbers2.txt) and flag_Controller is zero (which means there is no any number in source file)
	//then flagN1 or flagN2 is equal to zero.
	if (strcmp(filename,filenameN1) == 0 && flag_Controller == 0)
	{
		flagN1 = 0;

	}else if(strcmp(filename,filenameN2) == 0 && flag_Controller == 0 ){
		
		flagN2 = 0;
	}

	//If our source file (filename) is equal to filenameR (resource.txt) and resource_Controller is 0 (which means there is no any prime number in source list)
	//and numbers1.txt and numbers2.txt is empty, flagR_prime is 0.
	if (strcmp(filename,filenameR) == 0 && resource_Controller == 0 && flagN1 == 0 && flagN2 == 0)
	{
		flagR_prime = 0;
	
	}

}

//Below that these function deletes all prime numbers in list at a time.
void delete_negative_number(char *filename){
    int num;
    int flag_Controller = 0; //flag_Controller checks if there is no number in source file (filename) , then it sets flagN1 or flagN2 to 0.
    int resource_Controller = 0; //resource_Controller check if there is any negative number in source file or not.

    char buff[50];

    //I open two lists. One of them (filename) is to read numbers. Other (temp1.txt) is to append numbers that are not negative.
	FILE *file = fopen(filename, "r");
    FILE *file_temp = fopen("temp1.txt", "a");

    while(fgets(buff,50, file) != NULL) {

    	num = atoi(buff);
    	flag_Controller ++; //flag_Controller is updated if there is any number.

    	if (num >= 0)
    	{	
    		fprintf(file_temp, "%s", buff); //If the number is not negative, the number is appended to temp1.txt.

    	}else{
    		resource_Controller ++; //resorce_Controller is updated if there is any negative number.
    	}
    	
    }

	fclose(file);
	fclose(file_temp);

	//If flag_Controller is positive, it means that there is any number in list and we can append elements of temporary textfile (temp1.txt) to source file (filename).
	if(flag_Controller > 0) {
		remove(filename);
		file = fopen(filename, "a");
		file_temp = fopen("temp1.txt", "r");

		while(fgets(buff,50, file_temp) != NULL){
			fprintf(file, "%s", buff);
		}
		fclose(file);
		fclose(file_temp);
	}

	//After all operations, we can remove temporary text file.
	remove("temp1.txt");

	//If our source file (filename) is equal to filenameN1 (numbers1.txt) or filenameN2 (numbers2.txt) and flag_Controller is zero (which means there is no any number in source file)
	//then flagN1 or flagN2 is equal to zero.
	if (strcmp(filename,filenameN1) == 0 && flag_Controller == 0)
	{
		flagN1 = 0;

	}else if(strcmp(filename,filenameN2) == 0 && flag_Controller == 0){

		flagN2 = 0;
	}

	//If our source file (filename) is equal to filenameR (resource.txt) and resource_Controller is 0 (which means there is no any negative number in source list)
	//and numbers1.txt and numbers2.txt is empty, flagR_negative is 0.
	if (strcmp(filename,filenameR) == 0 && resource_Controller == 0 && flagN1 == 0 && flagN2 == 0)
	{
		flagR_negative = 0;
	
	}

}

//These fuction cuts a single line from one list and pastes the line to another list. (Our cutted_filename is number1.txt or number2.txt, pasted_filename is always resource.txt)
void cut_and_past_number(char *cutted_filename, char *pasted_filename){
	int num;
	int flag_Controller = 0; //flag_Controller checks if there is no number in source file (cutted_filename) , then it sets flagN1 or flagN2 to 0.
	char buff[50];
	
	//I opened there files. A single line is cut from cutted_filename. This single line is pasted to pasted_filename. Rest of numbers of cutted_filename is pasted to temp.txt .
	FILE *file_c = fopen(cutted_filename, "r");
	FILE *file_p = fopen(pasted_filename,"a");
	FILE *file_temp = fopen("temp.txt","w");


	if(fgets(buff,50, file_c) != NULL){
		fprintf(file_p, "%s", buff);			
	}

	while(fgets(buff,50, file_c) != NULL){
		num = atoi(buff);
		fprintf(file_temp, "%d\n", num);

		flag_Controller ++; //flag_Controller is updated if there is any number.
	}

	fclose(file_c);
	fclose(file_p);
	fclose(file_temp);

	//If flag_Controller is positive, it means that there is any number in list and we can append elements of temporary textfile (temp.txt) to source file (cutted_filename).
	if(flag_Controller > 0) {
		remove(cutted_filename);
		file_c = fopen(cutted_filename, "a");
		file_temp = fopen("temp.txt", "r");

		while(fgets(buff,50, file_temp) != NULL){
			fprintf(file_c, "%s", buff);
		}
		fclose(file_c);
		fclose(file_temp);
	}

		//After all operations, we can remove temporary text file.
		remove("temp.txt");

	//If our source file (cutted_filename) is equal to filenameN1 (numbers1.txt) or filenameN2 (numbers2.txt) and flag_Controller is zero (which means there is no any number in source file)
	//then flagN1 or flagN2 is equal to zero.
	if (strcmp(cutted_filename,filenameN1) == 0 && flag_Controller == 0)
	{
		flagN1 = 0;

	}else if(strcmp(cutted_filename,filenameN2) == 0 && flag_Controller == 0){

		flagN2 = 0;
	}

}


//These function belongs to thread A.
void *threadwork_A(void *parameters){
	THREAD_PARAMETERS* param = (THREAD_PARAMETERS*)parameters;

	//These method works until all of flags are equal to zero. It means that there is no number in both of numbers1.txt and numbers2.txt .
	while(flagN1 || flagN2){

	/////////// THERE ARE THREE DIFFERENT SCENARIOS IN THESE CONDITIONS FOR THREAD A ///////////
	
	//For first scenario, thread A keeps firsty filenameN1 and then filenameR.

		if (flagN1 && pthread_mutex_trylock(&lockN1) == 0){ //It locks for filenameN1.
			
			if (pthread_mutex_trylock(&lockR) == 0){ //It locks for filenameR


				srand(time(NULL));
				int random = (rand()%10) +1; //Random number specifies how many lines is cutted and pasted.

				for (int i = 0; i < random; ++i){

					//If there is no number to cut in cutted file, then thread should not work additionally.
					if (flagN1){

						cut_and_past_number(filenameN1,filenameR);

					}else{

						break;
					}
				}

				//After operations, locks are opened.
				pthread_mutex_unlock(&lockR);
				pthread_mutex_unlock(&lockN1);
			
			}else{

				pthread_mutex_unlock(&lockN1); //After lockN1, if other mutexes are not available, then lockN1 releases.

			}
		}

		//For second scenario, thread A keeps firsty filenameN2 and then filenameR.

		if (flagN2 && pthread_mutex_trylock(&lockN2) == 0){ //It locks for filenameN2.
			
			if (pthread_mutex_trylock(&lockR) == 0){ //It locks for filenameR

				srand(time(NULL));
				int random = (rand()%10) +1; //Random number specifies how many lines is cutted and pasted.

				for (int i = 0; i < random; ++i){

					//If there is no number to cut in cutted file, then thread should not work additionally.
					if (flagN2){

						cut_and_past_number(filenameN2,filenameR);

					}else{

						break;
					}
				}

				//After operations, locks are opened.
				pthread_mutex_unlock(&lockR);
				pthread_mutex_unlock(&lockN2);
			
			}else{

				pthread_mutex_unlock(&lockN2); //After lockN2, if other mutexes are not available, then lockN2 releases.
			}
		}

		//For second scenario, thread A keeps firsty filenameR and then filenameN1 or filenameN2.

		if (pthread_mutex_trylock(&lockR) == 0){ //It locks for filenameR2.

			if (flagN1 && pthread_mutex_trylock(&lockN1) == 0){ //It locks for filenameN1.

				srand(time(NULL));
				int random = (rand()%10) +1; //Random number specifies how many lines is cutted and pasted.

				for (int i = 0; i < random; ++i){

					//If there is no number to cut in cutted file, then thread should not work additionally.
					if (flagN1){

						cut_and_past_number(filenameN1,filenameR);

					}else{

						break;
					}
				}

				//After operations, locks are opened.
				pthread_mutex_unlock(&lockN1);
				pthread_mutex_unlock(&lockR);

			}else if (flagN2 && pthread_mutex_trylock(&lockN2) == 0){  //It locks for filenameN2.
				
				srand(time(NULL));
				int random = (rand()%10) +1; //Random number specifies how many lines is cutted and pasted.

				for (int i = 0; i < random; ++i){

					//If there is no number to cut in cutted file, then thread should not work additionally.
					if (flagN2){

						cut_and_past_number(filenameN2,filenameR);

					}else{

						break;
					}
				}

				//After operations, locks are opened.
				pthread_mutex_unlock(&lockN2);
				pthread_mutex_unlock(&lockR);
				
			}else{

				pthread_mutex_unlock(&lockR); //After lockR, if other mutexes are not available, then lockR releases.

			}
		}

	}

	printf("Thread A %d finished its work!\n",param->thread_No);

	pthread_exit(0);

}

void *threadwork_B(void *parameters){

	//These method works until all of flags are equal to zero and one list time checking prime numbers in resource.txt . 
	//It means that there is no number in both of numbers1.txt and numbers2.txt and there is no "prime" number in resource.txt .
	while(flagN1 || flagN2 || flagR_prime){

		/////////// THERE ARE THREE DIFFERENT SCENARIOS IN THESE CONDITIONS FOR THREAD A ///////////

		if (flagN1 &&  pthread_mutex_trylock(&lockN1) == 0){ //It locks for filenameN1.


			delete_prime_number(filenameN1);

			pthread_mutex_unlock(&lockN1); //After operation, lock is opened.
			
		}

		if (flagN2 && pthread_mutex_trylock(&lockN2) == 0 ){ //It locks for filenameN2.

			delete_prime_number(filenameN2);

			pthread_mutex_unlock(&lockN2); //After operation, lock is opened.
		}	

		if (pthread_mutex_trylock(&lockR) == 0){ //It locks for filenameR.

			//One last time when numbers1.txt and numbers2.txt are empty, I checked "resource.txt" for any prime numbers.
			if (flagR_prime && flagN1 == 0 && flagN2 == 0)
			{

				delete_prime_number(filenameR);

				pthread_mutex_unlock(&lockR); //After operation, lock is opened.
			
			}else{


				delete_prime_number(filenameR);

				pthread_mutex_unlock(&lockR); //After operation, lock is opened.
			}

		}

	}

	printf("Thread B finished its work!\n");

	pthread_exit(0);

}

void *threadwork_C(void *parameters){

	//These method works until all of flags are equal to zero and one list time checking negative numbers in resource.txt . 
	//It means that there is no number in both of numbers1.txt and numbers2.txt and there is no "negative" number in resource.txt .
	while(flagN1 || flagN2 || flagR_negative){

		if ( flagN1 && pthread_mutex_trylock(&lockN1) == 0) //It locks for filenameN1.
		{
			delete_negative_number(filenameN1);

			pthread_mutex_unlock(&lockN1); //After operation, lock is opened.
		}

		if (flagN2 && pthread_mutex_trylock(&lockN2) == 0) //It locks for filenameN2.
		{
			delete_negative_number(filenameN2);

			pthread_mutex_unlock(&lockN2); //After operation, lock is opened.

		}	

		if (pthread_mutex_trylock(&lockR) == 0) //It locks for filenameR.
		{

			//One last time when numbers1.txt and numbers2.txt are empty, I checked "resource.txt" for any negative numbers.
			if (flagR_negative && flagN1 == 0 && flagN2 == 0)
			{
				delete_negative_number(filenameR);

				pthread_mutex_unlock(&lockR); //After operation, lock is opened.
			
			}else{

				delete_negative_number(filenameR);

				pthread_mutex_unlock(&lockR); //After operation, lock is opened.

			}

		}

	}

	printf("Thread C finished its work!\n");

	pthread_exit(0);

}

