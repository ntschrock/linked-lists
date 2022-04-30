#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "leak_detector_c.h"

void remove_crlf(char *s)
{
    char *t = s + strlen(s);

    t--;

    while ((t >= s) && (*t == '\n' || *t == '\r'))
    {
        *t = '\0'; 
        t--;
    }
}

void get_next_nonblank_line(FILE *ifp, char *s, int max_length)
{
    s[0] = '\0';

    while (s[0] == '\0')
    {
        fgets(s, max_length, ifp);
        remove_crlf(s);
    }
}

void print_space(FILE *ofp)
{
    fprintf(ofp, "\n");
}

/*--------------------typedefs of the structures--------------------*/
typedef struct failfish_struct //fishy structure
{
    int sequence_num;
    struct failfish_struct *next;
    struct failfish_struct *previous;
}failfish;

typedef struct queue_struct //structure for the queue
{
	char *pondname;
    int nfailfish; //n
    int eat_count; //e
    int min_threshold; //th
    failfish *head;
    failfish *tail;
}failfish_queue;

typedef struct pond_struct //structure for the pond 
{
    int pond_number; //g
    char *name;
    failfish_queue *failfish_queue;
}pond;

/*--------------------initial setup--------------------*/
int get_number_of_ponds(FILE *ifp) //gets the number of ponds from the first line of input 
{
    char num_line[32];
    int number_of_ponds;

    get_next_nonblank_line(ifp, num_line, 31);
	number_of_ponds = atoi(num_line);
	
    return number_of_ponds;
}

failfish *create_failfish(int sequence_number) //creates a new fishy
{
    failfish *f;

    f = malloc(sizeof(failfish));
    f-> sequence_num = sequence_number;
    f-> next = NULL;
    f-> previous = NULL;

    return f;
}

failfish_queue *create_failfish_queue(char *pondname, int n, int e, int th) //makes a new queue
{
    failfish_queue *q;

    q = malloc(sizeof(failfish_queue));
	q-> pondname = strdup(pondname);
    q-> nfailfish = n;
    q-> eat_count = e;
    q-> min_threshold = th;
    q-> head = NULL;
    q-> tail = NULL;

    return q;
}

pond *create_pond_array(int number_of_ponds) //makes a new pond array
{
    pond *pa = calloc(number_of_ponds, sizeof(pond));

    return pa;
}

void *create_pond(void) //makes a new pond 
{
    pond *p;

    p = malloc(sizeof(pond));
    p-> pond_number = 0;
    p-> name = NULL;
    p-> failfish_queue = NULL;

    return p;
}

/*--------------------functions that destroy--------------------*/
void destroy_failfish(failfish *f) //destroys the fishy
{
    free(f);
}

void destroy_queue(failfish_queue *q) //destroys the queue
{
    free(q);
}

void destroy_pond_array(pond *pa) //destroys the pond array this probably isnt necessary because calloc
{
    free(pa);
}

void destroy_pond(pond *p) //destroys the pond 
{
    free(p);
}

/*--------------------functions for pre course set-up--------------------*/
pond *fill_pond(FILE *ifp, pond *pa, int number_of_ponds) //fills the pond with all the fish and info
{
    char line[128];
    int pond_num = 0;
    char pondname[128];
    int nfailfish = 0;
    int eat_count = 0;
    int min_threshold = 0;

    get_next_nonblank_line(ifp, line, 127); //grabs the fishes information from input 
    sscanf(line, "%d %s %d %d %d", &pond_num, pondname, &nfailfish, &eat_count, &min_threshold);

    pa-> pond_number = pond_num; //assigns the pond number
    pa-> name = strdup(pondname); //assigns the pond name

    pa-> failfish_queue = create_failfish_queue(pondname, nfailfish, eat_count, min_threshold); //assigns the values that it takes from input to the values in the queue

    return pa;
}

void make_a_copy(pond *current, pond *copy) //makes a copy of the current pond 
{
    copy-> pond_number = current-> pond_number; //copy gets same number as current
    copy-> name = current-> name; //copy gets same name as current
	copy-> failfish_queue = current-> failfish_queue; //copy gets same queue as current
}

void sort_ponds(pond *pa, int number_of_ponds) //takes  all the ponds and sorts them by pond number
{
	pond *place_holder = create_pond(); //needed so that we dont lose a pond when sorting 
    int x;
    int y;

    for (x = 0; x < number_of_ponds; x++)
    {
        for (y = (x + 1); y < number_of_ponds; y++)
        {
            if ((pa + x)-> pond_number > ((pa + y)-> pond_number)) //swaps two ponds 
            {
                make_a_copy((pa + x), place_holder); //puts the bigger pond in place holder
                pa[x] = pa[y]; //makes the original bigger pond now the smaller pond 
                make_a_copy(place_holder, (pa + y)); //makes the original smaller pond the bigger pond 
            }
        }
    }
    destroy_pond(place_holder); //gets rid of the place holder since were done with it 
}

failfish *peek(failfish_queue *q) //takes a look at then next header and returns it 
{
    return q-> head;
}

int is_empty(failfish_queue *q) //checks to see if the peek was empty
{
    if (peek(q) == NULL) //if it is empty return 0
    {
        return 0;
    }
    else //if there was something in it return 1
    {
        return 1;
    }
}

void enqueue(failfish_queue *q, failfish *new_failfish) //add a fish to the tail of the list 
{
    if (is_empty(q) == 0) //if it is empty then we just make the head and tail both equal to the new fish
    {
        q->head = new_failfish;
        q->tail = new_failfish;        
        new_failfish-> next = new_failfish;
		new_failfish-> previous = new_failfish;
    }
    else //if it is not empty we add the fish to the end of the list 
    {        
        new_failfish-> next = q-> head; 
		new_failfish-> previous = q-> tail;       
        q-> tail-> next = new_failfish;
 		q-> head-> previous = new_failfish;
        q-> tail = new_failfish;
    }
}

failfish *dequeue(failfish_queue *q) //returns the head of the queue
{
    failfish *f = q-> head;

	//this if statement has to be at the top or dequeue does not work and it screws everything up 
    if (q-> head-> sequence_num == q-> tail-> sequence_num) //if there is only one item sets the queue to NULL
    {
        q-> head = NULL;
        q-> tail = NULL;
    }
	if (is_empty(q) == 0) //if the head is NULL it empties the queue  
    {
		q-> head = NULL;
        q-> tail = NULL;
    }
    else //if the head is not NULL then it goes to the next fish 
    {
        q-> head = q-> head-> next;
        q-> head-> previous = f-> previous;
        f-> previous-> next = q-> head;
    }   

    return f;
}

int get_failfish_total(failfish_queue *q) //goes through the queue and every time it finds a head increases count
{
    int total_failfish = 0; //has to equal 0 or program dies
    failfish *current = q-> head;

	do //sets the current equal to next and adds one to the total
    {
        current = current-> next;
        total_failfish = total_failfish + 1;
    } while (current-> sequence_num != q-> head-> sequence_num);

    return total_failfish;
}

/*--------------------FIRST COURSE--------------------*/
void print_extended_input(FILE *ofp, pond *pa, int number_of_ponds) //prints out the input with individual numbers for the fish 
{
    int x;
    int y;

    fprintf(ofp, "Initial Pond Status\n"); //header
    for (x = 0; x < number_of_ponds; x++) //prints all of the failfish after the ponds have been sorted 
    {
        fprintf(ofp, "%d %s ", (pa + x)-> pond_number, (pa + x)-> name);
        for (y = 0; y < (pa + x)-> failfish_queue-> nfailfish; y++)
        {
            fprintf(ofp, "%d ", y + 1);
        }
        print_space(ofp);
    }
    fprintf(ofp, "\nFirst Course\n\n"); //header for the first course, was a pain in the ass trying to figure out where this goes 
}

void first_course(FILE *ofp, pond *pa) //first course 
{
	fprintf(ofp, "Pond %d: %s\n", pa-> pond_number, pa-> name); //output
    failfish *f = pa-> failfish_queue-> head; //sets the fish f equal to the head of the queue
    failfish *next_f = pa-> failfish_queue-> head-> next; //sets a filler fish equal to the next in the queue
	
	int appetizer = pa-> failfish_queue-> eat_count; //fish that is going to get eaten
    int minimum_thresh = pa-> failfish_queue-> min_threshold; //the threshold of the pond
    int total_failfish = get_failfish_total(pa-> failfish_queue); //total fish sitting in queue
	int x;
    
    while (total_failfish > minimum_thresh) //reapeats the eating process until we reach threshold
    {
        for (x = 1; x < appetizer; x++) //goes to the next fishy
        {
            f = f-> next;
		}
        f-> next-> previous = f-> previous; //anti-break-everything measures
        f-> previous-> next = f-> next;

        if (f-> sequence_num == pa-> failfish_queue-> head-> sequence_num) //move head forward if fishy is the head
        {
            pa-> failfish_queue-> head = f-> next;
            pa-> failfish_queue-> tail-> next = pa-> failfish_queue-> head;
        }
        if (f-> sequence_num == pa-> failfish_queue-> tail-> sequence_num) //move tail backwards if fishy is the tail 
        {
            pa-> failfish_queue-> tail = f-> previous;
            f-> previous-> next = pa-> failfish_queue-> head;
        }

        fprintf(ofp, "Failfish %d eaten\n", f-> sequence_num); //outputs the fish that was eaten and moves to a holder 
        next_f = f-> next; 
        destroy_failfish(f);
        total_failfish = get_failfish_total(pa-> failfish_queue); //gets the new total of fish in queue
        f = next_f;
    }
    print_space(ofp);
}

void print_course_one(FILE *ofp, pond *pa, int number_of_ponds) //outputs the results of the first course 
{
    failfish *fishy;
    int x;

    fprintf(ofp, "End of Course Pond Status\n"); //header
    for (x = 0; x < number_of_ponds; x++) //outputs the pond number and name and what fish are left 
    {
        fishy = (pa + x)-> failfish_queue-> head;
        fprintf(ofp, "%d %s ", (pa + x)-> pond_number, (pa + x)-> name);
        do
        {
            fprintf(ofp, "%d ", fishy-> sequence_num);
            fishy = fishy-> next;
        }while (fishy-> sequence_num != (pa + x)-> failfish_queue-> head-> sequence_num);
        print_space(ofp);
    }
    print_space(ofp);
}

/*--------------------SECOND COURSE--------------------*/
void second_course(FILE *ofp, pond *pa, int number_of_ponds)
{
	//if total_failfish and alpha are not set to 0 it screws the whole output up
    failfish *f;
    int pond_number;
    int total_failfish = 0;
	int alpha = 0;
	int positive = 1;
    int x;
    int y;
    int z;

    fprintf(ofp, "Second Course\n"); //header
    for (x = 0; x < number_of_ponds; x++) //gets the amount of fish in each pond queue
    {
        total_failfish += get_failfish_total((pa + x)-> failfish_queue);
    }
    print_space(ofp);

    for (y = 0; y < total_failfish - positive; y++) 
    {
        for (z = 0; z < number_of_ponds; z++)
        {
            if (is_empty((pa + z)-> failfish_queue) == positive) //assigns items in queue with highest sequence number to trmporary holders
            {
                if ((pa + z)-> failfish_queue-> head-> sequence_num > alpha)
                {
                    alpha = (pa + z)-> failfish_queue-> head-> sequence_num;
                    pond_number = (pa + z)->pond_number;
                }
            }
        }
        fprintf(ofp, "Eaten: Failfish %d from pond %d\n", alpha, pond_number); //output

        for (x = 0; x < number_of_ponds; x++) //disposes of the failfish 
        {
            if (pond_number == (pa + x)-> pond_number)
            {
                f = dequeue((pa + x)-> failfish_queue);
                destroy_failfish(f);
            }
        }
        alpha = 0; //value rests for next loop
        pond_number = 0;
    }
}

void print_course_two(FILE *ofp, pond *pa, int number_of_ponds) //ouput 
{
    int x;

    for (x = 0; x < number_of_ponds; x++) //outputs the last living fish big chungo
    {
        if(is_empty((pa + x)-> failfish_queue) == 1)
        {
            fprintf(ofp, "\nFailfish %d from pond %d remains\n", (pa + x)-> failfish_queue-> head-> sequence_num, (pa + x)-> pond_number);
        }
    }
}

/*--------------------queue printer--------------------*/
void print_failfish_queue(FILE *ofp, failfish_queue *q) //required in implementaion requirements but not needed in output
{
    failfish *f = q-> head-> next;

    fprintf(ofp, "These are our fish: \n");
    while (f-> sequence_num != q-> head-> sequence_num)
    {
        fprintf(ofp, "fish %d has link to tail %d and head %d\n", q-> head-> sequence_num, q-> head-> previous-> sequence_num, q-> head-> next-> sequence_num);

        f = f-> next;
    }
    print_space(ofp);
}

/*--------------------MAIN FUNCTION--------------------*/
int main(void)
{
    atexit(report_mem_leak); //Memory leak detector

	//initial main function junk
    FILE *ifp;
    FILE *ofp;
    ifp = fopen("cop3502-as2-input-2.txt", "r");
    ofp = fopen("cop3502-as2-output-Schrock-Noah.txt", "w+");
    int number_of_ponds;
    int x;
    int y;

	//initial set up for the ponds 
    failfish *f;
    number_of_ponds = get_number_of_ponds(ifp);
    pond *pa = create_pond_array(number_of_ponds);

	//fills the pond based on the input 
    for (x = 0; x < number_of_ponds; x++)
    {
        fill_pond(ifp, (pa + x), number_of_ponds);
    }
	sort_ponds(pa, number_of_ponds);

	//makes the linked list for the ponds 
    for (x = 0; x < number_of_ponds; x++)
    {
        for (y = 1; y <= ((pa + x)-> failfish_queue-> nfailfish); y++)
        {
            f = create_failfish(y);
            enqueue(((pa + x)-> failfish_queue), f);
        }
    }

	//prints out the begining pond status 
    print_extended_input(ofp, pa, number_of_ponds);

	//runs the first course in each pond for the number of ponds determined in input and prints them
    for (x = 0; x < number_of_ponds; x++)
    {
        first_course(ofp, (pa + x));
    }
    print_course_one(ofp, pa, number_of_ponds);

	//runs and prints the second course
    second_course(ofp, pa, number_of_ponds);
    print_course_two(ofp, pa, number_of_ponds);

	//closing functions 
    fclose(ofp);
    fclose(ifp);
    for (x = 0; x < number_of_ponds; x++)
    {
        destroy_queue((pa + x)-> failfish_queue);
    }
    destroy_pond(pa);
    return 0;
}
// enter the leak detector