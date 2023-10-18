// Need this to use the getline C function on Linux. Works without this on MacOs. Not tested on Windows.
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "token.h"
#include "queue.h"
#include "stack.h"

bool isSymbol(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')');
}

Queue *stringToTokenQueue(const char *expression) {
    Queue *file;
    file = createQueue();
    const char *curpos = expression;
    int i = 0;
    while (*curpos != '\0') {
        while (*curpos == ' ' || *curpos == '\n') curpos++;
        if (isSymbol(*curpos)) {
            Token *tokenSymbol = createTokenFromString(curpos, sizeof(char));
            queuePush(file, tokenSymbol);
            curpos++;
        } else if (*curpos != '\0') {
            const char *posinit = curpos;
            curpos++;
            i++;
            Token *token = createTokenFromString(posinit, i);
            queuePush(file, token);
        }
    }
    return file;
}

void printToken(FILE *f, void *e) {
    Token *token = (Token *) e;
    tokenDump(f, token);
}

Queue *shuntingYard(Queue *infix) {
    Queue *postfix = createQueue();
    Stack *operator = createStack((int) queueSize(infix));
    while (!queueEmpty(infix)) {
        Token *token = queueTop(infix);
        if (tokenIsNumber(token)) {
            queuePush(postfix, token);
        } else if (tokenIsOperator(token)) {
            while (!stackEmpty(operator) && ((tokenIsOperator(token) && tokenIsOperator((stackTop(operator)))) && ((tokenGetOperatorPriority((stackTop(operator))) > tokenGetOperatorPriority(token)) || ((tokenGetOperatorPriority((stackTop(operator))) == tokenGetOperatorPriority(token)) &&
                     tokenOperatorIsLeftAssociative(token)))) && (!tokenIsParenthesis(stackTop(operator)) ? true : tokenGetParenthesisSymbol((stackTop(operator))) != ')' )) {
                queuePush(postfix, (stackTop(operator)));
                stackPop(operator);
            }
            stackPush(operator, token);
        } else if (tokenIsParenthesis(token)&&tokenGetParenthesisSymbol(token) == '(') {
            stackPush(operator, token);
        } else if (tokenIsParenthesis(token)&&tokenGetParenthesisSymbol(token) == ')') {
            while (!tokenIsParenthesis(stackTop(operator)) ? true : tokenGetParenthesisSymbol((stackTop(operator))) != '(' ) {
                queuePush(postfix, (stackTop(operator)));
                stackPop(operator);
            }
            stackPop(operator);
        }
        queuePop(infix);
    }
    if (queueSize(infix) == 0) {
        while (!stackEmpty(operator)) {
            queuePush(postfix, stackTop(operator));
            stackPop(operator);
        }
    }
    deleteStack(&operator);
    deleteQueue(&infix);
    return postfix;
}
Token *evaluateOperator(Token *arg1, Token *op, Token *arg2){
    float val1 = tokenGetValue(arg1);
    float val2 = tokenGetValue(arg2);
    float result = 0;
    switch(tokenGetOperatorSymbol(op)) {
        case '+':
            result = val1 + val2;
            break;
        case '-':
            result = val1 - val2;
            break;
        case '*':
            result = val1 * val2;
            break;
        case '/':
            result = val1 / val2;
            break;
        case '^':
            result = powf(val1, val2);
            break;
    }
    deleteToken(&arg1);
    deleteToken(&arg2);
    deleteToken(&op);
    return createTokenFromValue(result);
}
float evaluateExpression(Queue *postfix) {
    Stack *operand = createStack((int) queueSize(postfix));
    while(!queueEmpty(postfix)) {
        if(tokenIsOperator(queueTop(postfix))) {
            Token *arg2 = stackTop(operand);
            stackPop(operand);
            Token *arg1 = stackTop(operand), *op = queueTop(postfix);
            stackPop(operand);
            stackPush(operand, evaluateOperator(arg1, op, arg2));
        } else {
            stackPush(operand, queueTop(postfix));
        }
        queuePop(postfix);
    }
    float value;
    if(!stackEmpty(operand)){value = tokenGetValue(stackTop(operand));}
    else {deleteStack(&operand);
        fprintf(stderr,"erreur lors de l'évaluation de l'expression\n");
        exit(EXIT_FAILURE);}
    deleteStack(&operand);
    return value;
}

void computeExpressions(FILE *fd) {
    size_t size = 128;
    char *buffer;
    buffer = malloc(size * sizeof(char));
    if(!buffer){
        fprintf(stderr, "malloc buffer");
        exit(EXIT_FAILURE);
    }
    int testgetline;
    Queue *queue;
    while (!feof(fd)) {
        if ((testgetline = getline(&buffer, &size, fd)) > 1) {
            printf("Input    : %s\rInfix    : ", buffer);
            queue = stringToTokenQueue(buffer);
            queueDump(stdout, queue, &printToken);
            printf("\nPostfix  : ");
            Queue *postfix = shuntingYard(queue);
            queueDump(stdout, postfix, &printToken);
            printf("\nEvaluate : %6f\n\n", evaluateExpression(postfix));
            deleteQueue(&postfix);
        }
        if (!testgetline) {
            perror("getline ");
            exit(EXIT_FAILURE);
        }
    }
    free(buffer);
    exit(0);
}


/** Main function for testing.
 * The main function expects one parameter that is the file where expressions to translate are
 * to be read.
 *
 * This file must contain a valid expression on each line
 *
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage : %s filename\n", argv[0]);
        return 1;
    }
    FILE *fd;
    fd = fopen(argv[1], "r");
    if (!fd) {
        perror(argv[1]);
        return 1;
    }
    computeExpressions(fd);
    fclose(fd);
    return 0;
}

