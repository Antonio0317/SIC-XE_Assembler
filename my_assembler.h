/*
* my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
*
*/
#define MAX_INST 256
#define MAX_LINES 5000

#define MAX_COLUMNS 4
#define MAX_OPERAND 3

/*
* ���� ��� ���Ϸ� ���� ������ �޾ƿ� �����ϴ� ���� ��ȯ ���̺��̴�.
* �ش� ������ ������ ��ū���� ���� ����ϰ� �ִ�.
*/
char *inst[MAX_INST][MAX_COLUMNS];
int inst_index;

/*
 * ����� �� �ҽ��ڵ带 ��ū ������ �����ϴ� ���̺��̴�.
 * ���� ������ �ҽ� ���� ������ �����Ǿ�����.
*/
char *input_data[MAX_LINES];
static int line_num;

int label_num;

struct token_unit {
	char *label;
	char *operator;
	char *operand[MAX_OPERAND];
	char *comment;
	char *hexacode;
};

typedef struct token_unit token;
token *token_table[MAX_LINES];

static int token_line;

/*
* �ɺ��� �����ϴ� ����ü�̴�.
* �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
*/
struct symbol_unit {
	char symbol[10];
	int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
symbol lit_table[10];
static int literal_line;
static int pre_line;

static int locctr;
//--------------

static char *input_file;
static char *output_file;

int init_my_assembler(void);
static int assem_pass1(void);
static int assem_pass2(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int search_opcode(char *str);
void make_objectcode(char *file_name);