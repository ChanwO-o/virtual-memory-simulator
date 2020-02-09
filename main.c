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

int isLRU = 0;
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
void accessptable(int addr, int offset, int isWrite, int writeval)
{
    struct pt_entry entry = ptable[addr];
	
    //if not valid then access disk
    if (entry.valid == 0)
    {
    	printf("Page Fault Has Occurred\n");
        ++globalcounter;
        //copy page from virtual memory to main memory
        copypage(addr, offset, isWrite, writeval);
    }
    //if valid access main memory
    else
    {
		//increment counter if using LRU
		if (isLRU) {
			++globalcounter;
			ptable[addr].counter = globalcounter;
		}
		
        //access main memory
        printf("Page table already in memory\n");
        int targetindex = entry.pgnum;
		if (isWrite) // write data into mainmem
        {
        	ptable[addr].dirty = 1;
        	mainmem[targetindex].data[offset] = writeval;
        }
        else // just read value
        {
        	printf("%d\n", mainmem[targetindex].data[offset]);
        }
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
	int lowestcounter, lowestindex, i;
	int checkedlowest = 0;
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

int getlruindex()
{
	int lowestcounter, lowestindex, i;
	int checkedlowest = 0;
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

int getmainmemindex(int isWrite)
{
	if (mainmemisfull() == 0) {
		return getnextindex();
	}
	else
	{
		int ptindex;
		if (isLRU)
			ptindex = getlruindex();
		else
			ptindex = getfifoindex();
		ptable[ptindex].valid = 0;
		int targetindex = ptable[ptindex].pgnum;

		// if (isWrite)
		// {
			if (ptable[ptindex].dirty)
			{	
				int i;
				for (i = 0; i < DATASIZE; i++)
				{
					printf("writing to disk index %d the value %d\n", ptindex, mainmem[targetindex].data[i]);
					diskmem[ptindex].data[i] = mainmem[targetindex].data[i];
				}
			}
		// }
		return targetindex;
	}
}

//copy index address in disk into main memory
void copypage(int addr, int offset, int isWrite, int writeval)
{
    int targetindex = getmainmemindex(isWrite);
    printf("TINDEX: %d\n", targetindex);

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

    if (isWrite)
    {
    	printf("writing to mainmemory index %d the value %d\n", targetindex, writeval);
    	ptable[addr].dirty = 1;
    	mainmem[targetindex].data[offset] = writeval;
    }
    else
    {
    	printf("%d\n", mainmem[targetindex].data[offset]);
    }
    printptable();
}

void parsecmd(char * buf)
{
    char * cmd;
    cmd = strtok(buf, " \n");

    if (strcmp(cmd, "read") == 0)
    {
        cmd = strtok(NULL, " ");
        if (cmd != NULL)
        {
            int ppn = atoi(cmd);
            printf("%d\n", ppn);
            if (ppn >= 0 && ppn <= 31)
            {
                accessptable(addrtrans(ppn), addrtransoff(ppn), 0, NULL);
            }
        }
    }
    else if (strcmp(cmd, "write") == 0)
    {
        cmd = strtok(NULL, " ");
        if (cmd != NULL)
        {
        	int ppn = atoi(cmd);
        	printf("%d\n", ppn);
        	if (ppn >= 0 && ppn <= 31)
            {
            	cmd = strtok(NULL, " ");
            	int writeval = atoi(cmd);
            	printf("WRITEVAL: %d\n", writeval);
                accessptable(addrtrans(ppn), addrtransoff(ppn), 1, writeval);
            }
        }
    }
    else if (strcmp(cmd, "showmain") == 0)
    {
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
		ptable[i].pgnum = i;
        ptable[i].counter = 0;
	}
}

void printmainmem(int pagenum)
{
    if (pagenum < 0 || pagenum >= MMSIZE)
        return;
	int i;
	for (i = 0; i < DATASIZE; ++i)
		printf("%d:%d\n", pagenum * DATASIZE + i, mainmem[pagenum].data[i]);
}

void printdiskmem(int pagenum)
{
    if (pagenum < 0 || pagenum >= DISKSIZE)
        return;
    int i;
	for (i = 0; i < DATASIZE; ++i)
		printf("%d:%d\n", pagenum * DATASIZE + i, diskmem[pagenum].data[i]);
}

void printptable()
{
	int i;
	for (i = 0; i < PTSIZE; ++i)
		printf("%d:%d:%d:%d:%d\n", 
			ptable[i].entry, 
			ptable[i].valid, 
			ptable[i].dirty, 
			ptable[i].pgnum,
			ptable[i].counter);
}

int main(int argc, char* argv[])
{
	if (argc == 2 && strcmp(argv[1], "LRU") == 0)
		isLRU = 1;
	
    char buf[80];
	
	initializemainmem();
	initializediskmem();
	initializeptable();

    while (1)
    {
        printf("> ");

        if (fgets(buf, sizeof(buf), stdin) != NULL)
        { 
            if (strcmp(buf, "quit\n") == 0)
                break;
			else if (strcmp(buf, "\n") == 0)
				continue;
            parsecmd(buf);    
        }
    }
    return 0;
}
