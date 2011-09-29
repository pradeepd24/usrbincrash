/* This program uses a Dynamic programming approach. The worst case run time is O(eject*N), where eject=weight that needs to be ejected, N=# of Stock Keeping Units(SKU) in the input file */

#include <stdio.h>
#include <stdlib.h>

/* this structure is the information about each SKU */
struct item {
	unsigned int weight;
	unsigned int cost;
}x;

/* this is the linked list of the SKUs */
struct node {
	struct item x;
	int visited;
	struct node *next;
	struct node *prev;
}*head;

/* adding a SKU node to the head of a circular linked list */
void add(struct item x) {
	struct node *temp;
	temp = (struct node*)malloc(sizeof(struct node));
	temp->x = x;
	temp->visited = 0;
	if(head == NULL) { 
		head = temp;
		temp->next = temp;
		temp->prev = temp; 
		return;
	}
 	else {
		temp->prev = head->prev;
		temp->next = head;
		head->prev->next = temp;
		head->prev = temp;
		return;
	}
}

/* removing a SKU node from the circular linked list */
void remove_ll(struct node *t) {
	if(t == NULL)
		return;
	if(t->next == t) {
		free(t); 
		head = NULL; 
		return;
	}
	if(t == head)
		head = head->next;
	t->prev->next = t->next;
	t->next->prev = t->prev;
	free(t);
}

/* finds the gcd of a and b */
unsigned int gcd(unsigned int a, unsigned int b) {
	while(1) {
		if(a<=b) {
			if(b%a == 0)
				return a;
			b = b%a;
			continue;
		}
		else {
			if(a%b == 0)
				return b;
			a = a%b;
			continue;
		}
	}
}

/* floor of x */
int floorf1(float x) {
	return((int)x);
}

/* ceil of x */
int ceilf1(float x) {
	if((int)x < x)
		return((int)x+1);
 	else
		return((int)x);
}


/* this function returns the min_cost for ejecting eject amount of weight */
unsigned int optm(unsigned int eject) {
	unsigned int i,b,c; /* i=loop variable going from 0 to eject, b is the best cost at some point of time, c is the best cost to eject i-some_weight */
	unsigned int *S1=(unsigned int *)malloc(sizeof(unsigned int)*(eject+1)); /* S1[i] will contain the min_cost to eject i amount of weight */
	unsigned int min_cost; /* minimum cost to eject eject amount of weight */
	struct node *temp; /* temp node to traverse through the SKU linked list */
	temp = head->next;
	for(i = 0; i <= eject; i++) { 
		b = (int)(ceilf1((i*1.0/head->x.weight)))*head->x.cost; /* initially best cost is with using the SKU in the head of the linked list */
		if(i>=head->x.weight) {
			c = S1[i-head->x.weight]; /* taking one SKU of the current unit and the remaining cost is in c */
			if(b > c + head->x.cost)
				b = c + head->x.cost;
		}
		temp = head->next;
		while(temp != head) { /* going through the entire linked list and finding the best cost for this i */
			if(i>=temp->x.weight) {
				c = S1[i-temp->x.weight];
				if(b>c + temp->x.cost)
					b = c + temp->x.cost;
			}
			else {
				if(temp->x.cost < b)
					b = temp->x.cost;
			}
   			temp = temp->next;
		}
  		S1[i] = b; /* now S[i] will contain the best cost for ejecting i amount of weight */
	}

	min_cost = S1[eject];
	free(S1);
	return min_cost;
} /* end of function optm */


int main(int argc, char *argv[]) {
	FILE *fp;
	unsigned int eject; //amount of weight to be ejected 
	unsigned int N=0;   //# of SKUs in the input
	struct node* temp,*temp1; // variables used to traverse the SKU linked list
	struct item x;  // variable used to obtain the inputs from the file
	unsigned int o_gcd; // to hold the gcd of all the weights
	char a[100]; // dummy string
	head = NULL; // head of the SKU linked list is initially NULL

	if(argc < 2) {
		printf("Please specify an input filename in the command line.\n");
		return 0;
	}

	fp = fopen(argv[1],"r");
	fscanf(fp,"%d",&eject);

	while(fscanf(fp,"%s %d %d",a,&x.weight,&x.cost) != EOF ) { // getting inputs from the file
		add(x); //adding it to the SKU linked list
		N++;
	}
	temp = head;
	if(N==1) { /* if only one SKU, then very easy, just find how many of them is required */
		printf("%d\n",((int)(ceilf1(eject*1.0/head->x.weight)))*(head->x.cost));
		return 0;
	}


	/* here, all those SKU whose weight is less than some other SKUs AND whose cost is more are removed from the list, as they are not required as we have a better SKU than that */
	/* this runs in O(N^2) */
	while(1) { 
		if(temp->visited == 1)
			break;
		else {
			temp1 = temp->next;
			while(temp1 != temp) {
				if(temp1->visited)
					break;
				if(temp1->x.weight < temp->x.weight && temp1->x.cost > temp->x.cost) { /* temp is better than temp1, temp1 need not be used at all because we can replace every use of temp1 by temp and getter a better result */
					temp1 = temp1->next; 
					remove_ll(temp1->prev); 
				}
				else 
					temp1= temp1->next;
			}
  			temp->visited = 1;
			temp = temp->next;
		}
	}

	/* this is very important thing in optimizing. gcd of all weights are found and all weights are divided by this gcd. This reduces the eject by a factor of o_gcd. This doesn't change the result*/	  /* but reduces the run-time and memory usage a lot */

	o_gcd = gcd(eject,head->x.weight); /*o_gcd is the overall gcd of all weights+eject */
	temp = head->next;
	while(temp != head) {
		o_gcd = gcd(o_gcd,temp->x.weight);
		temp = temp->next;
	}

	/* reducing all weights by a factor of o_gcd */
	eject = eject/o_gcd;
	head->x.weight = head->x.weight/o_gcd;
	temp = head->next;
	while(temp != head) {
		temp->x.weight = temp->x.weight/o_gcd;	
		temp = temp->next;
	}

	printf("%d\n",optm(eject));

	/* freeing all dynamically allocated memory */
	temp = head;
	while(temp->next != temp) {
		temp1 = temp->next;
		remove_ll(temp);
		temp = temp1;	
	}
	free(temp);
	return 0;
}



