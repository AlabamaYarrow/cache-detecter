#define _BSD_SOURCE

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "limits.h"

#define BOTTOM_SIZE 100 
#define TOP_SIZE 70000000 
#define PAYLOAD_SIZE 7
#define ITERATIONS 5000000

void print_cpu_info(FILE* fp);
void do_sequential_iteration(FILE* fp);
void do_random_iteration(FILE* fp);

struct Node {
	struct Node *next;
	char payload[PAYLOAD_SIZE]; 
};


int main() {
	printf("Cache size testing utility.\n");
	printf("Calculating cache size...\n");
	
	time_t current_time;
	time(&current_time);
	char* c_time_string;
	c_time_string = ctime(&current_time);

	FILE *fp;
	fp=fopen("output.txt", "w");
	fprintf(fp, "%s", c_time_string);
	fprintf(fp, "CACHE SIZE TEST\n");

	print_cpu_info(fp);

	
	fprintf(fp, "Sequential iteration time:\n");
	fprintf(fp, "Bytes-tics\n");

	do_sequential_iteration(fp);
	// do_random_iteration(fp);

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


void do_sequential_iteration(FILE* fp) {

	int iteration_time = 0;

	size_t node_size = sizeof(struct Node);
	
	size_t elements_in_1k = BOTTOM_SIZE / node_size;
	size_t elements_in_10M = TOP_SIZE / node_size;

	for(size_t size = elements_in_1k; size < elements_in_10M; size *= 2) {
		struct Node *buff = malloc(size * sizeof(struct Node));
		
		for(size_t i = 0; i < size - 1; i++) {
			buff[i].next = &buff[i+1];
		}

		buff[size - 1].next = &buff[0];
		clock_t start, end; 
		start = clock();
		struct Node *node = buff;
		for (size_t i = 0; i < ITERATIONS; i++) {
			node = node->next;	
		}
		free(buff);

		end = clock();
		iteration_time = (end - start);

		fprintf(fp, "%zd", node_size*size);
		fprintf(fp, "\t%d\n", iteration_time);
	}
}


void do_random_iteration(FILE* fp) {

	int iteration_time = 0;

	size_t node_size = sizeof(struct Node);
	
	size_t elements_in_1k = BOTTOM_SIZE / node_size;
	size_t elements_in_10M = TOP_SIZE / node_size;

	for(size_t size = elements_in_1k; size < elements_in_10M; size *= 2) {
		struct Node *buff = malloc(size * sizeof(struct Node));
		
		for(size_t i = 0; i < size - 1; i++) {
			buff[i].next = &buff[i+1];
		}

		buff[size - 1].next = &buff[0];
		clock_t start, end; 
		start = clock();
		struct Node *node = buff;
		for (size_t i = 0; i < ITERATIONS; i++) {
			node = node->next;	
		}
		free(buff);

		end = clock();
		iteration_time = (end - start);

		fprintf(fp, "%zd", node_size*size);
		fprintf(fp, "\t%d\n", iteration_time);
	}
}

