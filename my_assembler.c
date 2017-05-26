/*
* ȭ�ϸ� : my_assembler.c
* ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
* �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
*
*/

/*
*
* ���α׷��� ����� �����Ѵ�.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#pragma warning(disable:4996) 
/*
* my_assembler ���α׷��� �������� �����͵��� ���ǵ� ���̴�.
*/

#include "my_assembler.h"

/* -----------------------------------------------------------------------------------
* ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
* �Ű� : ���� ����, ����� ����
* ��ȯ : ���� = 0, ���� = < 0
* ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
*		   ���� �߰������� �������� �ʴ´�.
* -----------------------------------------------------------------------------------
*/


int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0){
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	if (assem_pass2() < 0){
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	
	make_objectcode("output") ; 
}
/* -----------------------------------------------------------------------------------
* ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
* �Ű� : ����
* ��ȯ : �������� = 0 , ���� �߻� = -1
* ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
*		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
*		   �����Ͽ���.
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
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/

static int assem_pass1(void)
{
	/* add your code here */
	for (int i = 0; i < line_num; i++) {	//line_num��ŭ ��ū �Ľ��� �մϴ�.
		if (token_parsing(i) == -1)
			return -1;
	}
	return 0;

}

/* -----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/

static int assem_pass2(void)
{
	/* add your code here */
	if (token_line == 0) {
		return -1;
	}	//������ ���� ����
	for (int i = 0; i < token_line; i++) {	
		if (token_table[i]->label!=NULL && token_table[i]->label[0] == '.')
			continue;	//.�ϰ�� ����Ʈ ���ϰ� �Ѿ
		int num_inst = -1;
		if (token_table[i]->operator != NULL) {
			num_inst = search_opcode(token_table[i]->operator);
		}	//�ش� instruction�� ã��
		int opcode = 0;
		int nixbpe = 0;
		int disp = 0;
		int complete = 0;

		if (num_inst != -1) {	//instuction�ϰ��
			for (int j = 0; j< 2; j++) {
				int tmp = inst[num_inst][2][j] - '0';
				opcode *= 16;	//4��Ʈ ��ŭ ����
				if (tmp > 9) {
					tmp -= 7;
				}
				opcode += tmp;	//opcode�� �� ����
			}
		}
		//4�����ϰ��
		else if (token_table[i]->operator != NULL && token_table[i]->operator[0] == '+') {
			num_inst = search_opcode(token_table[i]->operator+1);	//+�����ϰ� instruction�� ã��
			for (int j = 0; j< 2; j++) {
				int tmp = inst[num_inst][2][j] - '0';
				opcode *= 16;
				if (tmp > 9) {
					tmp -= 7;
				}
				opcode += tmp;	//�ش� opcode��ŭ opcode�� ����
			}
		}

		//���⼭���� nixbpe ��
		if (token_table[i]->operand[0] != NULL) {
			if (token_table[i]->operand[0][0] == '@') {	
				nixbpe += 1;
				nixbpe *= 2;
				nixbpe *= 2;
			}	//indirect�� ���
			else if (token_table[i]->operand[0][0] == '#') {
				nixbpe *= 2;
				nixbpe += 1;
				nixbpe *= 2;
			}	//immediate�� ���
			else {
				nixbpe += 1;
				nixbpe *= 2;
				nixbpe += 1;
				nixbpe *= 2;
			}	//Direct�� ���
			if (token_table[i]->operand[1] != NULL) {
				if (strcmp(token_table[i]->operand[1], "X") == 0) {
					nixbpe += 1;
					nixbpe *= 2;
				}
			}	//X�������͸� ��� �� ���
			else {
				nixbpe *= 2;
			}	//�ƴҰ�� �׳� ����
			nixbpe *= 2;
			nixbpe += 1;
			nixbpe *= 2;	//pc relative�� �ʱ�ȭ
			if (token_table[i]->operator[0] == '+') {
				nixbpe += 1;
			}	//4������ ���

			//������� nixbpe

			if (token_table[i]->operand[0][0] == '#') {
				disp = atoi(token_table[i]->operand[0] + 1);
				nixbpe -= 2;
			}	//direct�ϰ�� PC���� ����
			else if (token_table[i]->operand[0][0] == '@') {
				int j;
				for (j = 0; j < label_num; j++) {
					if (strcmp(sym_table[j].symbol, token_table[i]->operand[0] + 1) == 0)
						break;
				}
				int tmp1 = atoi(token_table[i+1]->comment);
				disp = sym_table[j].addr - tmp1;
			}	//sym_table���� �ش� �ε����� �̵� �� disp���
			else if (token_table[i]->operand[0][0] == '=') {
				int j;
				for (j = 0; j < literal_line; j++) {
					if (strcmp(lit_table[j].symbol, token_table[i]->operand[0]) == 0)
						break;
				}
				int tmp1 = atoi(token_table[i+1]->comment);
				disp = lit_table[j].addr - tmp1;
			}	//lit_table���� ã�� �� disp���
			else {
				if (token_table[i]->operator[0] == '+') {
					disp = 0;
					nixbpe -= 2;
					opcode *= 256;
					nixbpe *= 256;
				}	//4������ ��� ���˿� ������
				else if (strcmp(token_table[i]->operator, "RSUB") == 0) {
					disp = 0;
					nixbpe -= 2;
				}	//RSUB�� ��� PC���� ����
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
				}	//sym_table���� ���̺��� ã�� �� disp�� ���
			}
		}
		if (num_inst != -1) {
			if (inst[num_inst][1][0] != '2') {
				opcode *= 65536;
				complete += opcode;
				nixbpe *= 4096;
				complete += nixbpe;
				complete += disp;
			}	//2������ �ƴ� ���
			else {	//2���� �� ���
				opcode *= 16;
				complete += opcode;
				for (int j = 0; j < 2; j++) {
					if (token_table[i]->operand[j] == NULL) {
						complete += 0;
					}
					else if (strcmp(token_table[i]->operand[j], "B") == 0) {
						complete += 3;
					}	//B��������
					else if (strcmp(token_table[i]->operand[j], "S") == 0) {
						complete += 4;
					}	//S��������
					else if (strcmp(token_table[i]->operand[j], "T") == 0) {
						complete += 5;
					}	//T��������
					else if (strcmp(token_table[i]->operand[j], "F") == 0) {
						complete += 6;
					}	//F��������
					else if (strcmp(token_table[i]->operand[j], "A") == 0) {
						complete += 0;
					}	//A��������
					else if (strcmp(token_table[i]->operand[j], "X") == 0) {
						complete += 1;
					}	//X��������
					else if (strcmp(token_table[i]->operand[j], "L") == 0) {
						complete += 2;
					}	//L��������
					complete *= 16;
				}
				complete /= 16;	//�������� �ѹ� �� ������ ���� ���ش�.
			}
		}

		printf("%04X\t", atoi(token_table[i]->comment));	//LocationCount�� ���
		if (token_table[i]->label == NULL){
			printf("\t", token_table[i]->label);
		}	//label ���� ���
		else{
			printf("%s\t", token_table[i]->label);
		}	//label ���� ���
		printf("%s\t", token_table[i]->operator);	//operator���
		for (int j = 0; j < 3; j++) {
			if (token_table[i]->operand[j] == NULL) {
				break;
			}
			if (j != 0)
				printf(",");
			printf("%s", token_table[i]->operand[j]);	//operand������ŭ ���
		}

		if (token_table[i]->operator[0] != '=')
			token_table[i]->hexacode = NULL;
			//hexacode NULL�� �ʱ�ȭ
		if (token_table[i]->operator != NULL) {
			if (strcmp(token_table[i]->operator, "BYTE") == 0) {	//BYTE�� ���
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
			}	//hexacode�� ���˿� �°Բ� ���� �ٲ��� ����
			else if (strcmp(token_table[i]->operator, "WORD") == 0) {	//WORD�� ���
				complete = 0;
				char tmp[20];
				itoa(complete, tmp, 10);
				token_table[i]->hexacode = (char*)malloc(strlen(tmp) + 1);
				strcpy(token_table[i]->hexacode, tmp);
			}	//hexacode�� ���˿� �°Բ� ���� �ٲ��� ����
		}

		if (num_inst != -1 &&token_table[i]->comment != NULL) {
			char tmp[20];
			itoa(complete, tmp, 10);
			token_table[i]->hexacode = (char*)malloc(strlen(tmp) + 1);
			strcpy(token_table[i]->hexacode, tmp);
		}	//�������ϰ�� hexacode�� ���˿� �°� �� ���� �ٲ� �� ����
		if (token_table[i]->hexacode != NULL) {
			if (num_inst != -1 && inst[num_inst][1][0] != '2') {
				printf("\t%06X", atoi(token_table[i]->hexacode));
			}	//2�����ϰ��
			else if (strcmp(token_table[i]->operator, "WORD") == 0) {
				printf("\t%06X", atoi(token_table[i]->hexacode));
			}	//WORD�ϰ��
			else if (token_table[i]->operator[1] == 'X'){
				printf("\t%02X", atoi(token_table[i]->hexacode));
			}	//���ͷ�X�ϰ��
			else if (token_table[i]->operand[0] != NULL && token_table[i]->operand[0][0] == 'X'){
				printf("\t%02X", atoi(token_table[i]->hexacode));
			}	//�׳� X�ϰ��
			else {
				printf("\t%04X", atoi(token_table[i]->hexacode));
			}
		}	//�ش� ���˿� �°� ���
		printf("\n");
	}
	return 0;
}
/* -----------------------------------------------------------------------------------
* ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)��
*        �����ϴ� �Լ��̴�.
* �Ű� : ���� ��� ����
* ��ȯ : �������� = 0 , ���� < 0
* ���� : ���� ������� ������ ������ ����.
*
*	===============================================================================
*		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
*	===============================================================================
*
* -----------------------------------------------------------------------------------
*/

int init_inst_file(char *inst_file)
{
	/* add your code here */
	FILE* fin = fopen(inst_file, "r");	//���� �����͸� ����մϴ�.
	char buffer[25];	//�ӽ� ������Դϴ�.
	inst_index = 0;	
	locctr = 0;

	if (fin == NULL) {	//���� �����Դϴ�.
		return -1;
	}

	while (1) {
		if (fgets(buffer, 25, fin) == NULL) {	//���� �۾��� ������ break!
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
	}	//delimeter "|"�������� �ڸ� �� inst�迭�� �־��ݴϴ�.

	fclose(fin);
	return 0;
}

/* -----------------------------------------------------------------------------------
* ���� : ����� �� �ҽ��ڵ带 �о���� �Լ��̴�.
* �Ű� : ������� �ҽ����ϸ�
* ��ȯ : �������� = 0 , ���� < 0
* ���� :
*
* -----------------------------------------------------------------------------------
*/


int init_input_file(char *input_file)
{
	/* add your code here */
	FILE* fin = fopen(input_file, "r");	//���� �����͸� ����մϴ�.
	char buffer[100];	//�ӽ����� �����Դϴ�.
	line_num = 0;
	label_num = 0;
	literal_line = 0;
	pre_line = 0;

	if (fin == NULL) {	//������ ���� �����Դϴ�.
		return -1;
	}

	while (1) {
		if (fgets(buffer, 100, fin) == NULL)	//�� �а� �Ǹ� break!
			break;
		input_data[line_num] = (char*)malloc(strlen(buffer) + 1);
		strcpy(input_data[line_num++], buffer);	
	}	//���� �Ҵ� �� input_data�迭�� �־��ݴϴ�.

	fclose(fin);
	return 0;
}

/* -----------------------------------------------------------------------------------
* ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
*        �н� 1�� ���� ȣ��ȴ�.
* �Ű� : �ҽ��ڵ��� ���ι�ȣ
* ��ȯ : �������� = 0 , ���� < 0
* ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
* -----------------------------------------------------------------------------------
*/

int token_parsing(int index)
{
	/* add your code here */
	if (index > inst_index || index < 0) {
		return -1;
	}	//������ ���� �����Դϴ�.
	int i=0, locTmp = 0;
	token_table[token_line] = (token*)malloc(sizeof(token));

	if (input_data[index][0] == '.') {		//ó���� .���� �����ϴ°��
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
		}	//�ʱ�ȭ �۾��� ���ݴϴ�.
		else {
			token_table[token_line]->operator = NULL;
			token_table[token_line]->operand[0] = NULL;
			token_table[token_line]->operand[1] = NULL;
			token_table[token_line]->operand[2] = NULL;
			token_table[token_line]->comment = NULL;
		}	//�ʱ�ȭ �۾�.
	}
	else if (input_data[index][0] == '\t') {	//ó���� tab���� �����ϴ°��
		char *tmp;
		token_table[token_line]->label = NULL;

		tmp = strtok(input_data[index], "\t\n");
		token_table[token_line]->operator = (char*)malloc(strlen(tmp) + 1);	//�����Ҵ�
		strcpy(token_table[token_line]->operator, tmp);
		int num_inst = search_opcode(token_table[token_line]->operator);

		locTmp = locctr;
		if (strcmp(tmp, "LTORG") == 0) {	//���ͷ��� ���
			for (int i = pre_line; i < literal_line; i++) {
				if (lit_table[i].symbol[1] == 'X') {
					lit_table[i].addr = locctr;
					locctr += (strlen(lit_table[i].symbol) - 4) / 2;
				}	//X�ϰ�� ���̿��� =X''�� ������ ������ �κ��� 2�� ��������ŭ locctr�� ����
				else {
					lit_table[i].addr = locctr;
					locctr += strlen(lit_table[i].symbol) - 4;
				}	//X�� �ƴҰ�� =C''�� ������ �������κ��� ���� ��ŭ locctr�� ����
			}
		}
		else if (strcmp(tmp, "END") == 0) {	//END�� ���
			for (int i = pre_line; i < literal_line; i++) {	//�� ���α׷� ����
				if (lit_table[i].symbol[1] == 'X') {	//X��
					lit_table[i].addr = locctr;
					locctr += (strlen(lit_table[i].symbol) - 4) / 2;
				}	//=X''�� ������ ������ �κ��� 2�� ��������ŭ locctr�� ����
				else {
					lit_table[i].addr = locctr;
					locctr += strlen(lit_table[i].symbol) - 4;
				}	//=C''�� ������ �������κ��� ���� ��ŭ locctr�� ����
			}
		}
		if (tmp[0] == '+') {	//4�����ϰ��
			locctr += 4;
		}
		if (num_inst != -1) {	//instruction�� ���
			if (strcmp(inst[num_inst][1], "2") == 0) {
				locctr += 2;
			}	//2�����̸�
			else {
				locctr += 3;
			}	//3�����̸�
		}	//opcode�κ�
		
		tmp = strtok(NULL, "\t\n");
		if (tmp != NULL) {
			int i = 0;
			char* tmp1 = strtok(tmp, ",");
			token_table[token_line]->operand[i] = (char*)malloc(strlen(tmp1) + 1);
			strcpy(token_table[token_line]->operand[i++], tmp1);
			while ((tmp1 = strtok(NULL, ",")) != NULL) {	// , �������� �ڸ�
				token_table[token_line]->operand[i] = (char*)malloc(strlen(tmp1) + 1);
				strcpy(token_table[token_line]->operand[i++], tmp1);
			}	//�� �ε����� ��ū�� �־���
			for (i; i < 3; i++) {
				token_table[token_line]->operand[i] = NULL;
			}	//�� ä���� �κ��� NULL�� �ʱ�ȭ������
			if (tmp[0] == '=') {	//���ͷ��ϰ��
				int i;
				for (i = pre_line; i < literal_line; i++) {
					if (strcmp(lit_table[i].symbol, tmp) == 0) {
						break;
					}	//�ߺ��� �ƴ϶��
				}
				if (i == literal_line) {
					strcpy(lit_table[literal_line++].symbol, tmp);
				}	//���ͷ� ���̺��� �������� �־���
			}
		}
		else {
			token_table[token_line]->operand[0] = NULL;
			token_table[token_line]->operand[1] = NULL;
			token_table[token_line]->operand[2] = NULL;	//operand�κ�
		}	//operand�� ���ٸ� NULL�� �ʱ�ȭ

		if (strcmp(token_table[token_line]->operator,"RESW") == 0) {
			int i = atoi(token_table[token_line]->operand[0]);
			locctr += i * 3;
		}	//RESW�� ��� i*3��ŭ locctr����
		else if (strcmp(token_table[token_line]->operator,"RESB") == 0) {
			int i = atoi(token_table[token_line]->operand[0]);
			locctr += i;
		}	//RESB�� ��� i��ŭ locctr����
		else if (strcmp(token_table[token_line]->operator, "WORD") == 0) {
			locctr += 3;
		}	//WORD�� ��� 3��ŭ locctr����
		else if (strcmp(token_table[token_line]->operator, "BYTE") == 0) {
			if (token_table[token_line]->operand[0][0] == 'C') {
				locctr += strlen(token_table[token_line]->operand[0]) - 3;
			}	//BYTE�϶� C�� ��� C''�� ������ ������ ���� ��ŭ locctr����
			else if (token_table[token_line]->operand[0][0] == 'X'){
				locctr += (strlen(token_table[token_line]->operand[0]) - 3) / 2;
			}	//X�� ��� X''�� ������ ������ ���� / 2 ��ŭ locctr����
		}

		tmp = strtok(NULL, "\t\n");
		char buffer[10] = { 0 };
		itoa(locTmp, buffer, 10);
		token_table[token_line]->comment = (char*)malloc(strlen(buffer) + 1);
		strcpy(token_table[token_line]->comment, buffer);	//comment�κп� locctr�� �־���

		token_line++;	//���� ����

		if (strcmp(token_table[token_line - 1]->operator, "LTORG") == 0 || strcmp(token_table[token_line - 1]->operator, "END") == 0) {
			for (int i = pre_line; i < literal_line; i++) {	//���ͷ��̳� END�ϰ��
				token_table[token_line] = (token*)malloc(sizeof(token));
				token_table[token_line]->label = (char*)malloc(4);
				strcpy(token_table[token_line]->label, "*");
				token_table[token_line]->operator = (char*)malloc(strlen(lit_table[i].symbol) + 1);
				token_table[token_line]->operator = lit_table[i].symbol;
				token_table[token_line]->operand[0] = NULL;
				token_table[token_line]->operand[1] = NULL;
				token_table[token_line]->operand[2] = NULL;
				//���ͷ� ���̺�κ��� ���ͷ� ���� �ҷ��� ����
				char tmp[20];

				if (lit_table[i].symbol[1] == 'C') {	//C�ϰ��
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
				}	//���˿� �°� ���� �� �־���
				else if (lit_table[i].symbol[1] == 'X') {	//X�ϰ��
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
				}	//���˿� �°� ���� �� �־���
				itoa(lit_table[i].addr, tmp, 10);
				token_table[token_line]->comment = (char*)malloc(strlen(tmp) + 1);
				strcpy(token_table[token_line]->comment, tmp);	//comment�κп� locctr�� �־���
				token_line++;	//���� ����
			}
		}
	}
	else {		//���� �� �ִ°��
		char *tmp;
		tmp = strtok(input_data[index], "\t\n");
		token_table[token_line]->label = (char*)malloc(strlen(tmp) + 1);
		strcpy(token_table[token_line]->label, tmp);
		sym_table[label_num].addr = locctr;
		strcpy(sym_table[label_num++].symbol, tmp);	//���̺� �κ�
		

		tmp = strtok(NULL, "\t\n");
		token_table[token_line]->operator = (char*)malloc(strlen(tmp) + 1);
		strcpy(token_table[token_line]->operator, tmp);
		locTmp = locctr;
		if (strcmp(tmp, "CSECT") == 0) {	//�ٸ� CS�ϰ��
			locTmp = 0;
			locctr = 0;
			pre_line = literal_line;
		}	//�ʱ�ȭ
		if (tmp[0] == '+') {	//4�����ϰ��
			locctr += 4;
		}
		int num_inst = search_opcode(token_table[token_line]->operator);	//inst�� �ִ��� Ȯ���Ͽ� �߸��������� ���������̴�.
		if (num_inst != -1) {
			if (strcmp(inst[num_inst][1], "2") == 0) {
				locctr += 2;
			}	//2�����ϰ��
			else {
				locctr += 3;
			}	//3���� �ϰ��
		}	//opcode�κ�

		tmp = strtok(NULL, "\t\n");
		if (tmp != NULL) {
			int i = 0;
			char* tmp1 = strtok(tmp, ",");
			token_table[token_line]->operand[i] = (char*)malloc(strlen(tmp1) + 1);
			strcpy(token_table[token_line]->operand[i++], tmp1);
			while ((tmp1 = strtok(NULL, ",")) != NULL) {
				token_table[token_line]->operand[i] = (char*)malloc(strlen(tmp1) + 1);
				strcpy(token_table[token_line]->operand[i++], tmp1);
			}	//��ū ������ŭ operand�� �־���
			for (i; i < 3; i++) {
				token_table[token_line]->operand[i] = NULL;
			}	//������ operand�� NULL�� �ʱ�ȭ
			if (tmp[0] == '=') {	//���ͷ� �ϰ��
				int i;
				for (i = pre_line; i < literal_line; i++) {
					if (strcmp(lit_table[i].symbol, tmp) == 0) {
						break;
					}
				}	//���ͷ��� �ߺ��� ���� ���ؼ�
				if (i == literal_line) {
					strcpy(lit_table[literal_line++].symbol, tmp);
				}	//���ͷ� ���̺��� �������� �־���
			}
		}
		else
		{
			token_table[token_line]->operand[0] = NULL;
		}
		token_table[token_line]->operand[1] = NULL;
		token_table[token_line]->operand[2] = NULL;		//operand�κ�

		if (strcmp(token_table[token_line]->operator,"RESW") == 0) {
			int i = atoi(token_table[token_line]->operand[0]);
			locctr += i * 3;
		}	//RESW�� ��� i*3��ŭ locctr����
		else if (strcmp(token_table[token_line]->operator,"RESB") == 0) {
			int i = atoi(token_table[token_line]->operand[0]);
			locctr += i;
		}	//RESB�� ��� i��ŭ locctr����
		else if (strcmp(token_table[token_line]->operator, "WORD") == 0) {
			locctr += 3;
		}	//WORD�� ��� 3��ŭ locctr����
		else if (strcmp(token_table[token_line]->operator, "BYTE") == 0) {
			if (token_table[token_line]->operand[0][0] == 'C') {
				locctr += strlen(token_table[token_line]->operand[0]) - 3;
			}	//BYTE�϶� C�� ��� C''�� ������ ������ ���� ��ŭ locctr����
			else if (token_table[token_line]->operand[0][0] == 'X'){
				locctr += (strlen(token_table[token_line]->operand[0]) - 3) / 2;
			}	//X�� ��� X''�� ������ ������ ���� / 2 ��ŭ locctr����
		}

		tmp = strtok(NULL, "\t\n");
		char buffer[10] = { 0 };
		itoa(locTmp, buffer, 10);
		token_table[token_line]->comment = (char*)malloc(strlen(buffer) + 1);
		sprintf(token_table[token_line]->comment, "%s", buffer);
		//comment�� Location Count���� �־���
		token_line++;
	}
	return 0;
}
/* -----------------------------------------------------------------------------------
* ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�.
* �Ű� : ��ū ������ ���е� ���ڿ�
* ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
* ���� :
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
		// ������̺� �ʿ��� ��ü ���� ǥ����
		if (strcmp(token_table[i]->operator, "START") == 0) {	//START�ϰ��
			int itmp1 = atoi(token_table[i]->comment);
			int j=0;
			for (j = i; j < token_line; j++) {
				if (strcmp(token_table[j]->operator, "CSECT") == 0) {
					break;
				}	//���� CS���� ã�Ƽ�
			}
			int itmp2 = atoi(token_table[j - 1]->comment);
			fprintf(fout, "H%-6s%06X%06X\n", token_table[i]->label, itmp1, itmp2);
		}	//�ش� ���α׷��� �����ּҿ� �� �ּҸ� �����
		else if (strcmp(token_table[i]->operator, "CSECT") == 0)	//CS�ϰ��
		{
			fprintf(fout, "\n");
			for (int j = pre_index; j < i; j++) {
				if (token_table[j]->operator[0] == '+') {	//4�����϶�
					fprintf(fout, "M%06X05+%s\n", atoi(token_table[j]->comment)+1, token_table[j]->operand[0]);
				}	//��������̼� ���˿� ���缭 ���
				else if (strcmp(token_table[j]->operator, "WORD") == 0) {	//WORD�ϰ��
					char* tmp = strtok(token_table[j]->operand[0], "-");
					fprintf(fout, "M%06X06+%s\n", atoi(token_table[j]->comment), tmp);
					tmp = strtok(NULL, "-");
					fprintf(fout, "M%06X06-%s\n", atoi(token_table[j]->comment), tmp);
				}	//��������̼� ���˿� ���缭 ���
			}
			pre_index = i;	//�� ���α׷� �������� �˱� ���� ����
			int itmp1 = atoi(token_table[i]->comment);
			int j = 0;
			for (j = i+1; j < token_line; j++) {
				if (strcmp(token_table[j]->operator, "CSECT") == 0) {
					break;
				}
			}	//���� CS�� ã�Ƽ�
			int itmp2;
			if (strcmp(token_table[j - 1]->operator, "EQU") == 0) {	//EQU�� ���
				itmp2 = atoi(token_table[j - 1]->comment);
			}	//�״�� �־���
			else if (strcmp(token_table[j-2]->operator, "END") == 0) {	//END�� ���
				itmp2 = atoi(token_table[j - 2]->comment) + (strlen(token_table[j-1]->operator)-4)/2;
			}	//������ �ּҰ��� ������
			else {
				itmp2 = atoi(token_table[j - 1]->comment) + 3;
			}	//�ּҰ��� +3�� ������
			fprintf(fout, "E\n\nH%-6s%06X%06X\n", token_table[i]->label, itmp1, itmp2);
		}	//�ش� ���α׷��� �����ּҿ� �� �ּҸ� �����
		else if (strcmp(token_table[i]->operator, "END") == 0)
		{
			fprintf(fout, "E\n");
		}	//END�� ��� E�� �Բ� ������ ����
		else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {	//REF�κ�
			fprintf(fout, "R");
			for (int j = 0; j < 3; j++) {
				if (token_table[i]->operand[j] == NULL) {
					break;
				}
				fprintf(fout, "%-6s", token_table[i]->operand[j]);
			}
			fprintf(fout, "\n");
		}	//�ش� operand���� ��ŭ ���˿� ���� ���
		else if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {	//DEF�κ�
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
		}	//�ش� operand���� ��ŭ ���˿� ���� ��°� �Բ� �ش� �ּҵ� ���

		else if (strcmp(token_table[i]->operator, "RESW") == 0) {	//RESW�� ���
			fprintf(fout, "\n");	//������ ���ְ�
			for (i; i < token_line; i++) {
				if (strcmp(token_table[i]->operator, "RESW") != 0) {
					break;
				}
			}	//���� RESW�� �ƴҶ� ���� i++
			continue;
		}
		else if (strcmp(token_table[i]->operator, "EQU") == 0) {
			continue;	//���α׷� ���
		}
		else if (strcmp(token_table[i]->operator, "RESB") == 0) {
			continue;	// ���α׷� ���
		}
		else if (strcmp(token_table[i]->operator, "LTORG") == 0) {
			continue;	//���α׷� ���
		}
		else
		{
			char tmp[100] = "";	//buffer
			int len = 0;	//����
			fprintf(fout, "T%06X", atoi(token_table[i]->comment));
			while (i< token_line && token_table[i]->hexacode != NULL && len + strlen(token_table[i]->hexacode) <= 60)
			{
				if (strcmp(token_table[i]->operator, "CSECT") == 0 || strcmp(token_table[i]->operator, "RESW") == 0)
					break;	//�ش� ���� ������ break!
				if (token_table[i]->operator[0] == '+') {	//4���� �ϰ��
					len += 8;
				}
				else if (token_table[i]->operator[strlen(token_table[i]->operator)-1] == 'R'){	//���۷������� ������ ����
					len += 4;
				}	//2�����ϰ��
				else if (strcmp(token_table[i]->operator, "BYTE") == 0) {	//BYTE�� ���
					len += 2;
				}
				else if (token_table[i]->operator[0] == '=') {	//���ͷ� �ϰ��
					if (token_table[i]->operator[1] == 'C') {
						len += (strlen(token_table[i]->operator) - 4)*2;
					}	//C�� �ش� ���� *2 ��ŭ 
					else if (token_table[i]->operator[1] == 'X') {
						len += (strlen(token_table[i]->operator) - 4);
					}	//X�� �ش� ���� ��ŭ 
				}
				else {	//�� ���� ���
					len += 6;
				}
				if (token_table[i]->hexacode != NULL) {
					int itmp = atoi(token_table[i]->hexacode);
					char ctmp[10];
					if (token_table[i]->operator[0] == '+') {	//4�����ϰ��
						sprintf(ctmp, "%08X", itmp);
					}
					else if (token_table[i]->operator[strlen(token_table[i]->operator) - 1] == 'R') {
						sprintf(ctmp, "%04X", itmp);
					}	//2�����ϰ��
					else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
						sprintf(ctmp, "%02X", itmp);
					}	//BYTE�ϰ��
					else {	//�� ���� ���
						sprintf(ctmp, "%06X", itmp);
					}
					strcat(tmp, ctmp);	//tmp�� �̾ �־���
					i++;	//���� ����

					if (i < token_line && strcmp(token_table[i]->operator, "END") == 0) {
						itmp = atoi(token_table[i+1]->hexacode);
						sprintf(ctmp, "%02X", itmp);
						strcat(tmp, ctmp);
						break;
					}	//END�ϰ�� ���ͷ���ŭ ������
				}	
			}
			fprintf(fout, "%02X%s", strlen(tmp)/2, tmp);	//���̿� ����� HEXACODE���
			if (i < token_line && token_table[i]->hexacode != NULL ) {	//HEXACODE�� �ƴϸ�
				fprintf(fout, "\n");
				i--;	//�ݺ��� �������� ������ ���� �ѹ� ����
			}
			if (i < token_line && strcmp(token_table[i]->operator, "CSECT") == 0) {	//CSECT��
				i--;	//�ݺ��� �������� ������ ���� �ѹ� ����
			}
		}
		if (i==token_line)	//������ ������ ���
		{
			fprintf(fout, "\n");
			for (int j = pre_index; j < i; j++) {
				if (token_table[j]->operator[0] == '+') {
					fprintf(fout, "M%06X05+%s\n", atoi(token_table[j]->comment) + 1, token_table[j]->operand[0]);
				}
			}	//4���Ŀ� �ش��ϴ� ��������̼��� �������
			fprintf(fout, "E");	//E�� �������
			break;
		}
	}
	fclose(fout);
}