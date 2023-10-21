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
    return (c == 33 || c == 37 || (c > 39 && c < 44) || c == 45 || c == 47 || c == 94 || c == 124);
}

Queue *stringToTokenQueue(const char *expression) {
    Queue *file = createQueue();
    const char *curpos = expression;
    int i = 0;
    while (*curpos != '\0') {
        while (*curpos == ' ' || *curpos == '\n') curpos++;
        if (isSymbol(*curpos)) {
            Token *token = createTokenFromString(curpos, 1);
            curpos++;
            queuePush(file, token);
        } else if (*curpos != '\0') {
            const char *posinit = curpos;
            while (*curpos != '\0' && !isSymbol(*curpos)) {
                i++;
                curpos++;
            }
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
            while (!stackEmpty(operator) && ((tokenIsOperator(token) && tokenIsOperator((stackTop(operator)))) &&
                                             ((tokenGetOperatorPriority((stackTop(operator))) >
                                               tokenGetOperatorPriority(token)) ||
                                              ((tokenGetOperatorPriority((stackTop(operator))) ==
                                                tokenGetOperatorPriority(token)) &&
                                               tokenOperatorIsLeftAssociative(token)))) &&
                   (tokenIsParenthesis(stackTop(operator)) ? tokenGetParenthesisSymbol((stackTop(operator))) != ')'
                                                           : true)) {
                queuePush(postfix, (stackTop(operator)));
                stackPop(operator);
            }
            stackPush(operator, token);
        } else if (tokenIsParenthesis(token) && tokenGetParenthesisSymbol(token) == '(') {
            stackPush(operator, token);
        } else if (tokenIsParenthesis(token) && tokenGetParenthesisSymbol(token) == ')') {
            while (!stackEmpty(operator) && tokenIsParenthesis(stackTop(operator)) ?
                   tokenGetParenthesisSymbol((stackTop(operator))) != '(' : true) {
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
    return postfix;
}

Token *evaluateOperator(Token *arg1, Token *op, Token *arg2) {
    if (tokenIsNumber(arg1) && tokenIsNumber(arg2) && tokenIsOperator(op)) {
        float val1 = tokenGetValue(arg1);
        float val2 = tokenGetValue(arg2);
        float result = 0;
        switch (tokenGetOperatorSymbol(op)) {
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
            default:
                fprintf(stderr, "%c is not a supported operator. Supported operators are + - / * ^\n",
                        tokenGetOperatorSymbol(op));
                deleteToken(&arg1);
                deleteToken(&arg2);
                deleteToken(&op);
                exit(EXIT_FAILURE);
        }
        deleteToken(&arg1);
        deleteToken(&arg2);
        deleteToken(&op);
        return createTokenFromValue(result);
    } else {
        fprintf(stderr, "erreur lors de l'évaluation de l'opération\n");
        exit(EXIT_FAILURE);
    }
}

float evaluateExpression(Queue *postfix) {
    Stack *operand = createStack((int) queueSize(postfix));
    Token *evaluatedToken;
    while (!queueEmpty(postfix)) {
        if (!stackEmpty(operand) && tokenIsOperator(queueTop(postfix))) {
            Token *arg2 = stackTop(operand);
            stackPop(operand);
            if (!stackEmpty(operand)) {
                Token *arg1 = stackTop(operand), *op = queueTop(postfix);
                stackPop(operand);
                evaluatedToken = evaluateOperator(arg1, op, arg2);
                stackPush(operand, evaluatedToken);
            }
        } else {
            stackPush(operand, queueTop(postfix));
        }
        queuePop(postfix);
    }
    float value;
    if (!stackEmpty(operand) && tokenIsNumber(stackTop(operand))) { value = tokenGetValue(stackTop(operand)); }
    else {
        deleteStack(&operand);
        deleteToken(&evaluatedToken);
        fprintf(stderr, "erreur lors de l'évaluation de l'expression\n");
        exit(EXIT_FAILURE);
    }
    deleteToken(&evaluatedToken);
    deleteStack(&operand);
    return value;

}

void computeExpressions(FILE *fd) {
    int testgetline;
    size_t size;
    while (!feof(fd)) {
        char *buffer = NULL;
        if ((testgetline = getline(&buffer, &size, fd)) > 1) {
            printf("Input    : %s\rInfix    : ", buffer);
            Queue *queue = stringToTokenQueue(buffer);
            queueDump(stdout, queue, printToken);
            printf("\nPostfix  : ");
            Queue *postfix= shuntingYard(queue);
            queueDump(stdout, postfix, printToken);
            float evaluatedExpression = evaluateExpression(postfix);
            printf("\nEvaluate : %6f\n\n", evaluatedExpression);
            deleteQueue(&queue);
            deleteQueue(&postfix);
        }
        free(buffer);
        if (!testgetline) {
            perror("getline");
            exit(EXIT_FAILURE);
        }
    }
}


/** Main function for testing.
 * The main function expects one parameter that is the file where expressions to translate are
 * to be read.
 *
 * This file must contain a valid expression on each line
 *
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
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
    int testfclose = fclose(fd);
    if (testfclose == EOF) {
        perror("fclose");
        return 1;
    }

    return 0;
}

