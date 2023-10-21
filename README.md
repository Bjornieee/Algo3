# Rapport TP2

---

## Fonction Queue* stringToTokenQueue(const char* expression) :
  - L'énoncé nous dis d'utiliser curpos en tant que const hors pour pouvoir incrémenter la position curpos ne peut avoir const.
  - Pour que la fonction prenne en compte les valeurs a plusieurs chiffres il faut implémenter la boucle ``` while (*curpos != '\0' && !isSymbol(*curpos)) {
                i++;
                curpos++;
            }``` sans quoi une valeur comme 34 renverra les tokens "34" et "4"
- vérifier à chaque incrémentation de curpos que c'est pas la fin de la chaine pour ne pas avoir un segmentation fault

## Fonction void printToken(FILE* f, const void* e) :
- Fonction map classique (complexité O(n))
## Fonction Queue* shuntingYard(Queue* infix) :
- ### Problème rencontré :
     - Lors de l'inplémentation de la condition ``` while (( there is an operator at the top of the operator stack with greater precedence ) or ( the operator at the top of the operator stack has equal precedence and the operator is left associative )) and ( the operator at the top of the stack is not a left bracket ) ``` pour que tous les assert soient validés il a fallut pour la dernière condition ajouter une condition ternaire ( ? : ) avec uniquement un false. Etant donné le makefile qui transforme tous les warnings en erreur la seule solution était donc ``` tokenIsParenthesis(stackTop(operator)) ? tokenGetParenthesisSymbol((stackTop(operator))) != ')' : true ```
