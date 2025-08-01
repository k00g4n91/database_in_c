#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
	printf("Usage: %s -n -f <database file>\n", argv[0]);
	printf("\t -n - create new database file\n");
	printf("\t -f - (required) path to database file\n");
	return;
}

int main(int argc, char *argv[]) { 

	// dbhdr is a slut that is going to be passed around to all the functions
	// and updated
	char *addstring = NULL;
	struct dbheader_t *dbhdr = NULL;
	int dbfd = -1;
	char *filepath = NULL;	
	bool newFile = false;
	int c; 
	struct employee_t *employees = NULL;

	while ((c = getopt(argc, argv, "nf:a:")) != -1)
	{
		switch(c)
		{
			case 'n':
				newFile = true;
				break;
			case 'f':
				filepath = optarg;
				break;	
			case 'a':
				addstring = optarg;
				break;
			case ':':
				perror("missing option argument\n");
				return -1;
			case '?':
				printf("don't recognize %c\n", c);
				break;
			default:
				return -1;
		}
	}


	if (filepath == NULL)
	{
		printf("filepath is required argument\n");
		print_usage(argv);
		return 0;
	}


	if (newFile)
	{
		dbfd = create_db_file(filepath);
		if (dbfd == STATUS_ERROR)
		{
			printf("Unable to create db file\n");
			return -1;
		}
		
		if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR)
		{
			printf("Failed to create database header\n");
			return -1;
		}
	}
	else
	{
		dbfd = open_db_file(filepath);
		if (dbfd == STATUS_ERROR)
		{
			printf("Unable to open db file\n");
			return -1;
		}

		if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) 
		{
			printf("Failed to validate database header\n");
			return -1;
		}
	}

	if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS)
	{
		printf("Failed to read employees");
		return 0;
	}


	if (addstring) 
	{
		dbhdr->count++;
		employees = realloc(employees, dbhdr->count * sizeof(struct employee_t));

		add_employee(dbhdr, employees, addstring); 
	}


	output_file(dbfd, dbhdr, employees);

	return 0;

}
