//this C program reads machine instructions and executes it
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//various constants
#define ins_len 32
#define reg_len 5
#define lab_len 5
#define data_len 8
#define max_opcodes 25
//structure for storing various hash tables
struct mnemonic
{
    char text[20];
    unsigned int val;
};
typedef struct mnemonic mnemonic;

mnemonic hash_opcodes[50];
mnemonic hash_register[50];

unsigned int reg[18],instruction[50];
int machine_code_len=0,num_op=0,num_reg=0,num_lab=0;
//program to convert a binary string to an integer value
unsigned int get_decimal(char *s)
{
    unsigned int num=0;
    int i,l=strlen(s);
    for(i=0;i<l;i++)
    {
        num+=(s[i]-'0')*(1<<(l-i-1));
    }
    return num;
}
//utility function to count the length of machine code
void count_machine_code_length()
{
    FILE *p=fopen("machine.txt","r");
    char s[50];
    if(p==NULL)
    {
        printf("Machine code not found\n");
        return;
    }
    while(fscanf(p,"%s",s)!=EOF){machine_code_len++;}
    fclose(p);
}
//read and convert binary code to integer and store into instruction table
void store_machine_code()
{
    count_machine_code_length();
    FILE *p=fopen("machine.txt","r");
    char s[50];
    if(p==NULL)
    {
        return;
    }
    int i;
    for(i=0;i<machine_code_len;i++)
    {
        fscanf(p,"%s",s);
        instruction[i]=get_decimal(s);
    }
    fclose(p);
}
//generate hash table of opcodes
void gen_hash_table_opcodes()
{
    FILE *op=fopen("opcodes.txt","r");
    char s[50];
    if(op==NULL)
    {
        printf("Opcode Table not found\n");
    }
    else
    {
        while(fscanf(op,"%s %s",hash_opcodes[num_op].text,s)!=EOF)
        {
            hash_opcodes[num_op].val=get_decimal(s);
            num_op++;
        }
    }
    fclose(op);
}
//print hash table of opcodes
void print_opcode_hash_table()
{
    int i=0;
    for(i=0;i<num_op;i++)
    {
        printf("%s %u\n",hash_opcodes[i].text,hash_opcodes[i].val);
    }
}
//generate hash table of registers
void gen_hash_table_regsiters()
{
    FILE *op=fopen("registers.txt","r");
    char s[50];
    if(op==NULL)
    {
        printf("Register Table not found\n");
    }
    else
    {
        while(fscanf(op,"%s %s",hash_register[num_reg].text,s)!=EOF)
        {
            hash_register[num_reg].val=get_decimal(s);
            num_reg++;
        }
    }
    fclose(op);
}
//print hash table of registers
void print_reg_hash_table()
{
    int i=0;
    for(i=0;i<num_reg;i++)
    {
        printf("%s %d\n",hash_register[i].text,hash_register[i].val);
    }
}
//search opcode in hash table
int get_opcode(char *s,unsigned int num)
{
    int i;
    for(i=0;i<num_op;i++)
    {
        if(hash_opcodes[i].val==num)
        {
            strcpy(s,hash_opcodes[i].text);
            return 1;
        }
    }
    return 0;
}
//check if opcode is 19 bit using bit masking
int check_19_bit(unsigned int inst,char *s)
{
    unsigned int num=0;
    int i,found=0;
    for(i=13;i<32;i++)
    {
        num+=(1<<i);
    }
    inst=(inst&num);
    inst=inst>>13;
    found=get_opcode(s,inst);
    if(!found)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
//check if opcode is 22 bit using bit masking
int check_22_bit(unsigned int inst,char *s)
{
    unsigned int num=0;
    int i,found=0;
    for(i=10;i<32;i++)
    {
        num+=(1<<i);
    }
    inst=(inst&num);
    inst=inst>>10;
    found=get_opcode(s,inst);
    if(!found)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
//check if opcode is 27 bit using bit masking
int check_27_bit(unsigned int inst,char *s)
{
    unsigned int num=0;
    int i,found=0;
    for(i=5;i<32;i++)
    {
        num+=(1<<i);
    }
    inst=(inst&num);
    inst=inst>>5;
    found=get_opcode(s,inst);
    if(!found)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
//check if opcode is 32 bit using bit masking
int check_32_bit(unsigned int inst,char *s)
{
    unsigned int num=0;
    int i,found=0;
    for(i=0;i<32;i++)
    {
        num+=(1<<i);
    }
    inst=(inst&num);
    found=get_opcode(s,inst);
    if(!found)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//Various functions to perform OPCODE instructions

//NOTE:: Bit masking is used for extracting opcode and addresses. At each step the status of register, ALU, flags is displayed
//Status after each line is written in file "status.txt"

//move data to register
void MOVD(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    data=(1<<(data_len))-1;
    data=data<<reg_len;
    data=(inst&data);
    data=(data>>reg_len);
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    reg[reg_in]=data;
    if(reg[reg_in]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d\n",reg_in,reg[reg_in]);
    fprintf(op,"ALU Status: No ALU Operations Performed\n");
    fprintf(op,"Data Movement: Value %d moved to Register(0%d)\n",data,reg_in);
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//add data to register
void ADDD(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    data=(1<<(data_len))-1;
    data=data<<reg_len;
    data=(inst&data);
    data=(data>>reg_len);
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    reg[reg_in]+=data;
    if(reg[reg_in]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d\n",reg_in,reg[reg_in]);
    fprintf(op,"ALU Status: Value %d added to Register(0%d)\n",data,reg_in);
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//subtract data from register
void SUBD(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    data=(1<<(data_len))-1;
    data=data<<reg_len;
    data=(inst&data);
    data=(data>>reg_len);
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    reg[reg_in]-=data;
    if(reg[reg_in]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d\n",reg_in,reg[reg_in]);
    fprintf(op,"ALU Status: Value %d subtracted from Register(0%d)\n",data,reg_in);
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//divide data from register
void DIVD(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    data=(1<<(data_len))-1;
    data=data<<reg_len;
    data=(inst&data);
    data=(data>>reg_len);
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    reg[reg_in]/=data;
    if(reg[reg_in]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d\n",reg_in,reg[reg_in]);
    fprintf(op,"ALU Status: Value %d divided from Register(0%d)\n",data,reg_in);
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//multiply data into register
void MULD(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    data=(1<<(data_len))-1;
    data=data<<reg_len;
    data=(inst&data);
    data=(data>>reg_len);
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    reg[reg_in]*=data;
    if(reg[reg_in]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d\n",reg_in,reg[reg_in]);
    fprintf(op,"ALU Status: Value %d multiplied to Register(0%d)\n",data,reg_in);
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//compare data to a register and set flag accordingly
void CMPD(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    data=(1<<(data_len))-1;
    data=data<<reg_len;
    data=(inst&data);
    data=(data>>reg_len);
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    int cmp=reg[reg_in]-data;
    if(cmp>0)
    {
        reg[16]=0;
    }
    else if(cmp==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d\n",reg_in,reg[reg_in]);
    fprintf(op,"ALU Status: Value %d compared to Register(0%d)\n",data,reg_in);
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//logical and of data and value in register
void ANDD(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    data=(1<<(data_len))-1;
    data=data<<reg_len;
    data=(inst&data);
    data=(data>>reg_len);
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    reg[reg_in]=(reg[reg_in]&data);
    if(reg[reg_in]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d\n",reg_in,reg[reg_in]);
    fprintf(op,"ALU Status: Logical and of %d & Register(0%d) and Value stored to Register(0%d)\n",data,reg_in,reg_in);
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}

// all register addressing stores the value to second register after operation

//move value from one register to another
void MOVR(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in1=(1<<(reg_len))-1;
    reg_in1=(reg_in1<<reg_len);
    reg_in1=(inst&reg_in1);
    reg_in1=(reg_in1>>reg_len);
    reg_in2=(1<<(reg_len))-1;
    reg_in2=(inst&reg_in2);
    reg[reg_in2]=reg[reg_in1];
    if(reg[reg_in2]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in2]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d, Register(0%d)= %d\n",reg_in1,reg[reg_in1],reg_in2,reg[reg_in2]);
    fprintf(op,"ALU Status: No ALU Operations Performed\n");
    fprintf(op,"Data Movement: Value from Register(0%d) moved to Register(0%d)\n",reg_in1,reg_in2);
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//add value from one register to another
void ADDR(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in1=(1<<(reg_len))-1;
    reg_in1=(reg_in1<<reg_len);
    reg_in1=(inst&reg_in1);
    reg_in1=(reg_in1>>reg_len);
    reg_in2=(1<<(reg_len))-1;
    reg_in2=(inst&reg_in2);
    reg[reg_in2]+=reg[reg_in1];
    if(reg[reg_in2]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in2]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d, Register(0%d)= %d\n",reg_in1,reg[reg_in1],reg_in2,reg[reg_in2]);
    fprintf(op,"ALU Status: Value from Register(0%d) added to Register(0%d)\n",reg_in1,reg_in2);
    fprintf(op,"Data Movement: No Data Movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//subtract value from one register to another
void SUBR(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in1=(1<<(reg_len))-1;
    reg_in1=(reg_in1<<reg_len);
    reg_in1=(inst&reg_in1);
    reg_in1=(reg_in1>>reg_len);
    reg_in2=(1<<(reg_len))-1;
    reg_in2=(inst&reg_in2);
    reg[reg_in2]-=reg[reg_in1];
    if(reg[reg_in2]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in2]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d, Register(0%d)= %d\n",reg_in1,reg[reg_in1],reg_in2,reg[reg_in2]);
    fprintf(op,"ALU Status: Value from Register(0%d) subtracted from Register(0%d)\n",reg_in1,reg_in2);
    fprintf(op,"Data Movement: No Data Movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//divide value from one register to another
void DIVR(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in1=(1<<(reg_len))-1;
    reg_in1=(reg_in1<<reg_len);
    reg_in1=(inst&reg_in1);
    reg_in1=(reg_in1>>reg_len);
    reg_in2=(1<<(reg_len))-1;
    reg_in2=(inst&reg_in2);
    reg[reg_in2]/=reg[reg_in1];
    if(reg[reg_in2]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in2]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d, Register(0%d)= %d\n",reg_in1,reg[reg_in1],reg_in2,reg[reg_in2]);
    fprintf(op,"ALU Status: Value from Register(0%d) divided from Register(0%d)\n",reg_in1,reg_in2);
    fprintf(op,"Data Movement: No Data Movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//multiply value from one register to another
void MULR(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in1=(1<<(reg_len))-1;
    reg_in1=(reg_in1<<reg_len);
    reg_in1=(inst&reg_in1);
    reg_in1=(reg_in1>>reg_len);
    reg_in2=(1<<(reg_len))-1;
    reg_in2=(inst&reg_in2);
    reg[reg_in2]*=reg[reg_in1];
    if(reg[reg_in2]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in2]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d, Register(0%d)= %d\n",reg_in1,reg[reg_in1],reg_in2,reg[reg_in2]);
    fprintf(op,"ALU Status: Value from Register(0%d) multiplied into Register(0%d)\n",reg_in1,reg_in2);
    fprintf(op,"Data Movement: No Data Movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//compare value in both registers and set flags accordingly
void CMPR(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in1=(1<<(reg_len))-1;
    reg_in1=(reg_in1<<reg_len);
    reg_in1=(inst&reg_in1);
    reg_in1=(reg_in1>>reg_len);
    reg_in2=(1<<(reg_len))-1;
    reg_in2=(inst&reg_in2);
    int cmp=reg[reg_in2]-reg[reg_in1];
    if(cmp>0)
    {
        reg[16]=0;
    }
    else if(cmp==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d, Register(0%d)= %d\n",reg_in1,reg[reg_in1],reg_in2,reg[reg_in2]);
    fprintf(op,"ALU Status: Value of Register(0%d) and Register(0%d) compared\n",reg_in1,reg_in2);
    fprintf(op,"Data Movement: No Data Movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//logical AND of values in both register
void ANDR(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in1=(1<<(reg_len))-1;
    reg_in1=(reg_in1<<reg_len);
    reg_in1=(inst&reg_in1);
    reg_in1=(reg_in1>>reg_len);
    reg_in2=(1<<(reg_len))-1;
    reg_in2=(inst&reg_in2);
    reg[reg_in2]=(reg[reg_in2]&reg[reg_in1]);
    if(reg[reg_in2]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in2]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d, Register(0%d)= %d\n",reg_in1,reg[reg_in1],reg_in2,reg[reg_in2]);
    fprintf(op,"ALU Status: Logical and of Register(0%d) & Register(0%d) and Value stored to Register(0%d)\n",reg_in1,reg_in2,reg_in2);
    fprintf(op,"Data Movement: No Data Movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//increment value in register
void INC(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    reg[reg_in]+=1;
    if(reg[reg_in]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d\n",reg_in,reg[reg_in]);
    fprintf(op,"ALU Status: Value of Register(0%d) incremented\n",reg_in);
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");

}
//decrement value in register
void DEC(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    reg[reg_in]-=1;
    if(reg[reg_in]>0)
    {
        reg[16]=0;
    }
    else if(reg[reg_in]==0)
    {
        reg[16]=1;
    }
    else
    {
        reg[16]=(1<<1);
    }
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"Register Status: Register(0%d)= %d\n",reg_in,reg[reg_in]);
    fprintf(op,"ALU Status: Value of Register(0%d) decremented\n",reg_in);
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"Flag Status:\n");
    if(reg[16]==0)
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Unset\n");
    }
    else if(reg[16]==1)
    {
        fprintf(op,"Zero Flag : Set,   Negative Flag : Unset\n");
    }
    else
    {
        fprintf(op,"Zero Flag : Unset,   Negative Flag : Set\n");
    }
    fprintf(op,"\n\n");
}
//unconditional jump
void JMP(unsigned inst,int *i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    (*i)=reg_in-1;
    fprintf(op,"Line %d\n",(*i)+1);
    fprintf(op,"ALU Status: No ALU operation performed\n");
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"Control Flow: Jumped to line: %d\n",reg_in);
    fprintf(op,"\n\n");
}
//jump if zero flag is set
void JZR(unsigned inst,int *i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    if(reg[16]==1)
        (*i)=reg_in-1;
    else
        (*i)++;
    fprintf(op,"Line %d\n",(*i)+1);
    fprintf(op,"ALU Status: No ALU operation performed\n");
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    if(reg[16]==1)
        fprintf(op,"Control Flow: Jumped to line: %d\n",reg_in);
    else
        fprintf(op,"Control Flow: No Jump operation\n");
    fprintf(op,"\n\n");
}
//jump if zero flag is not set
void JNZ(unsigned inst,int *i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    if(reg[16]==0)
        (*i)=reg_in-1;
    else
        (*i)++;
    fprintf(op,"Line %d\n",(*i)+1);
    fprintf(op,"ALU Status: No ALU operation performed\n");
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    if(reg[16]==0)
        fprintf(op,"Control Flow: Jumped to line: %d\n",reg_in);
    else
        fprintf(op,"Control Flow: No Jump operation\n");
    fprintf(op,"\n\n");
}

//now these jump statements require a comparison

//jump is equal
void JEQ(unsigned inst,int *i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    if(reg[16]==1)
        (*i)=reg_in-1;
    else
        (*i)++;
    fprintf(op,"Line %d\n",(*i)+1);
    fprintf(op,"ALU Status: No ALU operation performed\n");
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    if(reg[16]==1)
        fprintf(op,"Control Flow: Jumped to line: %d\n",reg_in);
    else
        fprintf(op,"Control Flow: No Jump operation\n");
    fprintf(op,"\n\n");
}
//jump if not equal
void JNE(unsigned inst,int *i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    if(reg[16]!=1)
        (*i)=reg_in-1;
    else
        (*i)++;
    fprintf(op,"Line %d\n",(*i)+1);
    fprintf(op,"ALU Status: No ALU operation performed\n");
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    if(reg[16]!=1)
        fprintf(op,"Control Flow: Jumped to line: %d\n",reg_in);
    else
        fprintf(op,"Control Flow: No Jump operation\n");
    fprintf(op,"\n\n");
}
//jump if greater than
void JGT(unsigned inst,int *i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    if(reg[16]==0)
        (*i)=reg_in-1;
    else
        (*i)++;
    fprintf(op,"Line %d\n",(*i)+1);
    fprintf(op,"ALU Status: No ALU operation performed\n");
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    if(reg[16]!=1)
        fprintf(op,"Control Flow: Jumped to line: %d\n",reg_in);
    else
        fprintf(op,"Control Flow: No Jump operation\n");
    fprintf(op,"\n\n");
}
//jump if less than
void JLT(unsigned inst,int *i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    if(reg[16]==2)
       (*i)=reg_in-1;
    else
        (*i)++;
    fprintf(op,"Line %d\n",(*i)+1);
    fprintf(op,"ALU Status: No ALU operation performed\n");
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    if(reg[16]!=1)
        fprintf(op,"Control Flow: Jumped to line: %d\n",reg_in);
    else
        fprintf(op,"Control Flow: No Jump operation\n");
    fprintf(op,"\n\n");
}

//I/O

//take input and store to stated register
void INP(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    printf("Input: ");
    scanf("%d",&reg[reg_in]);
    fprintf(op,"Line %d\n",(i)+1);
    fprintf(op,"ALU Status: No ALU operation performed\n");
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"I/O: Value of %d taken from input stored to Register(%d)\n",reg[reg_in],reg_in);
    fprintf(op,"\n\n");
}
//print output from given register
void OUT(unsigned inst,int i,FILE *op)
{
    unsigned int data,reg_in,reg_in1,reg_in2;
    reg_in=(1<<(reg_len))-1;
    reg_in=(inst&reg_in);
    printf("Output: %d\n",reg[reg_in]);
    fprintf(op,"Line %d\n",i+1);
    fprintf(op,"ALU Status: No ALU operation performed\n");
    fprintf(op,"Data Movement: No Data movement operation performed\n");
    fprintf(op,"I/O: Output %d from Register(%d)\n",reg[reg_in],reg_in);
    fprintf(op,"\n\n");
}

//function to interpret and execute machine code
void solve_machine_code()
{
    FILE *op=fopen("status.txt","w");
    char s[50];
    int i=0;
    unsigned int inst;
    while(i<machine_code_len)
    {
        inst=instruction[i];
        if(check_19_bit(inst,s))
        {
            if(!strcmp(s,"MOVD"))
            {
                MOVD(inst,i,op);
            }
            else if(!strcmp(s,"ADDD"))
            {
                ANDD(inst,i,op);
            }
            else if(!strcmp(s,"SUBD"))
            {
                SUBD(inst,i,op);
            }
            else if(!strcmp(s,"DIVD"))
            {
                DIVD(inst,i,op);
            }
            else if(!strcmp(s,"MULD"))
            {
                MULD(inst,i,op);
            }
            else if(!strcmp(s,"CMPD"))
            {
                CMPD(inst,i,op);
            }
            else if(!strcmp(s,"ANDD"))
            {
                ANDD(inst,i,op);
            }
            i++;
        }
        else if(check_22_bit(inst,s))
        {
            if(!strcmp(s,"MOVR"))
            {
                MOVR(inst,i,op);
            }
            else if(!strcmp(s,"ADDR"))
            {
                ADDR(inst,i,op);
            }
            else if(!strcmp(s,"SUBR"))
            {
                SUBR(inst,i,op);
            }
            else if(!strcmp(s,"DIVR"))
            {
                DIVR(inst,i,op);
            }
            else if(!strcmp(s,"MULR"))
            {
                MULR(inst,i,op);
            }
            else if(!strcmp(s,"CMPR"))
            {
                CMPR(inst,i,op);
            }
            else if(!strcmp(s,"ANDR"))
            {
                ANDR(inst,i,op);
            }
            i++;
        }
        else if(check_27_bit(inst,s))
        {
            if(!strcmp(s,"INC"))
            {
                INC(inst,i,op);
                i++;
            }
            else if(!strcmp(s,"DEC"))
            {
                DEC(inst,i,op);
                i++;
            }
            else if(!strcmp(s,"JMP"))
            {
                JMP(inst,&i,op);
            }
            else if(!strcmp(s,"JZR"))
            {
                JZR(inst,&i,op);
            }
            else if(!strcmp(s,"JNZ"))
            {
                JNZ(inst,&i,op);
            }
            else if(!strcmp(s,"JEQ"))
            {
                JEQ(inst,&i,op);
            }
            else if(!strcmp(s,"JNE"))
            {
                JNE(inst,&i,op);
            }
            else if(!strcmp(s,"JGT"))
            {
                JGT(inst,&i,op);
            }
            else if(!strcmp(s,"JLT"))
            {
                JLT(inst,&i,op);
            }
            else if(!strcmp(s,"INP"))
            {
                INP(inst,i,op);
                i++;
            }
            else if(!strcmp(s,"OUT"))
            {
                OUT(inst,i,op);
                i++;
            }
        }
        else if(check_32_bit(inst,s))
        {
            if(!strcmp(s,"HLT"))
            {
                return;
            }
            i++;
        }
        else
        {
            printf("Error: Unrecognized Instruction Found!\n");
            i++;
        }
    }
    fclose(op);
}
void print_status_file()
{
    FILE *p=fopen("status.txt","r");
    char s[50];
    while(fgets(s,50,p)!=NULL){puts(s);}
    fclose(p);
}
int main()
{
    //generate hash table for opcodes and registers
    gen_hash_table_opcodes();
    gen_hash_table_regsiters();
    //read the machine code to be executed
    store_machine_code();
    //execute the machine code
    int choice;
    while(choice!=3)
    {
        printf("**********Menu*************\n");
        printf("1.Execute the machine code\n");
        printf("2.Print Status of step wise execution\n");
        printf("3.Exit\n");
        scanf("%d",&choice);
        switch(choice)
        {
        case 1:
            {
                //executes the code
                solve_machine_code();
                break;
            }
        case 2:
            {
                //print "status.txt"
                print_status_file();
                break;
            }
        }
    }

    return 0;
}
