#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <limits.h>
#include <windows.h>
#include <string.h>
#include <conio.h>
#define CommandLength 300
#define CommandWidth 600
#define CommandNumLimit 100000 // to avoid infinite loop
#define UpdatePeriod 3
#define FILENAME "Nolabel.bf" // the sourse of the command
#define COMMAND "BefungeOut" // the name of the output
#define SCREEN "screen" // the name of the screen file

unsigned short int CommandAry[CommandLength][CommandWidth];

enum dir{right, down, left, up};
const int dx[4] = {1, 0, -1, 0};
const int dy[4] = {0, 1, 0, -1};

typedef struct list{
    int value;
    struct list *next;
} List;

typedef struct stack{
    struct list *top; // top points the toppest element in the stack
} Stack;

List *genNode(int value, List *next){
    List *newNode = (List *)malloc(sizeof(List));
    assert(newNode != NULL);
    newNode->value = value;
    newNode->next = next;
    return newNode;
}

Stack *initialStack(void){
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    assert(stack != NULL);
    stack->top = NULL;
    return stack;
}

bool isempty(Stack *stack){
    return stack->top == NULL;
}

int pop(Stack *stack){
    if(isempty(stack)){
        printf("failed to pop from the stack\n");
        exit(-1);
    }
    int out = stack->top->value;
    List *pre = stack->top;
    stack->top = stack->top->next;
    free(pre);
    return out;
}

void push(Stack *stack, int value){
    stack->top = genNode(value, stack->top);
    return;
}

void LoadCommand(void){
    FILE *fp = fopen(FILENAME, "r");
    if(fp == NULL){
        printf("failed to open the file\n");
        exit(-1);
    }
    int i = 0, j = 0;
    while(i < CommandLength && j < CommandWidth){
        int c = fgetc(fp);
        if(c == EOF){
            fclose(fp);
            return;
        }
        if(c == '\n'){
            CommandAry[i][j] = c;
            i++;
            j = 0;
        }
        else{
            CommandAry[i][j] = c;
            j++;
        }
    }
    printf("the command size is too large to store\n");
    exit(-1);
    return;
}

bool isvalid(int y, int x){
    return y >= 0 && y < CommandLength && x >= 0 && x < CommandWidth;
}

void adjustPosition(int *y, int *x){
    if(*y < 0){
        *y += CommandLength;
        while(CommandAry[*y][*x] == 0){
            *y -= 1;
        }
    }
    else if(*y >= CommandLength){
        *y -= CommandLength;
    }
    if(*x < 0){
        *x += CommandWidth;
        while(CommandAry[*y][*x] == 0){
            *x -= 1;
        }
    }
    else if(*x >= CommandWidth){
        *x -= CommandWidth;
    }
    return;
}

void printStack(Stack *stack){
    if(isempty(stack)){
        return;
    }
    printf("stack :");
    for(List *looking = stack->top; looking != NULL; looking = looking->next){
        printf(" %d", looking->value);
    }
    printf("\n");
    return;
}

void printCommand(){
    FILE *fp = fopen(COMMAND, "w");
    assert(fp != NULL);
    for(int i = 0; i < CommandLength; i++){
        for(int j = 0; j < CommandWidth; j++){
            if(CommandAry[i][j] >= 32 && CommandAry[i][j] <= 126){
                fputc(CommandAry[i][j], fp);
            }
            else{
                fputc(' ', fp);
            }
        }
        fputc('\n', fp);
    }
    fclose(fp);
    return;
}

void KBDload(void){
    return;
}

void initialArray(void){
    for(int i = 0; i < CommandLength; i++){
        for(int j = 0; j < CommandWidth; j++){
            CommandAry[i][j] = ' ';
        }
    }
    return;
}

void initialScreen(void){
    FILE *fp = fopen(SCREEN, "w");
    assert(fp != NULL);
    for(int i = 0; i < 256; i++){
        for(int j = 0; j < 512; j++){
            fputc(' ', fp);
        }
        fputc('\n', fp);
    }
    fclose(fp);
    return;
}

void updateScreen(int index, unsigned short int c){
    assert(index >= 0 && index <= 8192);
    FILE *fp = fopen(SCREEN, "r+");
    assert(fp != NULL);
    int y = index / 32, x = index % 32 * 16;
    fseek(fp, y * (512 + 2) + x, SEEK_SET);
    for(int i = 0; i < 16; i++){
        int bit = (c >> (15 - i)) & 1;
        char pixel = 0;
        if(bit){
            pixel = '0';
        }
        else{
            pixel = ' ';
        }
        fputc(pixel, fp);
    }
    return;
}

int main(void){
    Stack *stack = initialStack();

    initialArray();
    LoadCommand();
    printCommand();

    initialScreen();

    srand(time(NULL));

    int x = 0, y = 0;
    int direction = right;
    int countChange = 0;
    bool stringMode = false, jumpMode = false;
    for(int numCommand = 0; numCommand < CommandNumLimit; numCommand++, x += dx[direction], y += dy[direction]){

        KBDload();

        if(!isvalid(y, x)){
            adjustPosition(&y, &x);
        }

        #ifdef DEBUG
        printStack(stack);
        if(CommandAry[y][x] >= 32 && CommandAry[y][x] <= 126){
            printf("position (%d, %d), command %c\n", x, y, CommandAry[y][x]);
        }
        else{
            printf("position (%d, %d)\n", x, y);
        }
        printf("direction before the command %d\n", direction);
        #endif

        if(jumpMode){
            if(isspace(CommandAry[y][x]) || CommandAry[y][x] == 0){
                continue;
            }
            jumpMode = false;
            continue;
        }
        if(stringMode){
            if(CommandAry[y][x] == '\"'){
                stringMode = false;
                continue;
            }
            push(stack, CommandAry[y][x]);
            continue;
        }
        
        int a = -1, b = -1, targetX = -1, targetY = -1, targetV = -1, inputInt = -1;
        unsigned short int inputChar = 0;
        switch(CommandAry[y][x]){
            case '>':
                direction = right;
                break;
            case '<':
                direction = left;
                break;
            case '^':
                direction = up;
                break;
            case 'v':
                direction = down;
                break;
            case '+':
                push(stack, pop(stack) + pop(stack));
                break;
            case '-':
                a = pop(stack), b = pop(stack);
                push(stack, b - a);
                break;
            case '*':
                push(stack, pop(stack) * pop(stack));
                break;
            case '/':
                a = pop(stack);
                b = pop(stack);
                assert(a != 0);
                push(stack, b / a);
                break;
            case '%':
                a = pop(stack);
                b = pop(stack);
                push(stack, b % a);
                break;
            case '!':
                push(stack, (pop(stack) == 0)? 1 : 0);
                break;
            case '`':
                push(stack, (pop(stack) < pop(stack))? 1 : 0);
                break;
            case '?':
                direction = rand() % 4;
                break;
            case '_':
                direction = (pop(stack) == 0)? right : left;
                break;
            case '|':
                direction = (pop(stack) == 0)? down : up;
                break;
            case '"':
                stringMode = true;
                break;
            case ':':
                a = pop(stack);
                push(stack, a);
                push(stack, a);
                break;
            case '\\':
                a = pop(stack);
                b = pop(stack);
                push(stack, a);
                push(stack, b);
                break;
            case '$':
                pop(stack);
                break;
            case '.':
                printf("%d", pop(stack));
                break;
            case ',':
                a = pop(stack);
                if(a < 0 || a > CHAR_MAX){
                    printf("the toppest element of the stack is not a char\n");
                    break;
                }
                printf("%c", a);
                break;
            case '#':
                jumpMode = true;
                break;
            case 'g':
                targetY = pop(stack);
                targetX = pop(stack);
                if(!isvalid(targetY, targetX)){
                    printf("The command g at (%d, %d) assigned an invalid position(%d, %d)\n", y, x, targetY, targetX);
                    push(stack, 0);
                    break;
                }
                push(stack, CommandAry[targetY][targetX]);
                break;
            case 'p':
                targetY = pop(stack);
                targetX = pop(stack);
                targetV = pop(stack);
                if(!isvalid(targetY, targetX)){
                    printf("The command p at (%d, %d) assigned an invalid position(%d, %d)\n", y, x, targetY, targetX);
                    break;
                }
                CommandAry[targetY][targetX] = targetV;
                countChange++;
                if((countChange %= UpdatePeriod) == 0){
                    printCommand();
                }
                if(192 * targetY + targetX >= 16384){
                    updateScreen(192 * targetY + targetX - 16384, targetV);
                }
                break;
            case '&':
                a = scanf("%d", &inputInt);
                if(a == 1){
                    push(stack, inputInt);
                    break;
                }
                if(a == EOF){
                    push(stack, -1);
                    break;
                }
                printf("failed to get user's input\n");
                exit(-1);
            case '~':
                a = scanf("%c", &inputChar);
                if(a == 1){
                    push(stack, inputChar);
                    break;
                }
                if(a == EOF){
                    push(stack, -1);
                    break;
                }
                printf("failed to get user's input\n");
                exit(-1);
            case '@':
                printf("\nThe program ended successfully\n");
                return 0;
            default:
                if(isdigit(CommandAry[y][x])){
                    push(stack, CommandAry[y][x] - '0');
                }
                else if(!isspace(CommandAry[y][x]) && CommandAry[y][x] != 0){
                    printf("Invalid Command '%c' at (%d, %d)\n", CommandAry[y][x], y, x);
                }
                break;
        }

        #ifdef DEBUG
        printf("direction after the command %d\n", direction);
        #endif

    }
    printf("The number of commands excceded the limit\n");
    return 0;
}
