//This Programs Convert Given Assembly code into binary
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//different constants
#define ins_len 32
#define reg_len 5
#define lab_len 5
#define data_len 8
#define max_opcodes 25
//structure for storing various hash tables
struct mnemonic
{
    char text[20];
    char binary[50];
};
typedef struct mnemonic mnemonic;

mnemonic hash_opcodes[50];
mnemonic hash_register[50];
mnemonic hash_label[50];

int num_op=0,num_reg=0,num_lab=0,inst_len=0;
char code[10];
//function to generate hash table for opcodes
void gen_hash_table_opcodes()
{
    FILE *op=fopen("opcodes.txt","r");
    if(op==NULL)
    {
        printf("Opcode Table not found\n");
    }
    else
    {
        while(fscanf(op,"%s %s",hash_opcodes[num_op].text,hash_opcodes[num_op].binary)!=EOF){num_op++;}
    }
    fclose(op);
}
//function to generate hash table for registers
void gen_hash_table_regsiters()
{
    FILE *op=fopen("registers.txt","r");
    if(op==NULL)
    {
        printf("Register Table not found\n");
    }
    else
    {
        while(fscanf(op,"%s %s",hash_register[num_reg].text,hash_register[num_reg].binary)!=EOF){num_reg++;}
    }
    fclose(op);
}
//function to reverse a string
void strr(char *s)
{
    int i,j,l=strlen(s);
    char c;
    for(i=0,j=l-1;i<j;i++,j--)
    {
        c=s[i];
        s[i]=s[j];
        s[j]=c;
    }
}
//convert a decimal number into a binary string of length l
void binary(int num,int len,char *mem)
{
    int i;
    for(i=0;i<len;i++)
    {
        mem[i]=num%2+'0';
        num>>=1;
    }
    mem[i]='\0';
    strr(mem);
}
//this is pass 1, all the labels are looked for and jump statements are assigned proper Instruction Locations
void take_care_of_labels()
{
    int i;
    FILE *as=fopen(code,"r");
    FILE *lb=fopen("labels.txt","w");
    char s[50],lab[10],mem[10];
    int line_cnt=0,lab_cnt=0;
    while(fgets(s,50,as)!=NULL)
    {
        s[strlen(s)-1]='\0';
        line_cnt++;
        if(s[0]=='L')
        {
            lab_cnt++;
            lab[0]=s[0];
            lab[1]=s[1];
            if(s[2]==':')
            {
                lab[2]='\0';
            }
            else
            {
                lab[2]=s[2];
                lab[2]='\0';
            }
            binary(line_cnt-(lab_cnt-1),lab_len,mem);
            fprintf(lb,"%s %s\n",lab,mem);
        }
    }
    fclose(as);
    fclose(lb);
}
//function to generate hash table for labels
void gen_hash_table_labels()
{
    num_lab=0;
    FILE *op=fopen("labels.txt","r");
    if(op==NULL)
    {
        printf("Label Table not found\n");
    }
    else
    {
        while(fscanf(op,"%s %s",hash_label[num_lab].text,hash_label[num_lab].binary)!=EOF){num_lab++;}
    }
    fclose(op);
}
//print functions :

void print_opcode_hash_table()
{
    int i=0;
    for(i=0;i<num_op;i++)
    {
        printf("%s %s\n",hash_opcodes[i].text,hash_opcodes[i].binary);
    }
}

void print_reg_hash_table()
{
    int i=0;
    for(i=0;i<num_reg;i++)
    {
        printf("%s %s\n",hash_register[i].text,hash_register[i].binary);
    }
}

void print_label_hash_table()
{
    int i=0;
    for(i=0;i<num_lab;i++)
    {
        printf("%s %s\n",hash_label[i].text,hash_label[i].binary);
    }
}

void print_assembly_code()
{
    FILE *as=fopen(code,"r");
    char s[50];
    while(fgets(s,50,as)!=NULL)
    {
        printf("%s",s);
    }
    fclose(as);
}

void print_machine_code()
{
    FILE *as=fopen("machine.txt","r");
    char s[50];
    while(fgets(s,50,as)!=NULL)
    {
        printf("%s",s);
    }
    fclose(as);
}
//function to convert a number in form of string to binary string
void get_binary(char *s,int l,int h,char *mem)
{
    int num=0,i;
    for(i=l;i<h;i++)
    {
        num=num*10+(s[i]-'0');
    }
    for(i=0;i<data_len;i++)
    {
        mem[i]=num%2+'0';
        num>>=1;
    }
    mem[i]='\0';
    strr(mem);
}
//utility function to generate machine code
void machine_utility(char *s,int l,int h,int *flag,int in_c,FILE *out)
{
    char mem[50];
    int i;
    if(in_c==1)
    {
        if(s[h-1]=='D')//Immediate addressing mode found, set flag to 1
        {
            (*flag)=1;
        }
        for(i=l;i<h;i++)
        {
            mem[i]=s[i];
        }
        mem[i]='\0';
        for(i=0;i<num_op;i++)
        {
            if(!strcmp(hash_opcodes[i].text,mem))
            {
                fprintf(out,"%s",hash_opcodes[i].binary);
                return;
            }
        }
    }
    else
    {
        if(*flag==1&&in_c==2)//constant value
        {
            get_binary(s,l,h,mem);
            fprintf(out,"%s",mem);
            return;
        }
        else if(in_c==2)//registers
        {
            for(i=l;i<h;i++)
            {
                mem[i-l]=s[i];
            }
            mem[i-l]='\0';
            for(i=0;i<num_reg;i++)
            {
                if(!strcmp(hash_register[i].text,mem))
                {
                    fprintf(out,"%s",hash_register[i].binary);
                    return;
                }
            }
            for(i=0;i<num_lab;i++)//if its not in register then its label
            {
                if(!strcmp(hash_label[i].text,mem))
                {
                    fprintf(out,"%s",hash_label[i].binary);
                    return;
                }
            }
        }
        else if(in_c==3)//3rd part of instruction if exists is always register
        {
            for(i=l;i<h;i++)
            {
                mem[i-l]=s[i];
            }
            mem[i-l]='\0';
            for(i=0;i<num_reg;i++)
            {
                if(!strcmp(hash_register[i].text,mem))
                {
                    fprintf(out,"%s",hash_register[i].binary);
                    return;
                }
            }
        }
    }
}
//main function to convert assembly code to machine code
void convert_assembly_to_machine()
{
    FILE *as=fopen(code,"r");
    FILE *out=fopen("machine.txt","w");
    char s[50];
    int i,j,flag,in_c;
    if(as==NULL)
    {
        printf("Assembly Code not found\n");
        return;
    }
    while(fgets(s,50,as)!=NULL)
    {
        s[strlen(s)-1]='\0';
        j=0;
        in_c=0;
        flag=0;
        int z=0;
        int fl=0;
        for(i=0;i<=strlen(s);i++)
        {
            if(s[i]==' '||s[i]=='\0')
            {

                in_c++;
               if(in_c==1&&s[j]=='L')
               {
                   fl=1;
                   break;
               }
                machine_utility(s,j,i,&flag,in_c,out);
                j=i+1;
            }

        }
       if(!fl)
            fprintf(out,"\n");
    }
    fclose(as);
    fclose(out);
    printf("Successfully Converted to Machine instruction\n");
}

int main()
{
    int choice=0;
    //generate hash tables from "opcodes.txt" and "registers.txt"
    gen_hash_table_opcodes();
    gen_hash_table_regsiters();
    //Menu
    while(choice!=7)
    {
        printf("**********Menu*************\n");
        printf("1.Print Opcode Table\n");
        printf("2.Print Register Table\n");
        printf("3.Convert Assembly to binary\n");
        printf("4.Print Label Table\n");
        printf("5.Print Assembly Code\n");
        printf("6.Print Machine Code\n");
        printf("7.Exit\n");

        scanf("%d",&choice);
        printf("\n");
        switch(choice)
        {
        case 1:
            {
                print_opcode_hash_table();
                break;
            }
        case 2:
            {
                print_reg_hash_table();
                break;
            }
        case 3:
            {
                printf("Enter the file name\n");
                scanf("%s",code);
                take_care_of_labels();//this is pass 1
                gen_hash_table_labels();
                convert_assembly_to_machine();//pass 2
                break;
            }
        case 4:
            {
                print_label_hash_table();
                break;
            }
        case 5:
            {
                print_assembly_code();
                break;
            }
        case 6:
            {
                print_machine_code();
                break;
            }
        }
        printf("\n\n");
    }
    return 0;
}
