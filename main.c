#define _BSD_SOURCE

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "limits.h"

#define BOTTOM_SIZE 100 
#define TOP_SIZE 70000000 
#define PAYLOAD_SIZE 7
#define ITERATIONS 5000000
#define ITERATION_CYCLES 30

struct Node {
	struct Node *next;
	char payload[PAYLOAD_SIZE]; 
};

void print_cpu_info(FILE* fp);
void do_iteration(FILE* fp, int is_random);
size_t get_random(size_t limit);


int main() {
	printf("Performing iterations...\n");
	
	time_t current_time;
	time(&current_time);
	char* c_time_string;
	c_time_string = ctime(&current_time);

	FILE *fp;
	fp=fopen("output.txt", "w");
	fprintf(fp, "%s", c_time_string);
	fprintf(fp, "CACHE SIZE TEST\n");

	print_cpu_info(fp);

	
	fprintf(fp, "\nSequential iteration:\n");	
	do_iteration(fp, 0);

	fprintf(fp, "\nRandom iteration:\n");	
	do_iteration(fp, 1);

	fclose(fp);

	printf("Check output.txt for results.\n");
	return 0;
}


void print_cpu_info(FILE* fp) {
	FILE *pp;
#ifdef __linux__
	pp = popen("lscpu | grep cache", "r");
#elif __APPLE__
	pp = popen("sysctl -a | grep cachesize", "r");
#else 
	return;
#endif	

	fprintf(fp, "\nObtaining cache info...\n");
	
	if (pp != NULL) {
		while (1) {
			char *line;
			char buf[1000];
			line = fgets(buf, sizeof buf, pp);
			if (line == NULL) break;			
			fprintf(fp, "%s", line);
		}
		fprintf(fp, "\n");
		pclose(pp);
	}
}


void do_iteration(FILE* fp, int is_random) {

	fprintf(fp, "Total iterations: %i\n", ITERATIONS);
	double iteration_time = 0;

	size_t node_size = sizeof(struct Node);
	
	size_t elements_in_1k = BOTTOM_SIZE / node_size;
	size_t elements_in_10M = TOP_SIZE / node_size;

	for(size_t size = elements_in_1k; size < elements_in_10M; size *= 2) {
		struct Node *buff = malloc(size * sizeof(struct Node));
		
		if (is_random) {
			for(size_t i = 0; i < size - 1; i++) {
				buff[i].next = &buff[ get_random(size) ];
			}	
		} else {
			for(size_t i = 0; i < size - 1; i++) {
				buff[i].next = &buff[i+1];
			}	
		}		
		buff[size - 1].next = &buff[0];

		clock_t start, end; 
				
		for (size_t j = 0; j < ITERATION_CYCLES; j++) {
			start = clock();
			struct Node *node = buff;

			for (size_t i = 0; i < ITERATIONS; i++) {
				node = node->next;	
			}

			end = clock();
			iteration_time += (double)(end - start) / CLOCKS_PER_SEC;	
		}

		iteration_time /= ITERATION_CYCLES;
		

		free(buff);

		fprintf(fp, "%zd", node_size*size);
		fprintf(fp, "\t%f\n", iteration_time);
	}
}


size_t get_random(size_t limit) {
	size_t divisor = RAND_MAX / (limit+1);
	size_t random_number;

	do {
		random_number = rand() / divisor;
	} while (random_number > limit);

	return random_number;
}