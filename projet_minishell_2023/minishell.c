/**************************************/
/*            minishell.c             */
/**************************************/

#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include "analex.h"

#define TAILLE_MAX 100 /* taille max d'un mot */
#define ARGS_MAX 10    /* nombre maximum d'arguments a une commande */

/* Execute la commande donnee par le tableau argv[] dans un nouveau processus ;
 * les deux premiers parametres indiquent l'entree et la sortie de la commande,
 * a rediriger eventuellement.
 * Renvoie le numero du processus executant la commande
 *
 * FONCTION À MODIFIER/COMPLÉTER.
 *
*/
pid_t execute(int entree, int sortie, char* argv[]) {
	int status=0;
	pid_t pid_fils=fork();
	if (pid_fils == -1){
		return -1;   // le fork a echoué
	}
	if ( pid_fils == 0) {
		int status_code =execvp(argv[0],argv); 
		if (status_code == -1){   // execute la commande et si l'execution du fils à échouer, le fils renvoie 2
			printf("La commande ne s'est pas execute correctement.\n");
			return 2;
		}
	}
	else {
		wait(&status);
		if (WEXITSTATUS(status) == 2){     // si l'execution du fils a échouer, le père retourne -2
			return -2;
		}
		if (WEXITSTATUS(status) ==-1){  //si l'execution de la commande a echoué on retourne -1
			return -1;
		}
		else{
			return pid_fils;      //sinon, on retourne 0
		}
	}
	return pid_fils;
}

/* Lit et execute une commande en recuperant ses tokens ;
 * les deux premiers parametres indiquent l'entree et la sortie de la commande ;
 * pid contient le numero du processus executant la commande et
 * background est non nul si la commande est lancee en arriere-plan
 *
 * FONCTION À MODIFIER/COMPLÉTER.
*/
TOKEN commande(int entree, int sortie, pid_t* pid, int* background){
	int i=0;
	TOKEN t;
	char word[TAILLE_MAX];
	char next_word[TAILLE_MAX];
	char* tabArgs[ARGS_MAX];
	char* copy;
	while(1) {
		t = getToken(word);
		if ( t==T_WORD ){
			copy = strdup(word);  //on copie le mot dans le tableau des arguments
			tabArgs[i]=copy;
      		//fprintf(stderr,"Argument %d : %s\n",i,tabArgs[i]);
      		i++;
		}
		if (t == T_GT){
			getToken(next_word);
			sortie = open(next_word,O_RDONLY);     //Revoir
		}
		if ( t==T_NL ) {    //fin de ligne donc on execute la commande
			//fprintf(stderr,"on entre NL: %d\n",i);
			tabArgs[i]=NULL;
			*pid=execute(entree,sortie,tabArgs);   //pb ici dans execute a regler
			//fprintf(stderr,"allo\n");
			return T_NL;
		}
		if ( t==T_EOF ) {
			//fprintf(stderr,"on entre EOF: %d\n",i);
			free(copy);
			return T_EOF;
		}
	}
}


/* Retourne une valeur non-nulle si minishell est en train de s'exécuter en mode interactif,
 * c'est à dire, si l'affichage de minishell n'est pas redirigé vers un fichier.
 * (Important pour les tests automatisés.)
 *
 * NE PAS MODIFIER CETTE FONCTION.
 */
int is_interactive_shell(){
  return isatty(1);
}

/* Affiche le prompt "minishell>" uniquement si l'affichage n'est pas redirigé vers un fichier.
 * (Important pour les tests automatisés.)
 *
 * NE PAS MODIFIER CETTE FONCTION.
 */
void print_prompt(){
  if(is_interactive_shell()){
    printf("mini-shell>");
    fflush(stdout);
  }
}

/* Fonction main
 * FONCTION À MODIFIER/COMPLÉTER.
 */
int main(int argc, char* argv[]) {
	TOKEN t;
	pid_t pid;//, fid;
	int flag = 0;
	int background = 0;
	int status = 0;
	print_prompt(); // affiche le prompt "minishell>"

	while (1) {  //boucle a modifier
		t = commande(0, 1, &pid, &background);
		if (t == T_NL) {
			print_prompt();
		}
		if (t == T_EOF) {
			break;
		}
		if (t == T_AMPER && flag ==0){
			flag=1;
			waitpid(-1,&status,0);
		}
	}
	if(is_interactive_shell()) {
		printf("\n") ;
	}
	return status; // Attention à la valeur de retour du wait, voir man wait et la macro WEXITSTATUS
}
