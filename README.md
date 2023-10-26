# Rapport TP2

---
__*Choix d'implémentation*__ 


__Fonction bool isSymbol(char c) :__
  - considère tous les symboles qui peuvent être utilisés pour les opérations
  - comparaison en ascii

__Fonction Queue* stringToTokenQueue(const char* expression) :__
  - L'énoncé nous dis d'utiliser curpos en tant que const hors pour pouvoir incrémenter la position curpos ne peut avoir const.
  - Pour que la fonction prenne en compte les valeurs a plusieurs chiffres il faut implémenter la boucle ```
     while (*curpos != '\0' && !isSymbol(*curpos)) {
                i++;
                curpos++;
            }``` sans quoi une valeur comme 34 renverra les tokens "34" et "4"
- vérifier à chaque incrémentation de curpos que c'est pas la fin de la chaine pour ne pas avoir un segmentation fault

__Fonction void printToken(FILE* f, const void* e) :__
- Fonction map classique (complexité O(n))

__Fonction Queue* shuntingYard(Queue* infix) :__
-  *Problème rencontré :*
     - Lors de l'inplémentation de la condition ``` while (( there is an operator at the top of the operator stack with greater precedence ) or ( the operator at the top of the operator stack has equal precedence and the operator is left associative )) and ( the operator at the top of the stack is not a left bracket ) ``` pour que tous les assert soient validés il a fallut pour la dernière condition ajouter une condition ternaire ( ? : ) avec uniquement un false. Etant donné le makefile qui transforme tous les warnings en erreur la seule solution était donc ``` tokenIsParenthesis(stackTop(operator)) ? tokenGetParenthesisSymbol((stackTop(operator))) != ')' : true ```

__Fonction Token *evaluateOperator(Token *arg1, Token *op, Token *arg2) :__
- le cas où l'opérateur n'est pas pris en compte l'erreur est géré et le programme interrompu.
- Si un argument ou l'opérateur n'est pas du bon type alors l'erreur est géré et le programme interrompu.

__Fonction void computeExpressions(FILE *fd) :__ 
- gestion de l'erreur du getfile

__Fonction int main(int argc, char *argv[]) :__
- gestion de l'erreur du fclose


__*Description du comportement sur les jeux de tests fournis*__

- Tous les jeux de test ont été validés


__*Analyse personnelle du travail effectué*__

- Il subsiste toujours une fuite de mémoire (80bits) que je n'arrive pas à régler. La fuite est lié à un token soit-disant non libéré alors qu'il est dans une file qui est libéré plus tard.

