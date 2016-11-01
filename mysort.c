
/*****************************************************************************
 * Copyright (C) Himanshu N. Chopra himanchops@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.

 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char **b;

/*to read file line by line*/ 
typedef struct line {
	char *text, *temp;
	int col;
}line;

void init_line(line *l) {
	l->col = 0;
	l->temp = NULL;
}

line *l;

/*Realloc if more lines*/
void checksize(int count) {
	static int size_line = 50;
	if(count == size_line) {
		size_line *= 2;
		l = (line *) realloc(l, sizeof(line) * size_line);
		if(l == NULL)
			exit(1);
	}
}


void get_line(int nlines) {
	checksize(nlines);
	l[nlines].text = (char *) malloc(128);
	if(l[nlines].text == NULL)
		exit(1);
	init_line(&l[nlines]);
}


void get_linedata(int nlines) {
	char ch;
	int i = 0;
	while(i < nlines) {
		l[i].temp = l[i].text;
		while(ch = *(l[i].temp)){
			if(ch == 32 || ch == 9)
				l[i].col++;
			l[i].temp++;
		}
		l[i].col++;
		i++;
	}
}

/*to hold options*/
typedef struct keyfield {
	int reverse;

	int keydef;
	int key;

	int output;
	int unique;
	int num;
	
	int ignoreblanks;
	
	int ignorecase;

	char **p;
	int nfiles;
	char *output_file;
	
	int merge;
	int arr[10];
	int index;

	int check;
	
}keyfield;

void init_keys(keyfield *k) {
	k->reverse = 0;
	k->keydef = 0;
	k->key = 0;
	k->output = 0;
	k->unique = 0;
	k->num = 0;
	k->ignoreblanks = 0;
	k->ignorecase = 0;
	k->nfiles = 0;
	k->merge = 0;
	k->arr[0] = 0;
	k->index = 1;
	k->check = 0;
}

/*Read lines from files*/
int readfiles(keyfield *k) {
	FILE *fp;
	int size_text, j = 0, nlines = 0;
	char *s;
	while( j < k->nfiles) {
		fp = fopen(k->p[j++], "r");
		get_line(nlines);
		while(fgets(l[nlines].text, 128, fp)) {
									/*get complete line, catenation if needed*/
			if(strlen(l[nlines].text) == 127) {
				size_text = 128;
 				s = (char *) malloc(size_text);
				while(fgets(s, size_text, fp) && s[strlen(s) - 1] != '\n') {
					size_text += 128;
		    			l[nlines].text = (char *) realloc(l[nlines].text, size_text);
		    			strcat(l[nlines].text, s);
		 		}
				if(s[strlen(s) - 1] == '\n') {
					size_text += 128;
					l[nlines].text = (char *) realloc(l[nlines].text, size_text);
					strcat(l[nlines].text, s);
				}
				free(s);
			}
			nlines++;
			get_line(nlines);
		}
		free(l[nlines].text);
		k->arr[k->index] = nlines - 1;
		(k->index)++;
	}
	get_linedata(nlines);
	return nlines;
}

void help() {
	printf("Usage: ./project [OPTION]... [FILE]...\n");
	printf("   or: ./project [OPTION]... --files0-from=F \n");
	printf("Write sorted concatenation of all FILE(s) to standard output.\n");
	printf("With no file, or when FILE is -, read standard output.\n");
	printf("Ordering Options:\n");
	printf("%7s, ignore leading blanks\tIgnore leading blanks\n", "-b");
	printf("%7s, ignore-case\t\tFold lower-case to upper-case characters\n", "-f");
	printf("%7s, numersic sort\t\tCompare according to string numeric value\n", "-n");
	printf("%7s, reverse\t\tReverse the result of comparisions\n", "-r");
	printf("Other Options: \n");
	printf("%7s, check\t\t\tCheck for sorted input, and report first bad line; Do not sort\n", "-c");
	printf("%7s, key=KEYDEF\t\tSort via a key. KEYDEF gives location\n", "-k");
	printf("%7s, merge\t\t\tMerge already sorted files; Do not sort.\n", "-m");
	printf("%7s, output=FILE\t\tWrite result to FILE instead of standard output\n", "-o");
	printf("%7s, unique\t\t\tRemove duplicates from result\n", "-u");	
	printf("     %s,\t\tRead input from the files specified by NUL-terminated names in file F\n\t\t\t\tIf F is - then read names from standard input\n","--files0-from=F");
	printf("%7s,--help\t\t\tPrint this usage and exit\n", "-h");
	exit(0);
}


#define LOW 0	/*lowercase alphabet*/
#define UPP 1	/*uppercase alphabet*/

/*General Case Precedence : New Line, Space, Digit, Lowercase, Uppercase, OTHER*/
int compare(char *p, char *q, int num, int ignorecase, int ignoreblanks) {
	int p_case, q_case, p_num, q_num;
	if(strcmp(p,q) == 0)
		return 1;

	if(*p == 10)
		return 1;
	if(*q == 10)
		return 0;
	
	if(num) {
		p_num = atoi(p); q_num = atoi(q);
		if(p_num == 0 || q_num == 0)
			return compare(p, q, 0, ignorecase, ignoreblanks);
		if(p_num == q_num)
			return compare(p + 1, q + 1, num, ignorecase, ignoreblanks);
		if(p_num < q_num)
			return 1;
		if(p_num > q_num)
			return 0;
	}

	if(*p == *q)
		return compare(p + 1, q + 1, num, ignorecase, ignoreblanks);


	if(*p == 32)
		return 1;
	if(*q == 32)
		return 0;


	if(isdigit(*p) && isdigit(*q))
		if(*p < *q)
			return 1;
		else
			return 0;
	if(isdigit(*p))
		return 1;	
	if(isdigit(*q))
		return 0;


	if(*p >= 65 && *p <= 90)
		p_case = UPP;
	else if(*p >= 97 && *p <=122)
		p_case = LOW;
	if(*q >= 65 && *q <= 90)
		q_case = UPP;
	else if(*q >= 97 && *q <=122)
		q_case = LOW;


	if(p_case == q_case)
		if (*p < *q)
			return 1;
		else
			return 0;

	if(ignorecase == 1) {
		if(strcasecmp(p, q) <= 0)
			return 1;
		else
			return 0;
	}

	if(p_case == LOW)
		if((*p - 'a') <= (*q - 'A'))
			return 1;
		else
 			return 0;
	if(p_case == UPP)
		if((*p - 'A') < (*q - 'a'))
			return 1;
		else
 			return 0;

	if(*p < *q)
		return 1;
	return 0;
}

void merging(int low, int mid, int high, keyfield k) {
	int l1, l2, i, j, flag;
	char a = '\n';
	for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
		l[l1].temp = l[l1].text;
		l[l2].temp = l[l2].text;
		/*for -k option*/
		if(k.keydef == 1) {
			
			while(*(l[l1].temp) == 32 || *(l[l1].temp) == 9)	
				l[l1].temp++;
			j = 1;
			if(k.key <= l[l1].col)
				while(j < k.key && *(l[l1].temp)) {
					flag = 0;
					if(k.ignoreblanks) {
						while(*(l[l1].temp) == 32 || *(l[l1].temp) == 9) {					
							flag = 1;
							l[l1].temp++;
						}
						if(flag)
							j++;
					}
					if(*(l[l1].temp) == 32 || *(l[l1].temp) == 9)
						j++;
					if(flag == 0)
						l[l1].temp++;
				}
			else if(k.key <= l[l2].col)
				l[l2].temp = &a;
			

			while(*(l[l2].temp) == 32 || *(l[l2].temp) == 9)	
				l[l2].temp++;
			j = 1;
			if(k.key <= l[l2].col)
				while(j < k.key && *(l[l2].temp)) {
					flag = 0;
					if(k.ignoreblanks) {
						while(*(l[l2].temp) == 32 || *(l[l2].temp) == 9) {					
							flag = 1;
							l[l2].temp++;
						}
						if(flag)
							j++;
					}
					if(*(l[l2].temp) == 32 || *(l[l2].temp) == 9)
						j++;
					if(flag == 0)
						l[l2].temp++;
				}
			else if (k.key <= l[l1].col)
				l[l1].temp = &a;
		}
		if(compare(l[l1].temp, l[l2].temp, k.num, k.ignorecase, k.ignoreblanks))
			b[i] = l[l1++].text;
		else
			b[i] = l[l2++].text;
	}
	while(l1 <= mid)
		b[i++] = l[l1++].text;

   	while(l2 <= high) 
		b[i++] = l[l2++].text;

	for(i = low; i <= high; i++)
		l[i].text = b[i];
}

/*Merge Sort*/
void sort(int low, int high, keyfield k) {
	int mid;

	if(low < high) {
		mid = (low + high) / 2;
		sort(low, mid, k);
		sort(mid + 1, high, k);
		merging(low, mid, high, k);
	}
	else
		return;
}


#define SIZE 20

int main(int argc, char *argv[]) {
	keyfield k;
	init_keys(&k);
	k.p = (char **) malloc(sizeof(char *) * (argc - 1));
	int nlines = 0, i, j, sum, m, len;
	char c, str[SIZE], *str1 = (char *) malloc(128), *str2 = str1;
	FILE *f, *fp;

	/*Read Keys, Options and Files*/
	void read() {
		for(i = 1; i < argc; i++) {
			if(argv[i][0] == '-') {
				switch(argv[i][1]) {
					case 'r':
						k.reverse = 1;
						break;
					case 'k':
						if(strlen(argv[i]) > 2) {
							j = 2; sum = 0; 
							while(c = argv[i][j])
								if(isdigit(argv[i][j]))
									sum = sum * 10 + argv[i][j++] - '0';
								else {
									printf("Sort: Invalid number at field start: Invalid count at start of '%c'\n", c);
									exit(1);
								}
							k.key = sum;
						}
						else {
							j = 0; sum = 0; i++;
							if(i == argc) {
								printf("Sort: Option requires an argument --'k'\nTry 'mysort --help' for more information.\n\n");
								exit(1);
							}
							while(c = argv[i][j])
								if(isdigit(argv[i][j]))
									sum = sum * 10 + argv[i][j++] - '0';
								else {
									printf("Sort: Invalid number at field start: Invalid count at start of '%c'\n", c);							
									exit(1);
								}
							k.key = sum;
						}						
						if(k.key != 0)
							k.keydef = 1;
						break;
					case 'o':
						k.output = 1;
						if(i == argc - 1) {
							printf("Sort: Option requires an argument --'o'\nTry 'sort --help' for more information.\n");
							exit(1);
						}
						k.output_file = argv[++i];
						break;
					case 'u':
						k.unique = 1;
						break;
					case 'n':
						k.num = 1;
						break;
					case 'h':
						help();
					case '-':
						if(strcmp(argv[i], "--help") == 0)
							help();

						for(j = 0 ; j < 14; j++)
							str[j] = argv[i][j];
						str[j] = '\0';
						if(strcmp(str, "--files0-from=") == 0) {
							if(strlen(argv[i]) == 14) {
								printf("Sort: Option requires an argument file '--files0-from='\nTry 'sort --help' for more information.\n");
								exit(1);
							}
							m = 14; j = 0;
							while(argv[i][m])
								str[j++] = argv[i][m++];
							str[j] = '\0';
							if(strcmp(str, "-") == 0) {
								fgets(str1, 128, stdin);
								if(*str1 == 0) {
									printf("Sort: No input from '-'\n");
									exit(1);
								}
								while(*str1) {
									f = fopen(str1, "r");
									if(f) {
										k.p[k.nfiles] = (char *) malloc(strlen(str1) + 1);
										strcpy(k.p[k.nfiles], str1);
										(k.nfiles)++;
										fclose(f);
									}
									else {
										printf("Sort: Cannot read file: %sNo such file or directory\n", str1+2);
										exit(1);
									}
									str1 = str1 + strlen(str1) + 1;
								}
								break;
							}

							f = fopen(str, "r");
							if(f == NULL) {
								printf("Sort : Cannot open '%s' for reading: No such file or directory\n", str);
								exit(1);
							}
							fgets(str1, 128, f);
							if(*str1 == 0) {
								printf("Sort: No input from '%s'\n", str);
								exit(1);
							}
							while(*str1) {
								f = fopen(str1, "r");
								if(f) {
									k.p[k.nfiles] = (char *) malloc(strlen(str1) + 1);
									strcpy(k.p[k.nfiles], str1);
									(k.nfiles)++;
									fclose(f);
								}
								else {
									printf("Sort: Cannot read file: %sNo such file or directory\n", str1+2);
									exit(1);
								}
								str1 = str1 + strlen(str1) + 1;
							}
						}
						else {
							printf("Invalid option - '%s'\nTry 'sort --help' for more information.\n", argv[i]);
							exit(1);
						}
						break;
					case 'm' :
						k.merge = 1;
						break;
					case 'b' :
						k.ignoreblanks = 1;
						break;
					case 'f' :
						k.ignorecase = 1;
						break;
					case 'c' : 
						k.check = 1;
						break;
					default:
						printf("Invalid Option - '%s'\nTry 'sort --help' for more information\n", argv[i]);
						exit(1);
				}
			}
			else {
				f = fopen(argv[i], "r");
				if(f != NULL) {
					k.p[k.nfiles] = argv[i];
					(k.nfiles)++;
					fclose(f);
				}
				else {
					printf("Sort: Cannot read '%s': No such file or directory\n", argv[i]);
					exit(1);
				}
			}
		}
	}

	read();
	free(str2);

	/*read as filter*/
	l = (line *) malloc(sizeof(line) * 50);
	if(l == NULL)
		exit(1);

	/*read from files*/
	if(k.nfiles > 0)
		nlines = readfiles(&k);
	
	/*read from stdin*/
	else if(k.merge == 0) {
		int size_text; 
		nlines = 0;
		char *s;
		get_line(nlines);
		while(fgets(l[nlines].text, 128, stdin)) {

			if(strlen(l[nlines].text) == 127) {
				size_text = 128;
 				s = (char *) malloc(size_text);
				while(fgets(s, size_text, stdin) && s[strlen(s) - 1] != '\n') {
					size_text += 128;
	    				l[nlines].text = (char *) realloc(l[nlines].text, size_text);
					strcat(l[nlines].text, s);
		 		}
				if(s[strlen(s) - 1] == '\n') {
					size_text += 128;
					l[nlines].text = (char *) realloc(l[nlines].text, size_text);
					strcat(l[nlines].text, s);
				}
				free(s);
			}
			nlines++;
			get_line(nlines);
		}
		free(l[nlines].text);
		get_linedata(nlines);
	}
	

	/*Check if sorted*/
	if(k.check) {
		if(k.nfiles > 1) {
			printf("sort: Extra operand(s) not allowed with '-c': %s\n", k.p[1]);
			return 0;
		}
		if(k.reverse)
			for(i = 1; i < nlines; i++) {
				if(strcmp(l[i - 1].text, l[i].text) && compare(l[i - 1].text, l[i].text, k.num, k.ignorecase, k.ignoreblanks) == 1) {
					if(k.nfiles == 1)
						printf("Sort: %s:%d: Disorder: %s", k.p[0], i + 1, l[i].text);
					else
						printf("Sort: -:%d: Disorder: %s", i + 1, l[i].text);
					return 0;
				}
			}
		else
			for(i = 1; i < nlines; i++){
				if(compare(l[i - 1].text, l[i].text, k.num, k.ignorecase, k.ignoreblanks) == 0) {
					if(k.nfiles == 1)
						printf("Sort: %s:%d: Disorder: %s", k.p[0], i + 1, l[i].text);
					else
						printf("Sort: -:%d: Disorder: %s", i + 1, l[i].text);
					return 0;
				}
			}
		return 0;
	}


	/*Sort or Merge*/
	b = (char **) malloc(sizeof(char *) * nlines);
	if(k.merge == 0)	
		sort(0, nlines - 1, k);
	else if(k.nfiles > 1)
		for(i = 1; i < k.index - 1; i++)
			merging(0, k.arr[i], k.arr[i + 1], k);
	else{
		if(k.nfiles == 0)
			printf("No FILE input for merging\n");
		else
			printf("Only one FILE input for merging : %s\n", k.p[0]);
		return 0;
	}
	free(b);
	int tlines = nlines;

	/*Remove Duplicates*/
	if(k.unique == 1) {
		for(i = 1; i < nlines; i++) {
			if(k.ignorecase == 0)
				while(i != nlines && strcmp(l[i].text, l[i - 1].text) == 0) {
					for(j = i - 1; j < nlines - 1; j++)
						strcpy(l[j].text, l[j + 1].text);
					nlines--;
				}
			else {
				while(i != nlines && (strcasecmp(l[i].text, l[i - 1].text) == 0)) {
					for(j = i - 1; j < nlines - 1; j++)
						strcpy(l[j].text, l[j + 1].text);
					nlines--;
				}
			}
		}
	}


	/*Output on STDOUT or in FILE*/
	if(k.output == 0) {
		printf("------- \n");
		i = nlines - 1;
		for(j = 0; j < nlines; j++)
			if(k.reverse == 1)
				printf("%s", l[i--].text);
			else
				printf("%s", l[j].text);
		printf("-------\n");	
	}
	else {
		FILE *fp;
		fp = fopen(k.output_file, "w");
		i = nlines - 1;
		for(j = 0; j < nlines; j++)
			if(k.reverse == 1)
				fprintf(fp,"%s", l[i--].text);
			else
				fprintf(fp,"%s", l[j].text);
	}


	for(i = 0; i < tlines; i++)
		free(l[i].text);
	free(l);
	free(k.p);
	return 0;
}
