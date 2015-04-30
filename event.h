/*
 * FILE: event.h
 * NAME: Chuhan Feng
 * DATE: Apr 28, 2015
 * CLASS: CSE2421, T/TH 4:10PM
 */



/**********
 * Macros *
 **********/

#define MAX_TITLE_LEN 50



/*************************
 * Structure Definitions *
 *************************/

// Structure for date
typedef struct {
	short year;
	short month;
	short day;
} Date;

// Structure for event
typedef struct {
	long unsigned id;
	char* title;
	Date date;
} Event;



/****************************
 * Public Method Prototypes *
 ****************************/

// Create event with title and date
void create_event(Event*, char*, short, short, short);

// Save event into text database
int save_event_text(Event*);

// Save event into binary database
int save_event_bin(Event*);

// Print out all events to screen in order of ascending date
void list_event(void);

// Fetch event by unique id
int fetch_event_by_id(long unsigned, Event*);

// Print event by unique id
void print_event_by_id(long unsigned);

// Update event by unique id
int update_event_by_id(long unsigned, Event*);

// Delete event by unique id
int delete_event_by_id(long unsigned);