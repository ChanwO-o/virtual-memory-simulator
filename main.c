#include <stdio.h>

#define MMSIZE 4
#define DISKSIZE 8
#define PTSIZE 8
#define DATASIZE 4

struct pt_entry
{
    int entry;
    int valid;
    int dirty;
    int pgnum;
};

struct page
{
    int data[4];
    int originaddr;
};

struct page mainmem[MMSIZE];
struct page diskmem[DISKSIZE];
struct pt_entry ptable[PTSIZE];

int addrtrans(int addr)
{
    return addr / 4;
}

int addrtransoff(int addr)
{
    return addr % 4;
}

//accesses the address and offset in ptable
//and does relevant calculations.
void accessptable(int addr, int offset)
{
    struct pt_entry entry = ptable[addr];
 
    //if not valid then access disk
    if (entry.valid == 0)
    {
        //access disk due to page fault
    
    }
    //if valid access main memory
    else
    {
        //access main memory
    }
}

//copy index address in disk into main memory
void copypage(int addr)
{
    struct page pg = diskmem[addr];
    pg.originaddr = addr;


}

void parsecmd(char * buf)
{
    char * cmd;

    cmd = strtok(buf, " \n");

    if (strcmp(cmd, "read") == 0)
    {
        printf("Inside read\n");
        strtok(NULL, " ");
    }
    else if (strcmp(cmd, "write") == 0)
    {
        printf("Write\n");
        cmd = strtok(NULL, " ");
    }
    else if (strcmp(cmd, "showmain") == 0)
    {
        printf("Showmain\n");
        cmd = strtok(NULL, " ");
        if (cmd != NULL)
        {
            int ppn = atoi(cmd);
            if (ppn >= 0 && ppn <= MMSIZE)
                printmainmem(ppn);
        }

    }
    else if (strcmp(cmd, "showdisk") == 0)
    {
        printf("Showdisk\n");
        cmd = strtok(NULL, " ");
        if (cmd != NULL)
        {
            int dpn = atoi(cmd);
            if (dpn >= 0 && dpn <= DISKSIZE)
                printdiskmem(dpn);
        }
    }
    else if (strcmp(cmd, "showptable") == 0)
    {
        printf("Showptable\n");
        cmd = strtok(NULL, " ");
        if (cmd == NULL)
        {
            printptable();   
        }
    }
}

void initializemainmem()
{
	int i;
	for (i = 0; i < MMSIZE; ++i) {
		int j;
		for (j = 0; j < DATASIZE; ++j)
			mainmem[i].data[j] = -1;
	}
}

void initializediskmem()
{
	int i;
	for (i = 0; i < DISKSIZE; ++i) {
		int j;
		for (j = 0; j < DATASIZE; ++j)
			diskmem[i].data[j] = -1;
	}
}

void initializeptable()
{
	int i;
	for (i = 0; i < PTSIZE; ++i)
	{
		ptable[i].entry = i;
		ptable[i].valid = 0;
		ptable[i].dirty = 0;
		ptable[i].pgnum = -1;
	}
}

void printmainmem(int pagenum)
{
	printf("printmainmem()\n");
    
    if (pagenum < 0 || pagenum >= MMSIZE)
        return;

	int i;
	for (i = 0; i < DATASIZE; ++i) {
		printf("%d:%d\n", pagenum * DATASIZE + i, mainmem->data[pagenum]);
	}
}

void printdiskmem(int pagenum)
{
	printf("printdiskmem()\n");

    if (pagenum < 0 || pagenum >= DISKSIZE)
        return;
    
    int i;
	for (i = 0; i < DATASIZE; ++i) {
		printf("%d:%d\n", pagenum * DATASIZE + i, diskmem->data[pagenum]);
	}
}

void printptable()
{
	printf("printptable()\n");
	int i;
	for (i = 0; i < PTSIZE; ++i) {
		printf("%d:%d:%d:%d\n", ptable[i].entry, ptable[i].valid, ptable[i].dirty, ptable[i].pgnum);
	}
}


int main()
{
    char buf[80];
	
	initializemainmem();
	initializediskmem();
	initializeptable();
	
    /*
	printmainmem(0);
	printdiskmem(0);
	printptable();
    */

    while (1)
    {
        printf("> ");

        if (fgets(buf, sizeof(buf), stdin) != NULL)
        { 
            if (strcmp(buf, "quit\n") == 0)
                break;

            parsecmd(buf);
     
        }
    }

    return 0;
}
