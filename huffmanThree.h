#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


struct node {

	int num;
	int weight;
	char TAG;  // n -> NYT, l -> leaf, i -> node, r -> root, f -> NYT / root
	char symbol; // '~' means nothing
	struct node * left;
	struct node * right;
	struct node * father; // we will keep track of every node's father so it's easier to navigate the three

};

struct nodePointer {

	// this struct is only meant to save pointers to nodes in the three, used in findLeafs()
 
	struct node * pointer;
	struct nodePointer * next;

};


struct node * createNYTROOT(int m) {
	
	// first node ROOT/NYT
	int n;

	n = m + 1;
	struct node * NYT = malloc(sizeof(struct node));
	NYT->num = 2*n - 1;
	NYT->weight = 0;
	NYT->symbol = '~';
	NYT->left = NULL;
	NYT->right = NULL;
	NYT->father = NULL;
	NYT->TAG = 'f';

}

struct nodePointer * initnodePointerList() {
	
	struct nodePointer * head = malloc(sizeof(struct nodePointer));
	head->next = NULL;
	head->pointer = NULL; // head is not suposed to point to any node
	return head;
}

void printSizeList(struct nodePointer * head) {
	// mostly used for debugging

	struct nodePointer * h = head->next;
	int i = 0;
	while(h) {
		h = h->next;
		i++;
	}	

}

void cleanPointerList(struct nodePointer * head) {

	struct nodePointer * h;

	while(head->next) {
		h = head->next;
		head->next = head->next->next;	
		free(h);
	}

}

int nodePointerListEmpty(struct nodePointer * head) {
	if(head->next == NULL){
		return 1;	
	}else{
		return 0;
	}
}

void addnodePointerList(struct nodePointer * head, struct node * node) {
		
	struct nodePointer * new = malloc(sizeof(struct nodePointer));

	if(nodePointerListEmpty(head)) {

		head->next = new;
		new->pointer = node;
		new->next = NULL;
		
	} else {

		new->next = head->next;
		head->next = new;
		new->pointer = node;
	}
}

void removenodePointerList(struct nodePointer * head, struct node * node) {
	
	struct nodePointer * last = head;
	struct nodePointer * current = head->next;
	struct nodePointer * elem;	
	
	if(head->next->pointer == node) {
		elem = head->next;
		head->next = head->next->next;
		free(elem);
		return;
	}
	
	while(current != NULL) {
		if(current->pointer == node){
			last->next = current->next;
			free(current);
		}

	last = current;
	current = current->next;
	}

}


struct node * firstOccurrence(struct node * currentNYT, char c) {

	// currentNYT generates 2 nodes
	//	- left one is going to be the new NYT
	// 	- right one is going to be the new leaf which will contain the new simbol (char c)
	currentNYT->left = malloc(sizeof(struct node));
	currentNYT->right = malloc(sizeof(struct node));
	
	// currentNYT is now a inner node
	// 	- num stays the same
	//	- weight will be the sum of the new NYT node weight (0) + the weight of new symbol (1) = 1
	
	if(currentNYT->father == NULL){
		// if currentNYT does not have a father node, than it is the root node
		// this only happens in the generation of the node for the first symbol in the stream
		currentNYT->TAG = 'r';
	}else{
		// if it does, then it's a simple inner node
		currentNYT->TAG = 'i';
	}
	currentNYT->symbol = '~'; // inner nodes do not contain any symbol
	currentNYT->weight = 1;
	
	// new NYT node
	currentNYT->left->TAG = 'n';
	currentNYT->left->symbol = '~'; // NYT node does not contain any symbol
	currentNYT->left->num = currentNYT->num - 2;
	currentNYT->left->weight = 0;
	currentNYT->left->left = NULL;
	currentNYT->left->right = NULL;
	currentNYT->left->father = currentNYT;
	
	// new leaf which contains the new symbol
	currentNYT->right->TAG = 'l';
	currentNYT->right->symbol = c; // leaf gets the symbol
	currentNYT->right->num = currentNYT->num - 1;
	currentNYT->right->weight = 1;
	currentNYT->right->left = NULL;
	currentNYT->right->right = NULL;
	currentNYT->right->father = currentNYT;
	
	// we want to keep track of our NYT node at all times
	return currentNYT->left;
}

struct nodePointer * findLeafs(struct node * root) {

	// generate a linked list with pointers to all leafs in the three
	// having this linked list will be usefull to find out all the symbols that are currently in the three
	
	// we will create a list with pointers to all worth checking nodes
	struct nodePointer * nodePointerList = initnodePointerList();
	// and our leaf list
	struct nodePointer * leafs = initnodePointerList();
	
	struct node * aux; // auxiliar pointers to use
	aux = root;
	addnodePointerList(nodePointerList, aux);
	
	
	// starting from root, we will select the pointers that are worth checking
	// if a node has 2 != NULL pointers this means that
	//	- this node is either root or a inner node
	// 		- add this node to list and keep searching
	
	while(!nodePointerListEmpty(nodePointerList)){
		
		aux = nodePointerList->next->pointer;
	
		if(aux->left != NULL && aux->right != NULL) {
			
			addnodePointerList(nodePointerList, aux->left);				
			addnodePointerList(nodePointerList, aux->right);
			removenodePointerList(nodePointerList, aux);
			

		} else if(aux->left == NULL && aux->right == NULL) {
			// then pointer aux is pointing to either a leaf with a symbol, or NYT node
			// 	- so, if aux is a leaf we add it to the leafs list
			if(aux->TAG == 'l'){
 				
				addnodePointerList(leafs, aux);
				removenodePointerList(nodePointerList, aux);
				
			} else if(aux->TAG == 'n') {
				// else TAG == 'n' which is NYT

				removenodePointerList(nodePointerList, aux);

			} else if(aux->TAG == 'f') {
				// if we only have NYT / ROOT

				removenodePointerList(nodePointerList, aux);
			}
			
		}
	}
	
	return leafs;		
}

int nodeIsRoot(struct node * pointer) {

	if(pointer->TAG == 'r' || pointer->TAG == 'f') {
		return 1;
	}

	return 0;

}

struct node * charInThree(struct nodePointer * head, char c) {

	struct nodePointer * h = head->next;
	while(h) {
		
		if(h->pointer->symbol == c) {
			printf("%c %c\n",h->pointer->symbol,c);
			return h->pointer;
		}

		h = h->next;
	} 

	return NULL;
}

void swapNodes(struct node * a, struct node *b) {

	// swap every piece of info, exept the node number
	struct node saver; // auxiliar struct to use as container

	saver.weight = a->weight;
	saver.TAG = a->TAG;
	saver.symbol = a->symbol;
	saver.left = a->left;
	saver.right = a->right;
	//saver.father = a->father;

	a->weight = b->weight;
	a->TAG = b->TAG;
	a->symbol = b->symbol;
	a->left = b->left;
	a->right = b->right;
	//a->father = b->father;
	
	b->weight = saver.weight;
	b->TAG = saver.TAG;
	b->symbol = saver.symbol;
	b->left = saver.left;
	b->right = saver.right;
	//b->father = saver.father;

}

void printCodes(struct node * root, int array[], int top) {

	if(root->left) {
		array[top] = 0;
		printCodes(root->left, array, top + 1);
	}
	
	if(root->right) {
		array[top] = 1;
		printCodes(root->right, array, top + 1);
	}

	if(root->TAG == 'l') {
		printf("%c: ", root->symbol);
		for(int i = 0; i < top ; i++) {
			printf("%d",array[i]);
		}
		printf("\n");
	} else if(root->TAG == 'n') {
		printf("NYT: ");
		for(int i = 0; i < top ; i++) {
			printf("%d",array[i]);
		}
		printf("\n");
		}

}

struct node * maxInBlock(struct node * root, struct node * n) {
	// this function's purpose is to find out if there is a node in the three
	// that has a num > n->num having the same weight as n
	// 	- if there is - return the pointer to the node
	//	- if there isn't - return NULL
	
	// auxiliar list
	struct nodePointer * nodePointerList = initnodePointerList();
	
	// this list will save all the pointers in the same block as n
	// 	- block : all the nodes with the same weight that are not fathers of of n
	struct nodePointer * sameWeight = initnodePointerList();
	struct node * maxInBlock = NULL; // the pointer to return is initialized as NULL
	
	struct node * aux; // auxiliar pointers to use
	aux = root;
	addnodePointerList(nodePointerList, aux);

	while(!nodePointerListEmpty(nodePointerList)){
		
		aux = nodePointerList->next->pointer;
		
		if(aux->right != NULL && aux->left != NULL) {
			// check the aux node for it's weight, if it's in the BLOCK add it to sameWeight list
			// and remove it from the nodePointerList
			// if it isn't proceed to add it's children nodes to the nodePointerList to further check them 
			if(aux->weight == n->weight && n->father != aux) {
				// same weight && not n's father
				addnodePointerList(sameWeight, aux);
				
			}
			
				addnodePointerList(nodePointerList, aux->left);
				addnodePointerList(nodePointerList, aux->right);
				removenodePointerList(nodePointerList, aux);

		} else if(aux->left == NULL && aux->right == NULL) {
			// then this node is either leaf, NYT, or NYT/ROOT
			if(aux->weight == n->weight) {
				// if it's a leaf node we only need to check if it's weight
				addnodePointerList(sameWeight, aux);
			}
			removenodePointerList(nodePointerList, aux);
		}

	}
	
	// at this point the sameWeight list is built

	printSizeList(sameWeight);
	
	nodePointerList = sameWeight->next;
	maxInBlock = nodePointerList->pointer;
	
	while(nodePointerList) {
		// find the node with the higgest num
		if(nodePointerList->pointer->num > maxInBlock->num) {
			maxInBlock = nodePointerList->pointer;
		}
		nodePointerList = nodePointerList->next;	
	}

	
	return maxInBlock;
}
