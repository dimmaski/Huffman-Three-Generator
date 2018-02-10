#include "huffmanThree.h"

int main() {


	int top = 0;
	int array[100];
	// nº of members of the alphabet
	int m;

	char stream[200];
	char c;
	struct node * root = createNYTROOT(m); // save the root node
	struct node * currentNYT = malloc(sizeof(struct node));
	struct node * max;
	struct node * fl;
	
	currentNYT = root;
	struct nodePointer * leafs;
	struct node * aux;
	
	printf("Number of members in the alphabet: ");
	scanf("%d",m);
	
	printf("Stream to encode:");
	scanf("%s",stream);
		
	for(int i = 0; i < strlen(stream); i++) {
		
		printf("\n");
		
		leafs = findLeafs(root);
		fl = charInThree(leafs, stream[i]);
		
		
	
		if(fl != NULL) {
		
			// if symbol is present in three, then it's not the first occurrence
			// aux holds the pointer to the leaf
			
			
			while(1) {
				struct node * s;
				
				max = maxInBlock(root, fl);
				
				if(max != fl) {
					// node ain't max in block
					
					
					swapNodes(max, fl);
					s = max;
					max = fl;
					fl = s;
					
				}
				
				fl->weight = (fl->weight) + 1;
				
				if(nodeIsRoot) {
					break;
				}
			
			fl = fl->father;
						
			}
			
		
		

		} else {
			
			// if here, then the symbol is not in the three
			// 	- first occurrence
			currentNYT = firstOccurrence(currentNYT, stream[i]);
			
			// get aux to point to old NYT
			aux = currentNYT->father;
			

			while(!nodeIsRoot(aux)){
				
				aux = aux->father;
				// check if the num in aux is max in block
				max = maxInBlock(root,aux);
				
				if(max != aux) {
					
					struct node * s;
					swapNodes(max, aux);
					s = max;
					max = aux;
					aux = s;
				
				}
				
				aux->weight = (aux->weight) + 1;	
				
					
			}
	
		}


	cleanPointerList(leafs);
	printCodes(root, array, top);
	
	}
		

}

