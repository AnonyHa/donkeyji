#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	unsigned int f1:1;
	unsigned int f2:1;
	unsigned int f3:4;
}FF;

typedef int (*init_conf)(int);

typedef struct
{

}bucket;

typedef struct
{
	int max_size;
	bucket* b; 
}hash_tbl;

int main()
{
	FF ff;
	return 0;
}
