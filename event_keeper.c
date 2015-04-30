/*
 * FILE: event_keeper.c
 * NAME: Chuhan Feng
 * DATE: Apr 28, 2015
 * CLASS: CSE2421, T/TH 4:10PM
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "event.h"
#define bool uint8_t
#define true 1
#define false 0
#define failure(x) x
#define success 0

// Clear redundant input
void clear_input(void);

// Check date format
int date_checker(short, short, short);

int main(void) {
	Event *new = malloc(sizeof(Event));
	char *title = calloc(sizeof(char), MAX_TITLE_LEN);
	short y, m, d;
	long unsigned id;
	char instruction;
	printf("\nWelcome!\nType commands below to start:\n");
	printf("l - List all events\n");
	printf("a - Add event\n");
	printf("f - Find event\n");
	printf("u - Update event\n");
	printf("d - Delete event\n");
	printf("q - Quit program\n");
	printf("h - Help\n");
	putchar('\n');
	
	while (true) {
		printf(">> ");
		scanf("%c", &instruction);
		if (instruction == '\n') {
			continue;
		}
		clear_input();
		
		switch (instruction) {
			case 'l':
				printf("------- LIST EVENT -------\n");
				list_event();
				break;
			case 'a':
				printf("------- ADD EVENT --------\n");
				printf("Enter event title:\n");
				fgets(title, MAX_TITLE_LEN, stdin);
				strtok(title, "\n");
			prompt1:
				printf("Enter event date: (yyyy mm dd)\n");
				scanf("%hd %hd %hd", &y, &m, &d);
				if (!date_checker(y, m, d)) {
					printf("Invalid date! Try again.\n");
					goto prompt1;
				}
				create_event(new, title, y, m, d);
				save_event_bin(new);
				clear_input();
				break;
			case 'f':
				printf("------- FIND EVENT -------\n");
				printf("Enter event id:\n");
				scanf("%lx", &id);
				putchar('\n');
				print_event_by_id(id);
				clear_input();
				break;
			case 'u':
				printf("------ UPDATE EVENT ------\n");
				printf("Enter event id:\n");
				scanf("%lx", &id);
				clear_input();
				printf("Enter event title:\n");
				fgets(title, MAX_TITLE_LEN, stdin);
				strtok(title, "\n");
			prompt2:
				printf("Enter event date: (yyyy mm dd)\n");
				scanf("%hd %hd %hd", &y, &m, &d);
				if (!date_checker(y, m, d)) {
					printf("Invalid date! Try again.\n");
					goto prompt2;
				}
				create_event(new, title, y, m, d);
				update_event_by_id(id, new);
				clear_input();
				break;
			case 'd':
				printf("------ DELETE EVENT ------\n");
				printf("Enter event id:\n");
				scanf("%lx", &id);
				delete_event_by_id(id);
				clear_input();
				break;
			case 'q':
				puts("Exit program!\n");
				exit(success);
			case 'h':
				printf("l - List all events\n");
				printf("a - Add event\n");
				printf("f - Find event\n");
				printf("u - Update event\n");
				printf("d - Delete event\n");
				printf("q - Quit program\n");
				printf("h - Help\n");
				break;
			default:
				continue;
		}
		putchar('\n');
	}
	
	return success;
}

void clear_input(void) {
	while ( getchar() != '\n' );
}

int date_checker(short year, short month, short day) {
	int cons1 = year > 0;
	int cons2 = month > 0;
	int cons3 = day > 0;
	int cons4 = year <= 9999;
	int cons5 = month <= 12;
	int cons6;
	
	switch (month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			cons6 = day <= 31;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			cons6 = day <= 30;
			break;
		case 2:
			if (year % 4) {
				cons6 = day <= 28;
			} else if (year % 100) {
				cons6 = day <= 29;
			} else if (year % 400) {
				cons6 = day <= 28;
			} else {
				cons6 = day <= 29;
			}
			break;
		default:
			cons6 = false;
			break;
	}
	
	return cons1 && cons2 && cons3 && cons4 && cons5 && cons6;
}