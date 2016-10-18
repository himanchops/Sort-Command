#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char **b;
int size_line = 50;

/*to read file line by line*/ 
typedef struct line {
	char *text, *temp;
	int lenght;
	int col;
}line;

void init_line(line *l) {
	l->lenght = 0;
	l->col = 0;
	l->temp = NULL;
}

line *l;

void checksize(int count) {
	if(count == size_line) {
		size_line *= 2;
		l = (line *) realloc(l, sizeof(line) * size_line);
		if(l == NULL) {
			printf("Failed for %d lines\n", size_line);
			exit(1);
		}
	}
}


void get_line(int nlines) {
	checksize(nlines);
	l[nlines].text = (char *) malloc(sizeof(char) * 100);
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
			if(isblank(ch))
				l[i].col++;
			l[i].temp++;
		}
		l[i].col++;
		l[i].lenght = strlen(l[i].text);
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
}

int readfiles(keyfield *k) {
	FILE *fp;
	int size_text, j = 0, nlines = 0;
	char *s;
	while( j < k->nfiles) {
		fp = fopen(k->p[j++], "r");
		get_line(nlines);
		while(fgets(l[nlines].text, 100, fp)) {
									/*get complete line, catenation if needed*/
			if(strlen(l[nlines].text) == 99) {
				size_text = 100;
 				s = (char *) malloc(sizeof(char) * size_text);
				while(fgets(s, size_text, stdin) && s[strlen(s) - 1] != '\n') {
					size_text += 100;
		    			l[nlines].text = (char *) realloc(l[nlines].text, sizeof(char) * size_text);
		    			strcat(l[nlines].text, s);
		 		}
				if(s[strlen(s) - 1] == '\n') {
					size_text += 100;
					l[nlines].text = (char *) realloc(l[nlines].text, sizeof(char) * size_text);
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
	printf("usage: mysort [OPTION]... [FILE]...\n");
	printf("Write sorted concatenation of all FILE(s) to standard output.\n");
	printf("With no file, read standard output.\n");
	printf("OPTIONS:\n");
	printf("%7s, reverse\t\tReverse the result of comparisions\n", "-r");
	printf("%7s, key=KEYDEF\t\tSort via a key. KEYDEF gives location\n", "-k");
	printf("%7s, output=FILE\t\tWrite result to FILE instead of standard output\n", "-o");
	printf("%7s, numersic sort\t\tCompare according to string numeric value\n", "-n");
	printf("%7s, \t\t\tPrint this usage and exit\n", "--help");	
	exit(0);
}

void position (keyfield *k, int nlines) {
	int i = 0, j;
	while(i < nlines) {
		l[i].temp = l[i].text;
		j = 1;
		/*for -k option*/
		if(k->keydef == 1)
			if(k->key <= l[i].col)
				while(j < k->key) {
					if(isblank(*(l[i].temp)))
						j++;
					l[i].temp++;
				}
		i++;
	}
}


#define LOW 0	/*lowercase alphabet*/
#define UPP 1	/*uppercase alphabet*/

/*General case precedence : Space, Digit, Lowercase, Uppercase, OTHER*/
	
int compare(char *p, char *q,int num) {
	int p_case, q_case;

	if(strcmp(p,q) == 0)
		return 1;


	if(num == 1) {
		if(atoi(p) == 0 || atoi(q) == 0)
			return compare(p, q, 0);
		if(atoi(p) == atoi(q) && *p == *q)
			return compare(p + 1, q + 1, num);
		if(atoi(p) < atoi(q))
			return 1;
		if(atoi(p) > atoi(q))
			return 0;
	}
	if(*p == *q)
		return compare(p + 1, q + 1, num);


	if(isblank(*p))
		return 1;
	else if(isblank(*q))
		return 0;


	if(isdigit(*p) && isdigit(*q)){
		if(*p < *q)
			return 1;
		else
			return 0;
	}
	else if(isdigit(*p))
		return 1;	
	else if(isdigit(*q))
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
	else if(p_case == LOW)
		if((*p - 'a') <= (*q - 'A'))
			return 1;
		else
 			return 0;
	else if(p_case == UPP)
		if((*p - 'A') < (*q - 'a'))
			return 1;
		else
 			return 0;
	

	else if(*p < *q)
		return 1;
	else
		return 0;
	
}

void merging(int low, int mid, int high, int num) {
	int l1, l2, i;

	for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
	if(compare(l[l1].temp, l[l2].temp, num))
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

void sort(int low, int high, int num) {
	int mid;

	if(low < high) {
		mid = (low + high) / 2;
		sort(low, mid, num);
		sort(mid + 1, high, num);
		merging(low, mid, high, num);
	}
	else
		return;
}



int main(int argc, char *argv[]) {
	keyfield k;
	init_keys(&k);
	k.p = (char **) malloc(sizeof(char*) * (argc - 1));
	int nlines = 0, i, j, sum;
	char c;

	void read() {
		for(i = 1; i < argc; i++) {
			if(argv[i][0] == '-')
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
									printf("sort: invalid number at field start: invalid count at start of '%c'\n", c);
									exit(1);
								}
							k.key = sum;
						}
						else {
							j = 0; sum = 0; i++;
							if(i == argc) {
								printf("sort: option requires an argument -- 'k'\nTry 'mysort --help' for more information.\n\n");
								exit(1);
							}
							while(c = argv[i][j])
								if(isdigit(argv[i][j]))
									sum = sum * 10 + argv[i][j++] - '0';
								else {
									printf("sort: invalid number at field start: invalid count at start of '%c'\n", c);							
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
							printf("sort: option requires an argument -- 'o'\nTry 'sort --help' for more information.\n\n");
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
					case '-':
						if(strcmp(argv[i], "--help") == 0)
							help();
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
					default:
						printf("INVALID OPTION\n");
						exit(1);
				}
			else {
				FILE *f;
				f = fopen(argv[i], "r");
				if(f != NULL) {
					k.p[k.nfiles] = argv[i];
					(k.nfiles)++;
					fclose(f);
				}
				else {
					printf("Cannot open file: %s", argv[i]);
					exit(1);
				}
			}
		} 	
	}	

	/*read as filter*/
	read();
	l = (line *) malloc(sizeof(line) * size_line);
	if(l == NULL)
		exit(1);
							/*read from files*/
	if(k.nfiles)
		nlines = readfiles(&k);
							/*read from stdin*/
	else{
		int size_text; 
		nlines = 0;
		char *s;
		l = (line *) malloc(sizeof(line) * 10);
		if(l == NULL) {
			printf("Failed for %d lines\n", size_line);
			exit(1);
		}
		get_line(nlines);
		while(fgets(l[nlines].text, 100, stdin)) {
			if(strlen(l[nlines].text) == 99) {
				size_text = 100;
 				s = (char *) malloc(sizeof(char) * size_text);
				while(fgets(s, size_text, stdin) && s[strlen(s) - 1] != '\n') {
					size_text += 100;
	    				l[nlines].text = (char *) realloc(l[nlines].text, sizeof(char) * size_text);
					strcat(l[nlines].text, s);
		 		}
				if(s[strlen(s) - 1] == '\n') {
					size_text += 100;
					l[nlines].text = (char *) realloc(l[nlines].text, sizeof(char) * size_text);
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
	
	position(&k, nlines);
	
	b = (char **) malloc(sizeof(char *) * nlines);
	for(i = 0; i < nlines; i++)
		b[i] = (char *) malloc(sizeof(char) * (l[i].lenght + 1));
	
	if(k.merge == 0)					
		sort(0, nlines - 1, k.num);
	/*else if(k.nfiles > 1)					
		for(i = 1; i < k.index; i++)		
			merging(0, k.arr[i], k.arr[i + 1], k.num);
	else {							
		printf("ONLY ONE FILE INPUT FOR MERGING\n");
		return 0;
	}
	if(k.unique != 0) {
		for(i = 1; i < nlines; i++) {
			if(!strcmp(l[i].text, l[i - 1].text)) {
				free(l[i-1].text);
				for(j = i - 1; j < nlines - 1; j++)
					l[j] = l[j + 1];
				free(&l[j]);
				nlines--;
			}
		}
	}
	*/		
	if(k.output == 0) {
		printf(" ------ \n");
		i = nlines - 1;
		for(j = 0; j < nlines; j++)
			if(k.reverse == 1)
				printf("%s", l[i--].text);
			else
				printf("%s", l[j].text);
		printf(" ------\n");	
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
}
