/*
* 화일명 : my_assembler.c
* 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
* 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
*
*/

/*
*
* 프로그램의 헤더를 정의한다.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#pragma warning(disable:4996) 
/*
* my_assembler 프로그램의 의존적인 데이터들이 정의된 곳이다.
*/

#include "my_assembler.h"

/* -----------------------------------------------------------------------------------
* 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
* 매계 : 실행 파일, 어셈블리 파일
* 반환 : 성공 = 0, 실패 = < 0
* 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
*		   또한 중간파일을 생성하지 않는다.
* -----------------------------------------------------------------------------------
*/


int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}

	if (assem_pass1() < 0){
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}
	if (assem_pass2() < 0){
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
		return -1;
	}
	
	make_objectcode("output") ; 
}
/* -----------------------------------------------------------------------------------
* 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
* 매계 : 없음
* 반환 : 정상종료 = 0 , 에러 발생 = -1
* 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
*		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
*		   구현하였다.
* -----------------------------------------------------------------------------------
*/

int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input")) < 0)
		return -1;
	return result;
}

/* -----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/

static int assem_pass1(void)
{
	/* add your code here */
	for (int i = 0; i < line_num; i++) {	//line_num만큼 토큰 파싱을 합니다.
		if (token_parsing(i) == -1)
			return -1;
	}
	return 0;

}

/* -----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/

static int assem_pass2(void)
{
	/* add your code here */
	if (token_line == 0) {
		return -1;
	}	//비정상 종료 조건
	for (int i = 0; i < token_line; i++) {	
		if (token_table[i]->label!=NULL && token_table[i]->label[0] == '.')
			continue;	//.일경우 프린트 안하고 넘어감
		int num_inst = -1;
		if (token_table[i]->operator != NULL) {
			num_inst = search_opcode(token_table[i]->operator);
		}	//해당 instruction을 찾음
		int opcode = 0;
		int nixbpe = 0;
		int disp = 0;
		int complete = 0;

		if (num_inst != -1) {	//instuction일경우
			for (int j = 0; j< 2; j++) {
				int tmp = inst[num_inst][2][j] - '0';
				opcode *= 16;	//4비트 만큼 증가
				if (tmp > 9) {
					tmp -= 7;
				}
				opcode += tmp;	//opcode에 값 저장
			}
		}
		//4형식일경우
		else if (token_table[i]->operator != NULL && token_table[i]->operator[0] == '+') {
			num_inst = search_opcode(token_table[i]->operator+1);	//+제외하고 instruction을 찾음
			for (int j = 0; j< 2; j++) {
				int tmp = inst[num_inst][2][j] - '0';
				opcode *= 16;
				if (tmp > 9) {
					tmp -= 7;
				}
				opcode += tmp;	//해당 opcode만큼 opcode에 저장
			}
		}

		//여기서부터 nixbpe 들어감
		if (token_table[i]->operand[0] != NULL) {
			if (token_table[i]->operand[0][0] == '@') {	
				nixbpe += 1;
				nixbpe *= 2;
				nixbpe *= 2;
			}	//indirect일 경우
			else if (token_table[i]->operand[0][0] == '#') {
				nixbpe *= 2;
				nixbpe += 1;
				nixbpe *= 2;
			}	//immediate일 경우
			else {
				nixbpe += 1;
				nixbpe *= 2;
				nixbpe += 1;
				nixbpe *= 2;
			}	//Direct일 경우
			if (token_table[i]->operand[1] != NULL) {
				if (strcmp(token_table[i]->operand[1], "X") == 0) {
					nixbpe += 1;
					nixbpe *= 2;
				}
			}	//X레지스터를 사용 할 경우
			else {
				nixbpe *= 2;
			}	//아닐경우 그냥 증가
			nixbpe *= 2;
			nixbpe += 1;
			nixbpe *= 2;	//pc relative로 초기화
			if (token_table[i]->operator[0] == '+') {
				nixbpe += 1;
			}	//4형식일 경우

			//여기까지 nixbpe

			if (token_table[i]->operand[0][0] == '#') {
				disp = atoi(token_table[i]->operand[0] + 1);
				nixbpe -= 2;
			}	//direct일경우 PC값을 빼줌
			else if (token_table[i]->operand[0][0] == '@') {
				int j;
				for (j = 0; j < label_num; j++) {
					if (strcmp(sym_table[j].symbol, token_table[i]->operand[0] + 1) == 0)
						break;
				}
				int tmp1 = atoi(token_table[i+1]->comment);
				disp = sym_table[j].addr - tmp1;
			}	//sym_table에서 해당 인덱스로 이동 후 disp계산
			else if (token_table[i]->operand[0][0] == '=') {
				int j;
				for (j = 0; j < literal_line; j++) {
					if (strcmp(lit_table[j].symbol, token_table[i]->operand[0]) == 0)
						break;
				}
				int tmp1 = atoi(token_table[i+1]->comment);
				disp = lit_table[j].addr - tmp1;
			}	//lit_table에서 찾은 후 disp계산
			else {
				if (token_table[i]->operator[0] == '+') {
					disp = 0;
					nixbpe -= 2;
					opcode *= 256;
					nixbpe *= 256;
				}	//4형식일 경우 포맷에 맞춰줌
				else if (strcmp(token_table[i]->operator, "RSUB") == 0) {
					disp = 0;
					nixbpe -= 2;
				}	//RSUB일 경우 PC값을 빼줌
				else {
					int j;
					for (j = label_num; 0 < j; j--) {
						if (strcmp(sym_table[j].symbol, token_table[i]->operand[0]) == 0)
							break;
					}
					if (i < token_line-1) {
						int tmp1 = atoi(token_table[i + 1]->comment);
						disp = sym_table[j].addr - tmp1;
						disp &= 0x000fff;
					}
				}	//sym_table에서 레이블을 찾은 후 disp값 계산
			}
		}
		if (num_inst != -1) {
			if (inst[num_inst][1][0] != '2') {
				opcode *= 65536;
				complete += opcode;
				nixbpe *= 4096;
				complete += nixbpe;
				complete += disp;
			}	//2형식이 아닐 경우
			else {	//2형식 일 경우
				opcode *= 16;
				complete += opcode;
				for (int j = 0; j < 2; j++) {
					if (token_table[i]->operand[j] == NULL) {
						complete += 0;
					}
					else if (strcmp(token_table[i]->operand[j], "B") == 0) {
						complete += 3;
					}	//B레지스터
					else if (strcmp(token_table[i]->operand[j], "S") == 0) {
						complete += 4;
					}	//S레지스터
					else if (strcmp(token_table[i]->operand[j], "T") == 0) {
						complete += 5;
					}	//T레지스터
					else if (strcmp(token_table[i]->operand[j], "F") == 0) {
						complete += 6;
					}	//F레지스터
					else if (strcmp(token_table[i]->operand[j], "A") == 0) {
						complete += 0;
					}	//A레지스터
					else if (strcmp(token_table[i]->operand[j], "X") == 0) {
						complete += 1;
					}	//X레지스터
					else if (strcmp(token_table[i]->operand[j], "L") == 0) {
						complete += 2;
					}	//L레지스터
					complete *= 16;
				}
				complete /= 16;	//마지막에 한번 더 증가된 값을 빼준다.
			}
		}

		printf("%04X\t", atoi(token_table[i]->comment));	//LocationCount값 출력
		if (token_table[i]->label == NULL){
			printf("\t", token_table[i]->label);
		}	//label 없을 경우
		else{
			printf("%s\t", token_table[i]->label);
		}	//label 있을 경우
		printf("%s\t", token_table[i]->operator);	//operator출력
		for (int j = 0; j < 3; j++) {
			if (token_table[i]->operand[j] == NULL) {
				break;
			}
			if (j != 0)
				printf(",");
			printf("%s", token_table[i]->operand[j]);	//operand갯수만큼 출력
		}

		if (token_table[i]->operator[0] != '=')
			token_table[i]->hexacode = NULL;
			//hexacode NULL로 초기화
		if (token_table[i]->operator != NULL) {
			if (strcmp(token_table[i]->operator, "BYTE") == 0) {	//BYTE일 경우
				char* tmp1;
				int itmp = 0;
				tmp1 = strtok(token_table[i]->operand[0], "'XC");
				
				for (int j = 0; j<strlen(tmp1); j++) {
					itmp += tmp1[j] - '0';
					if ((tmp1[j] - '0') > 9) {
						itmp -= 7;
					}
					itmp *= 16;
				}
				itmp /= 16;
				complete = itmp;
				strcat(token_table[i]->operand[0], "'");
				char tmp[20];
				itoa(complete, tmp, 10);
				token_table[i]->hexacode = (char*)malloc(strlen(tmp) + 1);
				strcpy(token_table[i]->hexacode, tmp);
			}	//hexacode에 포맷에 맞게끔 값을 바꾼후 저장
			else if (strcmp(token_table[i]->operator, "WORD") == 0) {	//WORD일 경우
				complete = 0;
				char tmp[20];
				itoa(complete, tmp, 10);
				token_table[i]->hexacode = (char*)malloc(strlen(tmp) + 1);
				strcpy(token_table[i]->hexacode, tmp);
			}	//hexacode에 포맷에 맞게끔 값을 바꾼후 저장
		}

		if (num_inst != -1 &&token_table[i]->comment != NULL) {
			char tmp[20];
			itoa(complete, tmp, 10);
			token_table[i]->hexacode = (char*)malloc(strlen(tmp) + 1);
			strcpy(token_table[i]->hexacode, tmp);
		}	//나머지일경우 hexacode를 포맷에 맞게 끔 값을 바꾼 후 저장
		if (token_table[i]->hexacode != NULL) {
			if (num_inst != -1 && inst[num_inst][1][0] != '2') {
				printf("\t%06X", atoi(token_table[i]->hexacode));
			}	//2형식일경우
			else if (strcmp(token_table[i]->operator, "WORD") == 0) {
				printf("\t%06X", atoi(token_table[i]->hexacode));
			}	//WORD일경우
			else if (token_table[i]->operator[1] == 'X'){
				printf("\t%02X", atoi(token_table[i]->hexacode));
			}	//리터럴X일경우
			else if (token_table[i]->operand[0] != NULL && token_table[i]->operand[0][0] == 'X'){
				printf("\t%02X", atoi(token_table[i]->hexacode));
			}	//그냥 X일경우
			else {
				printf("\t%04X", atoi(token_table[i]->hexacode));
			}
		}	//해당 포맷에 맞게 출력
		printf("\n");
	}
	return 0;
}
/* -----------------------------------------------------------------------------------
* 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
*        생성하는 함수이다.
* 매계 : 기계어 목록 파일
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : 기계어 목록파일 형식은 다음과 같다.
*
*	===============================================================================
*		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
*	===============================================================================
*
* -----------------------------------------------------------------------------------
*/

int init_inst_file(char *inst_file)
{
	/* add your code here */
	FILE* fin = fopen(inst_file, "r");	//파일 포인터를 사용합니다.
	char buffer[25];	//임시 저장소입니다.
	inst_index = 0;	
	locctr = 0;

	if (fin == NULL) {	//오류 조건입니다.
		return -1;
	}

	while (1) {
		if (fgets(buffer, 25, fin) == NULL) {	//읽은 작업이 끝나면 break!
			break;
		}

		char* tmp;
		int column_num = 0;

		tmp = strtok(buffer, "|");
		inst[inst_index][column_num] = (char*)malloc(strlen(tmp) + 1);
		strcpy(inst[inst_index][column_num++], tmp);
		tmp = strtok(NULL, "|");
		inst[inst_index][column_num] = (char*)malloc(strlen(tmp) + 1);
		strcpy(inst[inst_index][column_num++], tmp);
		tmp = strtok(NULL, "|");
		inst[inst_index][column_num] = (char*)malloc(strlen(tmp) + 1);
		strcpy(inst[inst_index][column_num++], tmp);
		tmp = strtok(NULL, "|");
		inst[inst_index][column_num] = (char*)malloc(strlen(tmp) + 1);
		strcpy(inst[inst_index][column_num++], tmp);
		inst_index++;
	}	//delimeter "|"기준으로 자른 후 inst배열에 넣어줍니다.

	fclose(fin);
	return 0;
}

/* -----------------------------------------------------------------------------------
* 설명 : 어셈블리 할 소스코드를 읽어오는 함수이다.
* 매계 : 어셈블리할 소스파일명
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/


int init_input_file(char *input_file)
{
	/* add your code here */
	FILE* fin = fopen(input_file, "r");	//파일 포인터를 사용합니다.
	char buffer[100];	//임시저장 버퍼입니다.
	line_num = 0;
	label_num = 0;
	literal_line = 0;
	pre_line = 0;

	if (fin == NULL) {	//비정상 종료 조건입니다.
		return -1;
	}

	while (1) {
		if (fgets(buffer, 100, fin) == NULL)	//다 읽게 되면 break!
			break;
		input_data[line_num] = (char*)malloc(strlen(buffer) + 1);
		strcpy(input_data[line_num++], buffer);	
	}	//동적 할당 후 input_data배열에 넣어줍니다.

	fclose(fin);
	return 0;
}

/* -----------------------------------------------------------------------------------
* 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
*        패스 1로 부터 호출된다.
* 매계 : 소스코드의 라인번호
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
* -----------------------------------------------------------------------------------
*/

int token_parsing(int index)
{
	/* add your code here */
	if (index > inst_index || index < 0) {
		return -1;
	}	//비정상 종료 조건입니다.
	int i=0, locTmp = 0;
	token_table[token_line] = (token*)malloc(sizeof(token));

	if (input_data[index][0] == '.') {		//처음에 .으로 시작하는경우
		char *tmp;
		tmp = strtok(input_data[index], "\t\n");
		token_table[token_line]->label = (char*)malloc(strlen(tmp) + 1);
		strcpy(token_table[token_line]->label, tmp);

		tmp = strtok(NULL, "\t\n");
		if (tmp != NULL) {
			token_table[token_line]->comment = (char*)malloc(strlen(tmp) + 1);
			token_table[token_line]->operator = NULL;
			token_table[token_line]->operand[0] = NULL;
			token_table[token_line]->operand[1] = NULL;
			token_table[token_line]->operand[2] = NULL;
			strcpy(token_table[token_line]->comment, tmp);
		}	//초기화 작업을 해줍니다.
		else {
			token_table[token_line]->operator = NULL;
			token_table[token_line]->operand[0] = NULL;
			token_table[token_line]->operand[1] = NULL;
			token_table[token_line]->operand[2] = NULL;
			token_table[token_line]->comment = NULL;
		}	//초기화 작업.
	}
	else if (input_data[index][0] == '\t') {	//처음에 tab으로 시작하는경우
		char *tmp;
		token_table[token_line]->label = NULL;

		tmp = strtok(input_data[index], "\t\n");
		token_table[token_line]->operator = (char*)malloc(strlen(tmp) + 1);	//동적할당
		strcpy(token_table[token_line]->operator, tmp);
		int num_inst = search_opcode(token_table[token_line]->operator);

		locTmp = locctr;
		if (strcmp(tmp, "LTORG") == 0) {	//리터럴일 경우
			for (int i = pre_line; i < literal_line; i++) {
				if (lit_table[i].symbol[1] == 'X') {
					lit_table[i].addr = locctr;
					locctr += (strlen(lit_table[i].symbol) - 4) / 2;
				}	//X일경우 길이에서 =X''을 제외한 나머지 부분을 2로 나눈값만큼 locctr을 증가
				else {
					lit_table[i].addr = locctr;
					locctr += strlen(lit_table[i].symbol) - 4;
				}	//X가 아닐경우 =C''을 제외한 나머지부분의 길이 만큼 locctr을 증가
			}
		}
		else if (strcmp(tmp, "END") == 0) {	//END일 경우
			for (int i = pre_line; i < literal_line; i++) {	//각 프로그램 동안
				if (lit_table[i].symbol[1] == 'X') {	//X면
					lit_table[i].addr = locctr;
					locctr += (strlen(lit_table[i].symbol) - 4) / 2;
				}	//=X''을 제외한 나머지 부분을 2로 나눈값만큼 locctr을 증가
				else {
					lit_table[i].addr = locctr;
					locctr += strlen(lit_table[i].symbol) - 4;
				}	//=C''을 제외한 나머지부분의 길이 만큼 locctr을 증가
			}
		}
		if (tmp[0] == '+') {	//4형식일경우
			locctr += 4;
		}
		if (num_inst != -1) {	//instruction일 경우
			if (strcmp(inst[num_inst][1], "2") == 0) {
				locctr += 2;
			}	//2형식이면
			else {
				locctr += 3;
			}	//3형식이면
		}	//opcode부분
		
		tmp = strtok(NULL, "\t\n");
		if (tmp != NULL) {
			int i = 0;
			char* tmp1 = strtok(tmp, ",");
			token_table[token_line]->operand[i] = (char*)malloc(strlen(tmp1) + 1);
			strcpy(token_table[token_line]->operand[i++], tmp1);
			while ((tmp1 = strtok(NULL, ",")) != NULL) {	// , 기준으로 자름
				token_table[token_line]->operand[i] = (char*)malloc(strlen(tmp1) + 1);
				strcpy(token_table[token_line]->operand[i++], tmp1);
			}	//각 인덱스로 토큰을 넣어줌
			for (i; i < 3; i++) {
				token_table[token_line]->operand[i] = NULL;
			}	//안 채워진 부분을 NULL로 초기화시켜줌
			if (tmp[0] == '=') {	//리터럴일경우
				int i;
				for (i = pre_line; i < literal_line; i++) {
					if (strcmp(lit_table[i].symbol, tmp) == 0) {
						break;
					}	//중복이 아니라면
				}
				if (i == literal_line) {
					strcpy(lit_table[literal_line++].symbol, tmp);
				}	//리터럴 테이블의 마지막에 넣어줌
			}
		}
		else {
			token_table[token_line]->operand[0] = NULL;
			token_table[token_line]->operand[1] = NULL;
			token_table[token_line]->operand[2] = NULL;	//operand부분
		}	//operand가 없다면 NULL로 초기화

		if (strcmp(token_table[token_line]->operator,"RESW") == 0) {
			int i = atoi(token_table[token_line]->operand[0]);
			locctr += i * 3;
		}	//RESW일 경우 i*3만큼 locctr증가
		else if (strcmp(token_table[token_line]->operator,"RESB") == 0) {
			int i = atoi(token_table[token_line]->operand[0]);
			locctr += i;
		}	//RESB일 경우 i만큼 locctr증가
		else if (strcmp(token_table[token_line]->operator, "WORD") == 0) {
			locctr += 3;
		}	//WORD일 경우 3만큼 locctr증가
		else if (strcmp(token_table[token_line]->operator, "BYTE") == 0) {
			if (token_table[token_line]->operand[0][0] == 'C') {
				locctr += strlen(token_table[token_line]->operand[0]) - 3;
			}	//BYTE일때 C일 경우 C''을 제외한 나머지 길이 만큼 locctr증가
			else if (token_table[token_line]->operand[0][0] == 'X'){
				locctr += (strlen(token_table[token_line]->operand[0]) - 3) / 2;
			}	//X일 경우 X''을 제외한 나머지 길이 / 2 만큼 locctr증가
		}

		tmp = strtok(NULL, "\t\n");
		char buffer[10] = { 0 };
		itoa(locTmp, buffer, 10);
		token_table[token_line]->comment = (char*)malloc(strlen(buffer) + 1);
		strcpy(token_table[token_line]->comment, buffer);	//comment부분에 locctr를 넣어줌

		token_line++;	//한줄 증가

		if (strcmp(token_table[token_line - 1]->operator, "LTORG") == 0 || strcmp(token_table[token_line - 1]->operator, "END") == 0) {
			for (int i = pre_line; i < literal_line; i++) {	//리터럴이나 END일경우
				token_table[token_line] = (token*)malloc(sizeof(token));
				token_table[token_line]->label = (char*)malloc(4);
				strcpy(token_table[token_line]->label, "*");
				token_table[token_line]->operator = (char*)malloc(strlen(lit_table[i].symbol) + 1);
				token_table[token_line]->operator = lit_table[i].symbol;
				token_table[token_line]->operand[0] = NULL;
				token_table[token_line]->operand[1] = NULL;
				token_table[token_line]->operand[2] = NULL;
				//리터럴 테이블로부터 리터럴 값을 불러와 써줌
				char tmp[20];

				if (lit_table[i].symbol[1] == 'C') {	//C일경우
					char* ctmp;
					int itmp=0;
					ctmp = strtok(lit_table[i].symbol, "=C'");
					for (int j = 0; j < strlen(ctmp); j++) {
						itmp += ctmp[j];
						itmp *= 16*16;
					}
					strcat(lit_table[i].symbol, "'");
					itmp /= 16*16;
					itoa(itmp, tmp, 10);
					token_table[token_line]->hexacode = (char*)malloc(strlen(tmp) + 1);
					strcpy(token_table[token_line]->hexacode, tmp);
				}	//포맷에 맞게 변경 후 넣어줌
				else if (lit_table[i].symbol[1] == 'X') {	//X일경우
					char* ctmp;
					int itmp = 0;
					ctmp = strtok(lit_table[i].symbol, "=X'");
					for (int j = 0; j<strlen(ctmp); j++) {
						itmp += ctmp[j] - '0';
						if ((ctmp[j] - '0') > 9) {
							itmp -= 7;
						}
						itmp *= 16*16;
					}
					itmp /= 16*16;
					strcat(lit_table[i].symbol, "'");
					itoa(itmp, tmp, 10);
					token_table[token_line]->hexacode = (char*)malloc(strlen(tmp) + 1);
					strcpy(token_table[token_line]->hexacode, tmp);
				}	//포맷에 맞게 변경 후 넣어줌
				itoa(lit_table[i].addr, tmp, 10);
				token_table[token_line]->comment = (char*)malloc(strlen(tmp) + 1);
				strcpy(token_table[token_line]->comment, tmp);	//comment부분에 locctr를 넣어줌
				token_line++;	//한줄 증가
			}
		}
	}
	else {		//전부 다 있는경우
		char *tmp;
		tmp = strtok(input_data[index], "\t\n");
		token_table[token_line]->label = (char*)malloc(strlen(tmp) + 1);
		strcpy(token_table[token_line]->label, tmp);
		sym_table[label_num].addr = locctr;
		strcpy(sym_table[label_num++].symbol, tmp);	//레이블 부분
		

		tmp = strtok(NULL, "\t\n");
		token_table[token_line]->operator = (char*)malloc(strlen(tmp) + 1);
		strcpy(token_table[token_line]->operator, tmp);
		locTmp = locctr;
		if (strcmp(tmp, "CSECT") == 0) {	//다른 CS일경우
			locTmp = 0;
			locctr = 0;
			pre_line = literal_line;
		}	//초기화
		if (tmp[0] == '+') {	//4형식일경우
			locctr += 4;
		}
		int num_inst = search_opcode(token_table[token_line]->operator);	//inst에 있는지 확인하여 잘못된접근을 막기위함이다.
		if (num_inst != -1) {
			if (strcmp(inst[num_inst][1], "2") == 0) {
				locctr += 2;
			}	//2형식일경우
			else {
				locctr += 3;
			}	//3형식 일경우
		}	//opcode부분

		tmp = strtok(NULL, "\t\n");
		if (tmp != NULL) {
			int i = 0;
			char* tmp1 = strtok(tmp, ",");
			token_table[token_line]->operand[i] = (char*)malloc(strlen(tmp1) + 1);
			strcpy(token_table[token_line]->operand[i++], tmp1);
			while ((tmp1 = strtok(NULL, ",")) != NULL) {
				token_table[token_line]->operand[i] = (char*)malloc(strlen(tmp1) + 1);
				strcpy(token_table[token_line]->operand[i++], tmp1);
			}	//토큰 갯수만큼 operand에 넣어줌
			for (i; i < 3; i++) {
				token_table[token_line]->operand[i] = NULL;
			}	//나머지 operand는 NULL로 초기화
			if (tmp[0] == '=') {	//리터럴 일경우
				int i;
				for (i = pre_line; i < literal_line; i++) {
					if (strcmp(lit_table[i].symbol, tmp) == 0) {
						break;
					}
				}	//리터럴의 중복을 막기 위해서
				if (i == literal_line) {
					strcpy(lit_table[literal_line++].symbol, tmp);
				}	//리터럴 테이블의 마지막에 넣어줌
			}
		}
		else
		{
			token_table[token_line]->operand[0] = NULL;
		}
		token_table[token_line]->operand[1] = NULL;
		token_table[token_line]->operand[2] = NULL;		//operand부분

		if (strcmp(token_table[token_line]->operator,"RESW") == 0) {
			int i = atoi(token_table[token_line]->operand[0]);
			locctr += i * 3;
		}	//RESW일 경우 i*3만큼 locctr증가
		else if (strcmp(token_table[token_line]->operator,"RESB") == 0) {
			int i = atoi(token_table[token_line]->operand[0]);
			locctr += i;
		}	//RESB일 경우 i만큼 locctr증가
		else if (strcmp(token_table[token_line]->operator, "WORD") == 0) {
			locctr += 3;
		}	//WORD일 경우 3만큼 locctr증가
		else if (strcmp(token_table[token_line]->operator, "BYTE") == 0) {
			if (token_table[token_line]->operand[0][0] == 'C') {
				locctr += strlen(token_table[token_line]->operand[0]) - 3;
			}	//BYTE일때 C일 경우 C''을 제외한 나머지 길이 만큼 locctr증가
			else if (token_table[token_line]->operand[0][0] == 'X'){
				locctr += (strlen(token_table[token_line]->operand[0]) - 3) / 2;
			}	//X일 경우 X''을 제외한 나머지 길이 / 2 만큼 locctr증가
		}

		tmp = strtok(NULL, "\t\n");
		char buffer[10] = { 0 };
		itoa(locTmp, buffer, 10);
		token_table[token_line]->comment = (char*)malloc(strlen(buffer) + 1);
		sprintf(token_table[token_line]->comment, "%s", buffer);
		//comment에 Location Count값을 넣어줌
		token_line++;
	}
	return 0;
}
/* -----------------------------------------------------------------------------------
* 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 문자열
* 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/

int search_opcode(char *str)
{
	/* add your code here */
	for (int i = 0; i < inst_index; i++) {
		if (!strcmp(inst[i][0], str)) {
			return i;
		}
	}
	return -1;
}
/* -----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/

void make_objectcode(char *file_name)
{
	/* add your code here */
	int len = 0;
	int pre_index = 0;
	FILE* fout = fopen(file_name, "w");
	for (int i = 0; i < token_line; i++){
		// 헤더테이블 필요함 전체 길이 표시할
		if (strcmp(token_table[i]->operator, "START") == 0) {	//START일경우
			int itmp1 = atoi(token_table[i]->comment);
			int j=0;
			for (j = i; j < token_line; j++) {
				if (strcmp(token_table[j]->operator, "CSECT") == 0) {
					break;
				}	//다음 CS까지 찾아서
			}
			int itmp2 = atoi(token_table[j - 1]->comment);
			fprintf(fout, "H%-6s%06X%06X\n", token_table[i]->label, itmp1, itmp2);
		}	//해당 프로그램의 시작주소와 끝 주소를 출력함
		else if (strcmp(token_table[i]->operator, "CSECT") == 0)	//CS일경우
		{
			fprintf(fout, "\n");
			for (int j = pre_index; j < i; j++) {
				if (token_table[j]->operator[0] == '+') {	//4형식일때
					fprintf(fout, "M%06X05+%s\n", atoi(token_table[j]->comment)+1, token_table[j]->operand[0]);
				}	//모디피케이션 포맷에 맞춰서 출력
				else if (strcmp(token_table[j]->operator, "WORD") == 0) {	//WORD일경우
					char* tmp = strtok(token_table[j]->operand[0], "-");
					fprintf(fout, "M%06X06+%s\n", atoi(token_table[j]->comment), tmp);
					tmp = strtok(NULL, "-");
					fprintf(fout, "M%06X06-%s\n", atoi(token_table[j]->comment), tmp);
				}	//모디피케이션 포맷에 맞춰서 출력
			}
			pre_index = i;	//각 프로그램 시작점을 알기 위한 변수
			int itmp1 = atoi(token_table[i]->comment);
			int j = 0;
			for (j = i+1; j < token_line; j++) {
				if (strcmp(token_table[j]->operator, "CSECT") == 0) {
					break;
				}
			}	//다음 CS를 찾아서
			int itmp2;
			if (strcmp(token_table[j - 1]->operator, "EQU") == 0) {	//EQU일 경우
				itmp2 = atoi(token_table[j - 1]->comment);
			}	//그대로 넣어줌
			else if (strcmp(token_table[j-2]->operator, "END") == 0) {	//END일 경우
				itmp2 = atoi(token_table[j - 2]->comment) + (strlen(token_table[j-1]->operator)-4)/2;
			}	//마지막 주소값을 더해줌
			else {
				itmp2 = atoi(token_table[j - 1]->comment) + 3;
			}	//주소값에 +3을 더해줌
			fprintf(fout, "E\n\nH%-6s%06X%06X\n", token_table[i]->label, itmp1, itmp2);
		}	//해당 프로그램의 시작주소와 끝 주소를 출력함
		else if (strcmp(token_table[i]->operator, "END") == 0)
		{
			fprintf(fout, "E\n");
		}	//END일 경우 E와 함께 개행을 해줌
		else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {	//REF부분
			fprintf(fout, "R");
			for (int j = 0; j < 3; j++) {
				if (token_table[i]->operand[j] == NULL) {
					break;
				}
				fprintf(fout, "%-6s", token_table[i]->operand[j]);
			}
			fprintf(fout, "\n");
		}	//해당 operand갯수 만큼 포맷에 맞춰 출력
		else if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {	//DEF부분
			fprintf(fout, "D");
			for (int j = 0; j < 3; j++) {
				int k;
				if (token_table[i]->operand[j] == NULL) {
					break;
				}
				for (k = 0; k < label_num; k++) {
					if (strcmp(sym_table[k].symbol, token_table[i]->operand[j]) == 0) {
						break;
					}
				}
				fprintf(fout, "%-6s%06X", token_table[i]->operand[j], sym_table[k].addr);
			}
			fprintf(fout, "\n");
		}	//해당 operand갯수 만큼 포맷에 맞춰 출력과 함께 해당 주소도 출력

		else if (strcmp(token_table[i]->operator, "RESW") == 0) {	//RESW일 경우
			fprintf(fout, "\n");	//개행을 해주고
			for (i; i < token_line; i++) {
				if (strcmp(token_table[i]->operator, "RESW") != 0) {
					break;
				}
			}	//다음 RESW가 아닐때 까지 i++
			continue;
		}
		else if (strcmp(token_table[i]->operator, "EQU") == 0) {
			continue;	//프로그램 계속
		}
		else if (strcmp(token_table[i]->operator, "RESB") == 0) {
			continue;	// 프로그램 계속
		}
		else if (strcmp(token_table[i]->operator, "LTORG") == 0) {
			continue;	//프로그램 계속
		}
		else
		{
			char tmp[100] = "";	//buffer
			int len = 0;	//길이
			fprintf(fout, "T%06X", atoi(token_table[i]->comment));
			while (i< token_line && token_table[i]->hexacode != NULL && len + strlen(token_table[i]->hexacode) <= 60)
			{
				if (strcmp(token_table[i]->operator, "CSECT") == 0 || strcmp(token_table[i]->operator, "RESW") == 0)
					break;	//해당 값이 나오면 break!
				if (token_table[i]->operator[0] == '+') {	//4형식 일경우
					len += 8;
				}
				else if (token_table[i]->operator[strlen(token_table[i]->operator)-1] == 'R'){	//오퍼레이터의 마지막 글자
					len += 4;
				}	//2형식일경우
				else if (strcmp(token_table[i]->operator, "BYTE") == 0) {	//BYTE일 경우
					len += 2;
				}
				else if (token_table[i]->operator[0] == '=') {	//리터럴 일경우
					if (token_table[i]->operator[1] == 'C') {
						len += (strlen(token_table[i]->operator) - 4)*2;
					}	//C에 해당 길이 *2 만큼 
					else if (token_table[i]->operator[1] == 'X') {
						len += (strlen(token_table[i]->operator) - 4);
					}	//X에 해당 길이 만큼 
				}
				else {	//그 외의 경우
					len += 6;
				}
				if (token_table[i]->hexacode != NULL) {
					int itmp = atoi(token_table[i]->hexacode);
					char ctmp[10];
					if (token_table[i]->operator[0] == '+') {	//4형식일경우
						sprintf(ctmp, "%08X", itmp);
					}
					else if (token_table[i]->operator[strlen(token_table[i]->operator) - 1] == 'R') {
						sprintf(ctmp, "%04X", itmp);
					}	//2형식일경우
					else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
						sprintf(ctmp, "%02X", itmp);
					}	//BYTE일경우
					else {	//그 외의 경우
						sprintf(ctmp, "%06X", itmp);
					}
					strcat(tmp, ctmp);	//tmp에 이어서 넣어줌
					i++;	//한줄 증가

					if (i < token_line && strcmp(token_table[i]->operator, "END") == 0) {
						itmp = atoi(token_table[i+1]->hexacode);
						sprintf(ctmp, "%02X", itmp);
						strcat(tmp, ctmp);
						break;
					}	//END일경우 리터럴만큼 더해줌
				}	
			}
			fprintf(fout, "%02X%s", strlen(tmp)/2, tmp);	//길이와 저장된 HEXACODE출력
			if (i < token_line && token_table[i]->hexacode != NULL ) {	//HEXACODE가 아니면
				fprintf(fout, "\n");
				i--;	//반복문 마지막에 증가된 값을 한번 빼줌
			}
			if (i < token_line && strcmp(token_table[i]->operator, "CSECT") == 0) {	//CSECT면
				i--;	//반복문 마지막에 증가된 값을 한번 빼줌
			}
		}
		if (i==token_line)	//마지막 라인일 경우
		{
			fprintf(fout, "\n");
			for (int j = pre_index; j < i; j++) {
				if (token_table[j]->operator[0] == '+') {
					fprintf(fout, "M%06X05+%s\n", atoi(token_table[j]->comment) + 1, token_table[j]->operand[0]);
				}
			}	//4형식에 해당하는 모디피케이션을 출력해줌
			fprintf(fout, "E");	//E를 출력해줌
			break;
		}
	}
	fclose(fout);
}