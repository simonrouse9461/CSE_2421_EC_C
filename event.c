/*
 * FILE: event.c
 * NAME: Chuhan Feng
 * DATE: Apr 28, 2015
 * CLASS: CSE2421, T/TH 4:10PM
 */

#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include "event.h"
#define bool uint8_t
#define true 1
#define false 0
#define failure(x) x
#define success 0



/**********
 * Macros *
 **********/

#define DB DB_BIN



/*************
 * Constants *
 *************/

static const char DB_TXT[] = "database.txt";
static const char DB_BIN[] = "database.bin";



/*****************************
 * Private Method Prototypes *
 *****************************/

// Create a unique id
long unsigned generate_id(void);

// Move current location of file stream in front of a event with given id.
void locate_event(FILE*, long unsigned);

// Print out event to screen
void print_event(Event*);

// Read event start from current position of database
int read_event(FILE*, Event*);

// Write event start from current position of database
void write_event(FILE*, Event*);

// Destroy event start from current position of database
void destroy_event(FILE*);

// Prompt for confirmation
int confirm(char*, char*, char*, char*);



/******************************
 * Private Method Definitions *
 ******************************/

long unsigned generate_id(void) {
	long unsigned id;
	
	srand((unsigned)time(NULL)^(unsigned)getpid());
	long unsigned key1 = rand();
	srand((unsigned)time(NULL)+(unsigned)getpid());
	unsigned key2 = rand();
	id = (key1 << 0x20) + key2;
	
	return id;
}

void print_event(Event* e) {
	char t[64];
	long int epoch;
	long int current;
	long int interval;
	struct tm tm;
	memset(&tm, 0, sizeof(struct tm));
	sprintf(t, "%04hd-%02hd-%02hd 00:00:00", e->date.year, e->date.month, e->date.day);
	strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
	strftime(t, sizeof(t), "%b %d, %Y", &tm);
	epoch = (long int)mktime(&tm);
	current =(long int)time(NULL);
	interval = (epoch - current) / (3600 * 24);
	
	printf("Event id: %016lx\n", e->id);
	printf("Event title: %s\n", e->title);
	printf("Event date: %s\n", t);
	printf("Days remaining: %ld\n", interval);
}

int read_event(FILE* f, Event* e) {
	if (feof(f)) {
		return failure(1);
	}
	
	int l1 = fread(&(e->id), sizeof(long unsigned), 1, f);
	int l2 = fread(&(e->date), sizeof(short), 3, f);
	e->title = calloc(sizeof(char), MAX_TITLE_LEN);
	int l3 = fread(e->title, sizeof(char), MAX_TITLE_LEN, f);
	
	int test_integrity = l1 != 1 || l2 != 3 || l3 != MAX_TITLE_LEN;
	if (test_integrity) {
		return failure(2);
	}
	
	return success;
}

void write_event(FILE* f, Event* e) {
	fwrite(&(e->id), sizeof(long unsigned), 1, f);
	fwrite(&(e->date), sizeof(short), 3, f);
	fwrite(e->title, sizeof(char), MAX_TITLE_LEN, f);
}

void destroy_event(FILE* f) {
	int buffer_size = sizeof(long unsigned) + sizeof(short) * 3 + MAX_TITLE_LEN;
	char buffer[buffer_size];
	FILE* tmp = tmpfile();
	int pos = ftell(f);
	int l;
	
	
	fread(buffer, sizeof(char), buffer_size, f);
	while (!feof(f)) {
		l = fread(buffer, sizeof(char), buffer_size, f);
		if (l == buffer_size) {
			fwrite(buffer, sizeof(char), buffer_size, tmp);
		}
	}
	
	rewind(tmp);
	fseek(f, pos, SEEK_SET);
	
	while (!feof(tmp)) {
		l = fread(buffer, sizeof(char), buffer_size, tmp);
		if (l == buffer_size) {
			fwrite(buffer, sizeof(char), buffer_size, f);
		}
	}
	
	pos = ftell(f);
	ftruncate(fileno(f), (off_t)pos);
	
	fclose(tmp);
}

int confirm(char* prompt, char* proceed, char* abort, char* retry) {
	char input;
	printf("%s (y/n)\n", prompt);
prompt:
	input = getchar();
	if (input == 'y') {
		printf("%s\n", proceed);
		return true;
	} else if (input == 'n') {
		printf("%s\n", abort);
		return false;
	} else {
		if (input != '\n') {
			printf("Unrecognized input! %s (y/n)", retry);
		}
		goto prompt;
	}
}



/*****************************
 * Public Method Definitions *
 *****************************/

void create_event(Event* e, char* t, short year, short month, short day) {
	Date d = {year, month, day};
	
	e->id = generate_id();
	e->date = d;
	e->title = t;
}

int save_event_text(Event* e) {
	FILE* f = fopen(DB_TXT, "a");
	
	if (f == NULL) {
		return failure(1);
	}
	fprintf(f, "%016lx%04x%01x%02x%s\n", e->id, e->date.year, e->date.month, e->date.day, e->title);
	printf("Event %016lx saved!\n", e->id);
	
	fclose(f);
	
	return success;
}

int save_event_bin(Event* e) {
	FILE* f = fopen(DB_BIN, "ab");
	
	if (f == NULL) {
		return failure(1);
	}
	fwrite(&(e->id), sizeof(long unsigned), 1, f);
	fwrite(&(e->date), sizeof(short), 3, f);
	fwrite(e->title, sizeof(char), MAX_TITLE_LEN, f);
	printf("Event %016lx saved!\n", e->id);
	
	fclose(f);
	
	return success;
}

void locate_event(FILE* f, long unsigned id) {
	long unsigned i;
	int buffer_size = MAX_TITLE_LEN + sizeof(short) * 3;
	char buffer[buffer_size];
	
	while (!feof(f)) {
		fread(&(i), sizeof(long unsigned), 1, f);
		if (i == id) {
			fseek(f, -sizeof(long unsigned), SEEK_CUR);
			break;
		}
		fread(buffer, sizeof(char), buffer_size, f);
	}
}

void list_event(void) {
	Event *e = malloc(sizeof(Event));
	FILE *f = fopen(DB, "rb");
	if (!f) {
		printf("Database does not exist!\n");
		return;
	}
	
	while (!feof(f)) {
		int fail = read_event(f, e);
		if (fail) {
			break;
		} else {
			putchar('\n');
			print_event(e);
		}
	}

	fclose(f);
}

int fetch_event_by_id(long unsigned id, Event* e) {
	FILE *f = fopen(DB, "rb");
	
	locate_event(f, id);
	int fail = read_event(f, e);
	fclose(f);
	
	if (fail) {
		return false;
	} else {
		return true;
	}
}

void print_event_by_id(long unsigned id) {
	Event *e = malloc(sizeof(Event));
	if(fetch_event_by_id(id, e)) {
		printf("Record %016lx found!\n", id);
		print_event(e);
	} else {
		printf("Record %016lx not found!\n", id);
	}
}

int update_event_by_id(long unsigned id, Event* e) {
	FILE* f = fopen(DB, "rb+");
	char prompt[128];
	char proceed[128];
	char abort[128];
	char retry[128];
	
	locate_event(f, id);
	
	if (feof(f)) {
		sprintf(prompt, "Record %016lx not found! Save new event anyway?", id);
		sprintf(proceed, "New event %016lx saved!", e->id);
		sprintf(abort, "Saving aborted!");
		sprintf(retry, "Save new event anyway?");
		
		int confirmed = confirm(prompt, proceed, abort, retry);
		
		if (confirmed) {
			write_event(f, e);
		}
	} else {
		sprintf(prompt, "Are you sure to overwrite record %016lx?", id);
		sprintf(proceed, "Record %016lx updated!", id);
		sprintf(abort, "Updating aborted!");
		sprintf(retry, "Are you sure to overwrite?");
		
		int confirmed = confirm(prompt, "proceed", abort, retry);
		
		if (confirmed) {
			e->id = id;
			write_event(f, e);
		}
	}
	
	fclose(f);
	return success;
}

int delete_event_by_id(long unsigned id) {
	FILE* f = fopen(DB, "rb+");
	char prompt[128];
	char proceed[128];
	char abort[128];
	char retry[128];
	
	locate_event(f, id);
	
	if (feof(f)) {
		printf("Record %016lx not found!\n", id);
		fclose(f);
		return failure(1);
	} else {
		sprintf(prompt, "Are you sure to delete record %016lx?", id);
		sprintf(proceed, "Record %016lx deleted!", id);
		sprintf(abort, "Deleting aborted!");
		sprintf(retry, "Are you sure to delete?");
		
		int confirmed = confirm(prompt, proceed, abort, retry);
		
		if (confirmed) {
			destroy_event(f);
		}
	}
	
	fclose(f);
	return success;
}