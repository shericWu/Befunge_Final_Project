#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#define CommandWidth 5000
#define CommandLength 500
#define CommandNumLimit 50000 // to avoid infinite loop
#define FILENAME "BefungeCommand.txt" // the sourse of the command

enum dir{right, down, left, up};

char CommandAry[CommandWidth][CommandLength];
int dx[4] = {1, 0, -1, 0};
int dy[4] = {0, 1, 0, -1};

typedef struct list{
    int value;
    struct list *next;
} List;

typedef struct stack{
    struct list *top; // top points the toppest element in the stack
} Stack;

List *genNode(int value, List *next){
    List *newNode = (List *)malloc(sizeof(List));
    newNode->value = value;
    newNode->next = next;
    return newNode;
}

void initialStack(Stack **stack){
    *stack = (Stack *)malloc(sizeof(Stack));
    (*stack)->top = NULL;
    return;
}

bool isempty(Stack *stack){
    return stack->top == NULL;
}

int pop(Stack *stack){
    if(isempty(stack)){
        printf("failed to pop from the stack\n");
    }
    int out = stack->top->value;
    stack->top = stack->top->next;
    return out;
}

void push(Stack *stack, int value){
    stack->top = genNode(value, stack->top);
    return;
}

void LoadCommand(FILE *fp){
    for(int i = 0; i < CommandLength; i++){
        fgets(CommandAry[i], CommandWidth - 5, fp);
    }
    return;
}

bool isvalid(int y, int x){
    return y >= 0 && y < CommandLength && x >= 0 && x < CommandWidth;
}

int main(void){
    Stack *stack = NULL;
    initialStack(&stack);
    FILE *fp = fopen(FILENAME, "r");
    if(fp == NULL){
        printf("failed to open the file\n");
        return 0;
    }
    LoadCommand(fp);
    fclose(fp);

    #ifdef DEBUG
    for(int i = 0; i < CommandLength; i++){
        for(int j = 0; j < CommandWidth; j++){
            printf("%c", CommandAry[i][j]);
        }
    }
    #endif

    int x = 0, y = 0;
    int direction = right;
    bool stringMode = false;
    for(int numCommand = 0; numCommand < CommandNumLimit; numCommand++, x += dx[direction], y += dy[direction]){

        #ifdef DEBUG
        printf("%d\n", direction);
        printf("%d %d %c\n", y, x, CommandAry[y][x]);
        #endif

        if(!isvalid(y, x)){
            printf("The program shut down for running at an invalid positon\n");
            return 0;
        }
        if(stringMode){
            if(CommandAry[y][x] == '\"'){
                stringMode = false;
                continue;
            }
            push(stack, CommandAry[y][x]);
            continue;
        }
        int a = -1, b = -1, targetX = -1, targetY = -1, targetV = -1, input = -1;
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
                push(stack, - pop(stack) + pop(stack));
                break;
            case '*':
                push(stack, pop(stack) * pop(stack));
                break;
            case '/':
                a = pop(stack);
                b = pop(stack);
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
                srand(time(NULL));
                direction = (rand()) % 4;
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
                printf("%c", pop(stack));
                break;
            case '#':
                x += dx[direction];
                y += dy[direction];
                break;
            case 'g':
                targetY = pop(stack);
                targetX = pop(stack);
                if(isvalid(targetY, targetX)){
                    push(stack, CommandAry[targetY][targetX]);
                    break;
                }
                push(stack, 0);
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
                break;
            case '&':
                assert(scanf("%d", &input));
                push(stack, input);
                break;
            case '~':
                assert(scanf("%c", &input));
                push(stack, input);
                break;
            case '@':
                return 0;
            default:
                if(isdigit(CommandAry[y][x])){
                    push(stack, CommandAry[y][x] - '0');
                }
                else if(!isspace(CommandAry[y][x])){
                    printf("Invalid Command '%c' at (%d, %d)\n", CommandAry[y][x], y, x);
                }
                break;
        }

        #ifdef DEBUG
        printf("%d\n", direction);
        #endif

    }
    printf("The number of commands excceded the limit\n");
    return 0;
}