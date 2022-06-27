#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 50
#define LINE_LEN 150
#define OP_LEN 40
#define MAX_OPS 3

typedef struct Node
{
	char *categoryName;
	int depth;
	struct Node *parentNode;
	struct Node *childNode;
	struct Node *siblingNode;
} Node;


/* checkPointer: checks if passed void pointer has NULL as it's value.
 * If pointer has value NULL, message is printed and program is immediately
 * terminated. This function is used to check whether an function has failed
 * or not.
 * 
 * It takes void pointer, which was casted in the calling fuction, and string,
 * represents function which has returned that pointer, as parameters */
void checkPointer( void *pointer, char *fname )
{
	if( pointer == NULL )
	{
		fprintf( stderr, "%s: failed\n", fname );
		exit(EXIT_FAILURE);
	}
}

/* createNode: creates new a node
 * 
 * It takes name of category and it's depth in the tree
 * as parameters and returns pointer to that node  */
Node *createNode( char *categoryName )
{
	Node *newNode = (Node *) malloc( sizeof( Node ) );
	checkPointer( (void *) newNode, "malloc" );

	newNode->categoryName = (char *) malloc( NAME_LEN + 1 );
	checkPointer( (void *) newNode->categoryName, "malloc" );

	strcpy( newNode->categoryName, categoryName );

	newNode->depth = 0;
	newNode->parentNode = NULL;
	newNode->childNode = NULL;
	newNode->siblingNode = NULL;

	return newNode;
}

/* addChild: adds child node to the parent node.
 * If parent is NULL, child node is set as a root
 *
 * Function takes pointer to the parent node pointer and
 * pointer to child node as parameters*/
void addChild( Node **parentNode, Node *childNode )
{
	childNode->parentNode = *parentNode;

	if( *parentNode == NULL)
		*parentNode = childNode;
	else if( (*parentNode)->childNode == NULL )
		(*parentNode)->childNode = childNode;
	else
	{
		Node *prevNode, *currNode;

		prevNode = NULL;
		currNode = (*parentNode)->childNode;
		/* find place where node, lexicographicly, belongs */
		while( currNode && strcmp( currNode->categoryName, childNode->categoryName ) <= 0 )
		{
			prevNode = currNode;
			currNode = currNode->siblingNode;;
		}

		if( prevNode == NULL )
		{ /* New child node should be placed as a first node in the list */
			childNode->siblingNode = currNode;
			(*parentNode)->childNode = childNode;
		}
		else
		{ /* New child node should be placed between previous and current node */
			childNode->siblingNode = currNode;
			prevNode->siblingNode = childNode;
		}
	}

	/* Child's depth has value 1 more then value of parent's depth */
	if( childNode->parentNode != NULL )
		childNode->depth = childNode->parentNode->depth + 1;
}

/* findNode: finds node which has the same categoryName as passed string
 * Takes pointer to the node from which search begins and category which
 * is searched as parameters, and returns pointer to that node. If node
 * is not found, NULL is returned */
Node *findNode( Node *root, char *categoryName )
{
	if( root == NULL )
		return NULL;
		
	if( !strcmp( root->categoryName, categoryName ) )
		return root;

	Node *temp;

	root = root->childNode;
	while( root != NULL )
	{
		if( !strcmp( root->categoryName, categoryName ) )
			return root;

		temp = findNode( root, categoryName );
		if( temp != NULL )
			return temp;
		root = root->siblingNode;
	}

	return NULL;
}

/* loadData: reads the data which is contained in the file
 * whose name is passed as a parameter. Returns pointer to
 * the top node; it's root of the tree */
Node *loadData( char *dataFilename )
{
	FILE *dataFile;
	Node *head, *parentNode, *newNode;
	char *currLinePosition, categoryName[NAME_LEN+1], line[LINE_LEN+1];	

	head = parentNode = newNode = NULL;

	dataFile = fopen( dataFilename, "r" );
	checkPointer( (void *) dataFile, "fopen" );

	/* Add top category into the tree */
	fgets( line, LINE_LEN, dataFile );
	sscanf( line, "%s", categoryName );
	newNode = createNode( categoryName);
	addChild( &head, newNode );

	currLinePosition = line + strlen( categoryName ) + 1;

	/* Add direct subcategories of the top category to the tree */
	while( sscanf( currLinePosition, "%s", categoryName ) == 1 )
	{
		newNode = createNode( categoryName );
		addChild( &head, newNode );
		
		currLinePosition += strlen( categoryName ) + 1;
	}

	/* Read rest of the file and add categories from it to the tree */
	while( fgets( line, LINE_LEN, dataFile ) != 0 )
	{
		sscanf( line, "%s", categoryName );
		parentNode = findNode( head, categoryName );
		if( parentNode == NULL )
		{
			fprintf( stderr, "Error: invalid category\n");
			return NULL;
		}

		currLinePosition = line + strlen( categoryName ) + 1;
		
		while( sscanf( currLinePosition, "%s", categoryName ) == 1 )
		{
			newNode = createNode( categoryName );
			addChild( &parentNode, newNode );
			
			currLinePosition += strlen( categoryName ) + 1;
		}
	}

	fclose( dataFile );

	return head;
}
/* getParent: returns pointer to the parten node, if given node
 * isn't top node (root of the tree), otherwise, it returnes NULL */
Node *getParent( Node *n )
{
	if( n == NULL )
		return NULL;
	return n->parentNode;
}

/* getChildren: returns linked list of child nodes for given parent node,
 * if that node has any child node, if not, NULL is returned */
Node *getChildren( Node *n )
{
	if( n == NULL )
		return NULL;
	return n->childNode;
}

void DirectSupercategory( Node *tree, char (*buffers)[OP_LEN+1] )
{
	Node *tempNode = findNode( tree, buffers[1] );
	if( tempNode == NULL )
	{
		fprintf( stderr, "DirectSupercategory: given category (%s) doesn't exist\n", buffers[1] );
		return;
	}
	
	tempNode = getParent( tempNode );
	if( tempNode )
		printf( "%s %s %s\n", buffers[0], buffers[1], tempNode->categoryName );
	else
		printf( "%s %s\n", buffers[0], buffers[1] );
}

void AllSupercategories( Node *tree, char (*buffers)[OP_LEN+1] )
{
	Node *tempNode = findNode( tree, buffers[1] );
	if( tempNode == NULL )
	{
		fprintf( stderr, "AllSupercategories: given category (%s) doesn't exist\n", buffers[1] );
		return;
	}

	printf( "%s %s", buffers[0], buffers[1] );
	
	tempNode = getParent( tempNode );
	while( tempNode != NULL )
	{
		printf( " %s", tempNode->categoryName );
		tempNode = getParent( tempNode );
	}
	
	printf( "\n" );
}

void NumberOfAllSupercategories( Node *tree, char (*buffers)[OP_LEN+1] )
{
	int i = 0;
	Node *tempNode = findNode( tree, buffers[1] );
	if( tempNode == NULL )
	{
		fprintf( stderr, "NumberOfAllSupercategories: given category (%s) doesn't exist\n", buffers[1] );
		return;
	}
	
	tempNode = getParent( tempNode );
	while( tempNode != NULL )
	{
		i++;
		tempNode = getParent( tempNode );
	}
	printf( "%s %s %d\n", buffers[0], buffers[1], i );
}

/* IsSupercategory: checks if the second category is supercategory of the first category
 * 
 * Query syntax: IsSupercategory cat1 cat2
 * Meaning: cat2 IsSupercategory (of) cat1 ? */
void IsSupercategory( Node *tree, char (*buffers)[OP_LEN+1] )
{
	Node *tempNode = findNode( tree, buffers[1] );
	if( tempNode == NULL )
	{
		fprintf( stderr, "IsSupercategory: given category (%s) doesn't exist\n", buffers[1] );
		return;
	}
	
	while( tempNode != NULL )
	{
		if( !strcmp( tempNode->categoryName, buffers[2] ) )
			break;

		tempNode = getParent( tempNode );
	}
	
	if( tempNode )
		printf( "%s %s %s %s\n", buffers[0], buffers[1], buffers[2], "yes" );
	else
		printf( "%s %s %s %s\n", buffers[0], buffers[1], buffers[2], "no" );
}

/* IsSubcategory: checks if the first category is subcategory of the second category
 * 
 * Query syntax: IsSubcategory cat1 cat2
 * Meaning: cat1 IsSubcategory (of) cat2? */
void IsSubcategory( Node *tree, char (*buffers)[OP_LEN+1] )
{
	Node *tempNode = findNode( tree, buffers[2] );
	if( tempNode == NULL )
	{
		fprintf( stderr, "IsSubcategory: given category (%s) doesn't exist\n", buffers[1] );
		return;
	}

	/* search tree of second category name (it's subcategories ) */
	tempNode = findNode( tempNode, buffers[1] );

	if( tempNode )
		printf( "%s %s %s %s\n", buffers[0], buffers[1], buffers[2], "yes" );
	else
		printf( "%s %s %s %s\n", buffers[0], buffers[1], buffers[2], "no" );		
}

void ClosestCommonSupercategory( Node *tree, char (*buffers)[OP_LEN+1] )
{
	Node *tempNode1, *tempNode2;

	tempNode1 = findNode( tree, buffers[1] );
	if( tempNode1 == NULL )
	{
		fprintf( stderr, "ClosestCommonSupercategory: given category (%s) doesn't exist\n", buffers[1] );
		return;
	}
	
	tempNode2 = findNode( tree, buffers[2] );
	if( tempNode2 == NULL )
	{
		fprintf( stderr, "ClosestCommonSupercategory: given category (%s) doesn't exist\n", buffers[2] );
		return;
	}

	/* Move pointer of deeper node to the same level (depth)
	 * as another node, so they can move together towards
	 * the same "ancestor" */
	while( tempNode1->depth != tempNode2->depth ) 
	{
		if( tempNode1->depth > tempNode2->depth )
		{
			tempNode1 = getParent( tempNode1 );
			if( !tempNode1 )
				break;
		}

		if( tempNode1->depth < tempNode2->depth )
		{
			tempNode2 = getParent( tempNode2 );
			if( !tempNode2 )
				break;
		}
	}

	/* Move pointers until closest "ancestor" is found  */
	while( tempNode1 != tempNode2 )
	{
		tempNode1 = getParent( tempNode1 );
		tempNode2 = getParent( tempNode2 );
	}
	
	printf( "%s %s %s %s\n", buffers[0], buffers[1], buffers[2], tempNode1->categoryName );
}

void DirectSubcategories( Node *tree, char (*buffers)[OP_LEN+1] )
{
	Node *tempNode = findNode( tree, buffers[1] );
	if( tempNode == NULL )
	{
		fprintf( stderr, "DirectSubcategories: given category (%s) doesn't exist\n", buffers[1] );
		return;
	}
	
	printf( "%s %s", buffers[0], buffers[1] );
	tempNode = getChildren( tempNode );
	if( tempNode == NULL )
		printf("\n");
	else
	{
		while( tempNode != NULL )
		{
			printf(" %s", tempNode->categoryName );
			tempNode = tempNode->siblingNode;
		}
		
		printf("\n");
	}
}

/* preOrderPrint: prints category names of passed tree in pre-order */
void preOrderPrint( Node *tree )
{
	if( tree == NULL )
		return;

	printf( " %s", tree->categoryName );

	if( tree->childNode != NULL )
		preOrderPrint( tree->childNode );

	tree = tree->childNode;
	while( tree && tree->siblingNode != NULL )
	{
		preOrderPrint( tree->siblingNode );
		tree = tree->siblingNode;
	}
}

void AllSubcategories( Node *tree, char (*buffers)[OP_LEN+1] )
{
	Node *tempNode = findNode( tree, buffers[1] );
	if( tempNode == NULL )
	{
		fprintf( stderr, "AllSubcategories: given category (%s) doesn't exist\n", buffers[1] );
		return;
	}

	printf("%s %s", buffers[0], buffers[1] );

	tempNode = tempNode->childNode;
	while( tempNode )
	{
		preOrderPrint( tempNode );
		tempNode = tempNode->siblingNode;
	}
	
	printf("\n");
}

/* countSubcategories: counts number of subcategories of passed node */
int countSubcategories( Node *tree )
{
	if( tree == NULL )
		return 0;

	int num = 0;
	Node *tempNode;

	if( tree->childNode )
	{
		tempNode = tree->childNode ;

		while( tempNode )
		{
			num += 1 + countSubcategories( tempNode );
			tempNode = tempNode->siblingNode ;
		}
	}

	return num;
}

void NumberOfAllSubcategories( Node *tree, char (*buffers)[OP_LEN+1] )
{
	Node *tempNode = findNode( tree, buffers[1] );
	if( tempNode == NULL )
	{
		fprintf( stderr, "NumberOfAllSubcategories: given category (%s) doesn't exist\n", buffers[1] );
		return;
	}
	
	printf("%s %s %d\n", buffers[0], buffers[1], countSubcategories(tempNode) );
}

void processQueries( Node *tree, char (*buffers)[OP_LEN+1] )
{
	if( !strcmp( buffers[0], "DirectSupercategory" ) )
		DirectSupercategory( tree, buffers );
				
	else if( !strcmp( buffers[0], "AllSupercategories" ) )
		AllSupercategories( tree, buffers );
		
	else if( !strcmp( buffers[0], "NumberOfAllSupercategories" ) )
		NumberOfAllSupercategories( tree, buffers );
		
	else if( !strcmp( buffers[0], "IsSupercategory" ) )
		IsSupercategory( tree, buffers );

	else if( !strcmp( buffers[0], "IsSubcategory" ) )
		IsSubcategory( tree, buffers );
		
	else if( !strcmp( buffers[0], "ClosestCommonSupercategory" ) )
		ClosestCommonSupercategory( tree, buffers );
	
	else if( !strcmp( buffers[0], "DirectSubcategories" ) )
		DirectSubcategories( tree, buffers );

	else if( !strcmp( buffers[0], "DirectSubcategories" ) )
		DirectSubcategories( tree, buffers );

	else if( !strcmp( buffers[0], "AllSubcategories" ) )
		AllSubcategories( tree, buffers );

	else if( !strcmp( buffers[0], "NumberOfAllSubcategories" ) )
		NumberOfAllSubcategories( tree, buffers );
}

/* answerQueries: prints queries and answers to the screen
 * Takes tree, array which contains names of the files with queries and
 * how many of them are passed, as parameters*/
void answerQueries( Node *tree, char **queryFilenames, int nquery )
{
	int i, j;
	FILE *inputFile;
	char buffers[MAX_OPS][OP_LEN+1];
	char line[LINE_LEN+1];
	char *currLinePosition;

	
	for(i = 0; i < nquery; i++)
	{
		inputFile = fopen( queryFilenames[i], "r" );
		if( !inputFile )
			printf("fopen: failed\n");
		while( fgets( line, LINE_LEN, inputFile ) != 0 )
		{
			j = 0;
			
			/* Get operation */
			if( sscanf( line, "%s", buffers[j] ) != 1 )
				continue;
			
			currLinePosition = line + strlen( buffers[j] ) + 1;

			/* Get operands */
			while( sscanf( currLinePosition, "%s", buffers[++j] ) == 1 )
				currLinePosition += strlen( buffers[j] ) + 1;

			processQueries( tree, buffers );
		}

		fclose( inputFile );
	}
}

int main( int argc, char **argv )
{
	/* Check if user passed filenames for data and, at least, one query file */
	if( argc < 3 )
	{
		printf("Usage: categorytree data_filename query_filename [query_filename...]\n");
			
		return EXIT_FAILURE;
	}

	Node *root = NULL;
	
	char *dataFilename = argv[1];

	/* Allocate memory for the array of query file names */
	char **queryFilenames = (char**) malloc( (argc-2) * sizeof(char *) );
	checkPointer( (void *) queryFilenames, "malloc" );

	int i;

	for(i = 2; i < argc; i++)
	{
		/* Check if filename length is greater then allowed lenght */
		if( strlen( argv[i] ) > NAME_LEN )
		{
			fprintf( stderr, "categorytree: filename of %d. query file is too long\n", i-1 );
			return EXIT_FAILURE;
		}
		else
		{
			/* Allocate memory for a name of the query file */
			queryFilenames[i - 2] = (char *) malloc( NAME_LEN + 1 );
			checkPointer( (void *) queryFilenames[i - 2], "malloc" );
			
			strcpy( queryFilenames[i - 2], argv[i] );
		}
	}

	root = loadData( dataFilename );
	answerQueries( root, queryFilenames, argc-2 );

	return EXIT_SUCCESS;
}
