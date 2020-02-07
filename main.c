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
    int counter;
};

struct page
{
    int data[4];
    int originaddr;
};

struct page mainmem[MMSIZE];
struct page diskmem[DISKSIZE];
struct pt_entry ptable[PTSIZE];

int isLRU = 1;
int memindex = -1;
int globalcounter = 0;

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
    //printf("%d %d\n", addr, offset);
    //printptable();

    struct pt_entry entry = ptable[addr];
 
    //if not valid then access disk
    if (entry.valid == 0)
    {
    	printf("Page Fault Has Occurred\n");
        ++globalcounter;
        //access disk due to page fault
        copypage(addr);

    }
    //if valid access main memory
    else
    {
        //access main memory
    }
}

int mainmemisfull()
{
	int i;
	int validcount = 0;
	for (i = 0; i < PTSIZE; ++i) {
		if (ptable[i].valid == 1)
			validcount++;
	}
	if (validcount >= 4)
		return 1;
	return 0;
}

int getnextindex()
{
	++memindex;
	if (memindex >= 4)
	{
		memindex = 0;
	}
	return memindex;
}

int getfifoindex()
{
	int lowestcounter;
	int lowestindex;
	int checkedlowest = 0;
	int i;
	for (i = 0; i < PTSIZE; i++)
	{
		if (ptable[i].valid == 1)
		{
			if (checkedlowest == 0)
			{
				checkedlowest = 1;
				lowestcounter = ptable[i].counter;
				lowestindex = i;
			}
			else
			{
				if (lowestcounter > ptable[i].counter)
				{
					lowestcounter = ptable[i].counter;
					lowestindex = i;
				}
			}
		}
	}
	return lowestindex;
}

int getmainmemindex()
{
	if (mainmemisfull() == 0) {
		return getnextindex();
	}
	else
	{
		printf("Main memory is full\n");
		return getfifoindex();
	}
}

//copy index address in disk into main memory
void copypage(int addr)
{
	printf("INDSIDE COPYPAGE\n");

    int targetindex = getmainmemindex();
    printf("TINDEX: %d\n", targetindex);

    //remove page table from main memory
    ptable[targetindex].valid = 0;

    //add the new page table to main memory
    ptable[addr].valid = 1;
    ptable[addr].pgnum = targetindex;
    ptable[addr].dirty = 0;
    ptable[addr].counter = globalcounter;

    int i;
    for (i = 0; i < DATASIZE; i++)
    {
    	mainmem[targetindex].data[i] = diskmem[addr].data[i];
    }

    printptable();

    // int i;
    // for (i = 0; i < DATASIZE; i++)
    // {
    //     mainmem[targetindex].data;
    // }

    // diskmem[addr].data
    
    // struct page pg = diskmem[addr];
    // pg.originaddr = addr;
}

void parsecmd(char * buf)
{
    char * cmd;

    cmd = strtok(buf, " \n");

    if (strcmp(cmd, "read") == 0)
    {
        printf("Inside read\n");
        cmd = strtok(NULL, " ");
        if (cmd != NULL)
        {
            int ppn = atoi(cmd);
            printf("%d\n", ppn);
            if (ppn >= 0 && ppn <= 31)
            {
                accessptable(addrtrans(ppn), addrtransoff(ppn));
            }
        }
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
        ptable[i].counter = 0;
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
		printf("%d:%d:%d:%d:%d\n", 
			ptable[i].entry, 
			ptable[i].valid, 
			ptable[i].dirty, 
			ptable[i].pgnum,
			ptable[i].counter);
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
