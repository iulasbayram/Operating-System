#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Ismail Ulas Bayram - 220201040

/* Struct for list nodes */ 
struct lst_node_s {
	int data;
	struct lst_node_s* next; 
};

/* Struct for task nodes */ 
struct tsk_node_s {
	int task_num; //starting from 0
	int task_type; // insert:0, delete:1, search:2 
	int value;
	struct tsk_node_s* next;
};

// Initializing structs
struct lst_node_s* list = NULL;
struct tsk_node_s* front = NULL;
struct tsk_node_s* rear = NULL;
struct tsk_node_s* node = NULL;

bool Search(int data){
	struct lst_node_s* temp = list; // Copy of the list
	while(temp != NULL) {
		if (temp->data == data){ // If data is found in the copy of the list
			return true;
		}
		temp = temp->next; // moving on to the next node
	}
	return false;	
}

int Insert(int data){

	bool data_check = Search(data); // If data is not found, then we can insert value
	if (data_check != true) // If data is not found
	{	
		struct lst_node_s* temp = (struct lst_node_s*)malloc(sizeof(struct lst_node_s)); // We create new node that takes value of data
		temp->data = data;
		temp->next = NULL;

		if(list == NULL){
			list = temp; // If list is null, then we assign temp node to list
			return 1;

		}else if (list->next == NULL) // If next node of list is null
		{

			list->next = temp; // then we assign next node of list to temp
			return 1;

		}else{

			struct lst_node_s* copy_list = list; // We create copy of list to find next nodes of list
			
			while(copy_list->next != NULL){ // We try to reach at the end of the node to bind list with temp node.
				copy_list = copy_list->next;
			}
			copy_list->next = temp; // After moving on to the last node, we can bind it with temp node.
			return 1;
		}
	}else{
		return 0; // If data is already inserted to the list.
	}
}


int Delete(int data){

	bool data_check = Search(data); // We need to check whether data exists in the list or not
	
	if (data_check == true){ // If data exists, then we can delete it

		struct lst_node_s* temp = list, *prev; // Creating two nodes which are temp and prev and they equal to list
		
		while (temp != NULL && temp->data == data) // If temp is not empty and the value of the first node equals to data
		{ 
		
			list = temp->next; // Then we can delete by moving list on to the next node of it.
			temp = list;
			return 1;
		} 

		while (temp != NULL) 
		{ 
			while (temp != NULL && temp->data != data) { // If temp is not null and the value of first node does not equal to data
			
				prev = temp; // Assigning temp to prev, thus the previous node of temp can be safe in prev 
				temp = temp->next; // Moving on to the next node
			}

			if (temp != NULL){ 
				prev->next = temp->next; // We bind next element of prev node with next elemnt of temp node. Thus, we can delete the target node that is between prev and temp
				temp = prev->next;
				return 1;
			} 
		}
		
		return 1;
	}else{
		return 0; // If data does not exist
	}
} 


void Task_enqueue(int task_num, int task_type, int value){
	struct tsk_node_s *temp = (struct tsk_node_s*)malloc(sizeof(struct tsk_node_s)); // We create new task
	
	temp->task_num = task_num; // And assign variables with the parameters
	temp->value = value;
	temp->task_type = task_type;
	temp->next = NULL;

	if (rear == NULL && front == NULL) // Rear node and front node assign the same location at the beginning of enqueue operation
	{
		front = temp; // If both of them are null, then we bind both of node with the temp structure
		rear = temp;

	}else{
		rear->next = temp; // Otherwise, the next node of the rear structre assigns temp node
		rear = temp;
	}
}

struct tsk_node_s *Task_dequeue(){
	struct tsk_node_s* temp = front; // First enqueued task is assigned to the temp task
	
	if (front == NULL) // If there is no task at the beginning of the queue, all tasks are dequeued or no task is enqueued
	{
		printf("There is no task in queue\n");
		return NULL;
	}else{
		
		if (front == rear) // Otherwise, If front node and rear node are equal to the each other
		{	
			front = NULL; // It means that there are only one node ın the queue and this node will be dequeued
			rear = NULL;
			return temp;

		}else{

			front = front->next; // Otherwise, the first element of the queue is deleted and front task is equal to next node of it.
			return temp;
		}
		
	}
}


void Task_queue(int n){
	node = (struct tsk_node_s*)malloc(sizeof(struct tsk_node_s)); // Node is created in this method for queue
	srand(time(NULL) + 100);

	for (int i = 0; i < n; ++i)
	{	
		// Below that, all task's variables are generated
		int task_num = i;
        int task_type = rand() % 3;
		int value = rand() % 6;
		Task_enqueue(task_num,task_type,value);
	}
	printf("Generated %d random list tasks...\n", n);
}

void SortedList()
{
	if(list != NULL){ // If list is not empty
		struct lst_node_s *temp = list; // temp is assigned to the copy of the list
		struct lst_node_s *traverse, *min; // min and traverse nodes

		while(temp->next){ // Until next of the temp node is not null
			
			min = temp; // min is equal to temp node
			traverse = temp->next; // traverse is equal to next temp node

			while(traverse){ // Until traverse is not null
				
				if( min->data > traverse->data ){ // If value of min node is grater than value of traverse node
					min = traverse; // Than switch operation is applied between these two nodes
				
				}	
				traverse = traverse->next; // Then the same operation is applied for next traverse node
			}
			int temp_data = temp->data; // After that, final result is applied to the temp
			temp->data = min->data;
			min->data = temp_data;			
			temp = temp->next;
		}
		
		printf("Final list:\n"); // After sort operation, we can print the list
		printf("  ");
		
		while(list != NULL){
		
			printf("%d ", list->data);
			list = list->next;
		}
		
		printf("\n");	
	
	}else{

		printf("Final list:\n");
		printf("  ");
		printf("List is empty!\n");
	}
	
} 

int main(int argc, char const *argv[])
{	
	if(argc == 2){ // Number of argument must be two
		
		int n = atoi(argv[1]); // We assign second argument to the n variable by converting it from string to integer
		
		Task_queue(n); // Creating task and enqueue operations
		
		for (int i = 0; i < n; ++i)
		{
			struct tsk_node_s *task = Task_dequeue(); // We dequeue all tasks in the queue one by one
			
			printf("task %d-", i);
			
			if (task->task_type == 0) // For insert operation
			{
			
				printf("insert %d: ",task->value);
				int insert_check = Insert(task->value);
				if (insert_check == 1)
				{
					printf("%d is inserted", task->value);
				}else{
					printf("%d cannot be inserted", task->value);
				}

				printf("\n");
			}
			else if (task->task_type == 1) // For delete operation
			{
			
				printf("delete %d: ",task->value);
				int delete_check = Delete(task->value);
				if (delete_check == 1)
				{
					printf("%d is deleted",task->value);
				}else{
					printf("%d cannot be deleted", task->value);
				}

				printf("\n");
			}
			else if (task->task_type == 2) // For search operation
			{
			
				printf("search %d: ",task->value);
				bool data_check = Search(task->value);
				if (data_check == true)
				{
					printf("%d is found", task->value);
				}else{
					printf("%d is not found", task->value);
				}
				printf("\n");
			}
		}
		
		SortedList(); // AFter sorting the list, final list is printed in this section
	
	}else{ // If number of argument is not equal to two
	
		printf("Please, enter the number.\n");
	}

	return 0;
}

