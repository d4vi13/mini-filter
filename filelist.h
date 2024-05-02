#include "fileinfo.h"

struct Node{
	PFILE_INFO info;
	LIST_ENTRY ListEntry;
};

typedef struct Node NODE, * PNODE;

