
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

char dummy[5];

/*
===========================================================================
宣言部
===========================================================================
*/

#define MAX_LINE_LEN 1024
#define MAX_DATA_STORE 100
int profile_data_nitems = 0;


struct profile {
	char NAME[70];  //商品名
	int year1;		//2019年の売上個数
	int year2;		//2018年の売上個数
	int year3;		//2017年の売上個数
	int year4;		//2016年の売上個数
	int point;		//商品のポイント
};

struct profile profile_data_store[MAX_DATA_STORE];
struct profile* profile_data_pnt[MAX_DATA_STORE];

struct profile* new_profile(struct profile* p, char* line);

void exec_command(char cmd, char* num);

void make_profile_pnt(int i, int j) {
	//構造体をポインタに設定し直している
	//(ソート時に構造体全体を置き換えることによりメモリを多く使用してしまうことを防ぐ)
	profile_data_pnt[i] = &profile_data_store[j];
}


/*
===========================================================================
解析部
===========================================================================
*/

int subst(char* str, char c1, char c2) {
	//c1をc2に置き換える
	int num = 0;
	while (*str) {
		if (*str == c1) {
			*str = c2;
			num++;
		}
		str++;
	}
	return num;
}

int split(char* str, char* ret[], char sep, int max) {
	int num = 0;

	ret[num++] = str;

	while (*str && num < max) {
		if (*str == sep) {
			*str = '\0';
			ret[num++] = str + 1;
		}
		str++;
	}
	return num;
}

int get_line(FILE* abc, char* line) {
	//文字列を読み込む
	if (fgets(line, MAX_LINE_LEN + 1, abc) == NULL) {
		return 0;
	}
	subst(line, '\n', '\0');
	if (*line == '\0')
		return 0;
	//なにも打たれていなかったらゼロを返す。
	return 1;
}

struct profile* new_profile(struct profile* p, char* line) {
	//読み込んだ文字列を構造体に格納する関数
	char* ret[5];

	if (split(line, ret, ',', 5) != 5) {
		printf("Please input 5 data\n");
		profile_data_nitems--;
		return NULL;  //カンマの数が違っていたらエラー
	}

	strncpy(p-> NAME, ret[0], 69);
	p-> NAME[69] = '\0';
	p->year1 = atoi(ret[1]);
	p->year2 = atoi(ret[2]);
	p->year3 = atoi(ret[3]);
	p->year4 = atoi(ret[4]);

	return p;
}

void parse_line(char* line) {
	int i, count = 0;

	if (line[0] == '%') {
		exec_command(line[1], &line[3]);
	}
	else {
		for (i = 0; i < profile_data_nitems; i++) {
			if (profile_data_store[i]. NAME == '\0') {
				new_profile(&profile_data_store[i], line);
				make_profile_pnt(profile_data_nitems, i);
				profile_data_nitems++;
				count++;

			}
		}
		if (count == 0) {
			new_profile(&profile_data_store[profile_data_nitems], line);
			make_profile_pnt(profile_data_nitems, profile_data_nitems);
			profile_data_nitems++;
		}
	}
}


/*
===========================================================================
各種コマンド実現部
===========================================================================
*/
void cmd_quit() {
	/*
	Qコマンド :
	*/
	exit(0);
}


void cmd_check() {//保持データの項目数の表示
	printf("%d profile(s)\n", profile_data_nitems);
}


void print(int nitems, int num);

void cmd_print(int nitems) { //保持データの出力
	if (nitems > 0 && profile_data_nitems > nitems) {
		print(nitems, 0);
	}
	else if (nitems < 0 && profile_data_nitems > -nitems) {
		print(profile_data_nitems, profile_data_nitems + nitems);
	}
	else {
		print(profile_data_nitems, 0);
	}
}

void print(int nitems, int num) {
	//%P "数値"の正負によってnitemsによって場合分けし構造体を表示する関数
	int i;
	int year = 2019;
	struct profile* p;
	printf("=======================================\n");
	printf("Name");
	printf("%8d",year);
	printf("%8d", year-1);
	printf("%8d", year-2);
	printf("%8d\n", year-3);
	printf("=======================================\n");

	for (i = num; i < 6; i++) {
		p = profile_data_pnt[i];
		printf("%4s", p-> NAME);
		printf("%8d", p->year1);
		printf("%8d", p->year2);
		printf("%8d", p->year3);
		printf("%8d\n", p->year4);
	}
	printf("=======================================\n");
}


int cmd_read(char* filename) {
	FILE* fp;
	char line[MAX_LINE_LEN + 1];

	fp = fopen(filename, "r");

	if (fp == NULL) { /* fp が NULL なら，オープン失敗 */
		fprintf(stderr, "Could not open file: %s\n", filename);
		return -1;
	}

	while (get_line(fp, line)) {
		parse_line(line);
	}
	fclose(fp);
	return 0;

}


void fprint(FILE* fp, struct profile* p);
int cmd_write(char* filename) {
	int i;
	char com[2];
	FILE* fp;

	if (profile_data_nitems == 0) {
		printf("There are no data.\n");
		printf("Please input Y or N depending on whether you want to output data (= Y) or not (=N).\n");
		scanf("%s", com);
		if (com[0] == 'Y') {
			printf("CAN DO IT!!\n");
		}
		else if (com[0] == 'N') {
			return -1;
		}
	}
	fp = fopen(filename, "w");

	for (i = 0; i < profile_data_nitems; i++) {
		fprint(fp, profile_data_pnt[i]);
	}
	fclose(fp);

	return 0;
}

void fprint(FILE* fp, struct profile* p) {
	fprintf(fp, "%s,", p-> NAME);
	fprintf(fp, "%d,", p->year1);
	fprintf(fp, "%d,", p->year2);
	fprintf(fp, "%d,", p->year3);
	fprintf(fp, "%d\n", p->year4);
}

int cmd_find_search(char* word, struct profile* p) {
	char year1[11] , date[11], year3[11], year4[11];

	sprintf(year1, "%d", p->year1);
	sprintf(year3, "%d", p->year3);
	sprintf(year4, "%d", p->year4);
	if (strcmp(p-> NAME, word) == 0 ||
		strcmp(year1, word) == 0 ||
		strcmp(date, word) == 0 ||
		strcmp(year3, word) == 0 ||
		strcmp(year4, word) == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

void cmd_find_print(struct profile* p) {
	printf(" NAME     : %s\n", p-> NAME);
	printf("Name   : %d\n", p->year1);
	printf("Birth  : %d\n", p->year2);
	printf("Addr   : %d\n", p->year3);
	printf("comment: %d\n", p->year4);
}


void cmd_find(char* word) {
	int i, j = 0;

	for (i = 0; i < profile_data_nitems; i++) {
		cmd_find_search(word, profile_data_pnt[i]);
		if (cmd_find_search(word, profile_data_pnt[i]) == 0) {
			printf("Profile No.%d\n", i + 1);
			cmd_find_print(profile_data_pnt[i]);
			j++;
		}
	}
	if (j == 0) {
		printf("No Data.\n");
	}
}

void cmd_sort_swap(int n1, int n2) {
	//ポインタを用いた置換
	struct profile* shelf;

	shelf = profile_data_pnt[n1];
	profile_data_pnt[n1] = profile_data_pnt[n2];
	profile_data_pnt[n2] = shelf;
}

void cmd_sort_1();
void cmd_sort_2();
void cmd_sort_3();
void cmd_sort_4();
void cmd_sort_5();

void cmd_sort(int num) {

	switch (num) {
	case 1: cmd_sort_1(); break;
	case 2: cmd_sort_2(); break;
	case 3: cmd_sort_3(); break;
	case 4: cmd_sort_4(); break;
	case 5: cmd_sort_5(); break;
	}
}

void cmd_sort_1() {
	int i, j;

	for (i = 0; i < profile_data_nitems - 1; i++) {
		for (j = 0; j < profile_data_nitems - 1; j++) {

			if (strcmp(profile_data_pnt[j]-> NAME, profile_data_pnt[j + 1]-> NAME)>0)
				cmd_sort_swap(j, j + 1);
		}
	}
}
void cmd_sort_2() {
	int i, j;
	for (i = 0; i < profile_data_nitems - 1; i++) {
		for (j = 0; j < profile_data_nitems - 1; j++) {
			if (profile_data_pnt[j]->year1 > profile_data_pnt[j + 1]->year1)
				cmd_sort_swap(j, j + 1);
		}
	}
}
void cmd_sort_3() {
	int i, j;

	for (i = 0; i < profile_data_nitems - 1; i++) {
		for (j = 0; j < profile_data_nitems - 1; j++) {
			if (profile_data_pnt[j]->year2 > profile_data_pnt[j + 1]->year2) {
				cmd_sort_swap(j, j + 1);
			}
		}
	}
}
void cmd_sort_4() {
	int i, j;
	for (i = 0; i < profile_data_nitems - 1; i++) {
		for (j = 0; j < profile_data_nitems - 1; j++) {
			if (profile_data_pnt[j]->year3 > profile_data_pnt[j + 1]->year3)
				cmd_sort_swap(j, j + 1);
		}
	}
}
void cmd_sort_5() {
	int i, j;
	for (i = 0; i < profile_data_nitems - 1; i++) {
		for (j = 0; j < profile_data_nitems - 1; j++) {
			if (profile_data_pnt[j]->year4 > profile_data_pnt[j + 1]->year4)
				cmd_sort_swap(j, j + 1);
		}
	}

}

void cmd_trend() {
	//1年前に最も売れている商品を選ぶ
	int i,num;
	int MAX = profile_data_pnt[0]->year1;

	cmd_sort_1();

	for (i = 0; i < profile_data_nitems-1; i--) {
		int j = 5;
		profile_data_pnt[i]->point = j;
		j--;
		if (j < 0) {
			j = 0;
		}
	}


	/*
	for (i=1; i < 4; i++) {
		if (MAX < profile_data_pnt[i]->year1) {
			MAX = profile_data_pnt[i]->year1;
		}
	}
	//その売れている商品に5ポイントを付ける
	for (i = 1; i < 4; i++) {
		if (MAX == profile_data_pnt[i]->year1) {
			num = i; //そのときの番数をnumに設定
		}
	}
	profile_data_pnt[num]->point += 5;


	//2年前に最も売れている商品を選ぶ
	MAX = profile_data_pnt[0]->year2;
	for (i = 1; i < 4; i++) {
		if (MAX < profile_data_pnt[i]->year2) {
			MAX = profile_data_pnt[i]->year2;
		}
	}
	//その商品に4ポイントを付ける
	for (i = 1; i < 7; i++) {
		if (MAX == profile_data_pnt[i]->year2) {
			num = i; //そのときの番数をnumに設定
		}
	}
	profile_data_pnt[num]->point += 4;

	//2年前に最も売れている商品を選ぶ
	MAX = profile_data_pnt[0]->year3;
	for (i = 1; i < 4; i++) {
		if (MAX < profile_data_pnt[i]->year3) {
			MAX = profile_data_pnt[i]->year3;
		}
	}
	//その商品に4ポイントを付ける
	for (i = 1; i < 7; i++) {
		if (MAX == profile_data_pnt[i]->year3) {
			num = i; //そのときの番数をnumに設定
		}
	}
	profile_data_pnt[num]->point += 3;
	//2年前に最も売れている商品を選ぶ
	MAX = profile_data_pnt[0]->year4;
	for (i = 1; i < 4; i++) {
		if (MAX < profile_data_pnt[i]->year4) {
			MAX = profile_data_pnt[i]->year4;
		}
	}
	//その商品に4ポイントを付ける
	for (i = 1; i < 7; i++) {
		if (MAX == profile_data_pnt[i]->year4) {
			num = i; //そのときの番数をnumに設定
		}
	}
	profile_data_pnt[num]->point += 2;
	*/

	printf("来年に最も売れそうな商品は以下のポイント順です。\n");
	for (i = 0; i < profile_data_nitems-1; i++) {
		printf("%s : %dポイント\n", profile_data_pnt[i]-> NAME, profile_data_pnt[i]->point);
	}
}

void cmd_help() {
	printf("%%Q : Quitting this app.\n");
	printf("%%C : Indicating number of the data you have ever enrolled.\n");
	printf("%%P : Indicating data you have ever enrolled.\n");
	printf("%%R : Reading disignated data and enrolling.\n");
	printf("%%W : Writing out data you have ever enrolled to disignated file.\n");
	printf("%%F : Indicating data with the same word as the entered word\n");
	printf("%%S : Sort data in order of ' NAME(1)','year1 name(2)','date of establishment','location','remarks data',\n");
	printf("%%T : Indicating each function.\n");
	printf("%%H : Indicating each function.\n");

}

void make_profile_delete(struct profile* a);
struct profile* del_profile(struct profile* p, char* line);
void cmd_delete(int num) {
	printf("coming soon\n");
	/*
	int i;
	if (num == 0 || num > profile_data_nitems) {
		for (num = 0; num < profile_data_nitems; num++) {
			profile_data_nitems = 0;
		}
		printf("DELETE ALL DATA\n");
	}
	else if (num != 0 && num <= profile_data_nitems) {
		profile_data_pnt[num - 1]-> NAME = '\0';
		for (i = num - 1; i < profile_data_nitems - 1; i++) {
			cmd_sort_swap(i, i + 1);
		}
		profile_data_nitems--;
	}
	*/
}

struct profile* del_profile(struct profile* p, char* line) {
	//読み込んだ文字列を構造体に格納する関数
	char* ret[5], * ret2[3];
	split(line, ret, ',', 5);

	strncpy(p-> NAME, ret[0], 69);
	p-> NAME[69] = '\0';

	p->year1 = atoi(ret[1]);
	p->year2 = atoi(ret[2]);
	p->year3 = atoi(ret[3]);
	p->year4 = atoi(ret[4]);

	return p;
}


void cmd_edit(int num) {
	char new_data[MAX_LINE_LEN + 1];
	get_line(stdin, new_data);
	new_profile(profile_data_pnt[num - 1], new_data);
}


void exec_command(char cmd, char* num) {
	switch (cmd) {
	case 'Q': cmd_quit(); break;
	case 'C': cmd_check(); break;
	case 'P': cmd_print(num); break;
	case 'R': cmd_read(num); break;
	case 'W': cmd_write(num); break;
	case 'F': cmd_find(num); break;
	case 'S': cmd_sort(atoi(num)); break;
	case 'H': cmd_help(); break;
	case 'T': cmd_trend(); break;
	case 'D': cmd_delete(atoi(num)); break;
	case 'E': cmd_edit(atoi(num)); break;


	default: fprintf(stderr, "Invalid command %c: ignored.\n", cmd);
		break;
	}
}


/*
===========================================================================
main関数
===========================================================================
*/

int main() {
	char line[MAX_LINE_LEN + 1];   //line:打ち込まれた文字列
	printf("If you don't know how to use this app, please input `%%H'\n");

	int t1, t2, dt;
	char hairetu[100];

	while (get_line(stdin, line)) {
		parse_line(line);
	}

	fgets(dummy, 5, stdin);

	return 0;
}
