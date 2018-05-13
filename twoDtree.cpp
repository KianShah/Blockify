
/**
 *
 * twoDtree (pa3)
 * slight modification of a Kd tree of dimension 2.
 * twoDtree.cpp
 * This file will be used for grading.
 *
 */

#include "twoDtree.h"
#include <iostream>
#include <cmath>

/* given */
twoDtree::Node::Node(pair<int,int> ul, pair<int,int> lr, RGBAPixel a)
	:upLeft(ul),lowRight(lr),avg(a),left(NULL),right(NULL)
	{}

/* given */
twoDtree::~twoDtree(){
	clear();
}

/* given */
twoDtree::twoDtree(const twoDtree & other) {
	copy(other);
}

/* given */
twoDtree & twoDtree::operator=(const twoDtree & rhs){
	if (this != &rhs) {
		clear();
		copy(rhs);
	}
	return *this;
}

/*
 * Constructor that builds a twoDtree out of the given PNG.
 * Every leaf in the tree corresponds to a pixel in the PNG.
 * Every non-leaf node corresponds to a rectangle of pixels 
 * in the original PNG, represented by an (x,y) pair for the 
 * upper left corner of the rectangle and an (x,y) pair for 
 * lower right corner of the rectangle. In addition, the Node
 * stores a pixel representing the average color over the 
 * rectangle. 
 *
 * Every node's left and right children correspond to a partition
 * of the node's rectangle into two smaller rectangles. The node's
 * rectangle is split by the horizontal or vertical lines that 
 * results in the two smaller rectangles whose sum of squared 
 * differences from their mean is as small as possible.
 *
 * The left child of the node will contain the upper left corner
 * of the node's rectangle, and the right child will contain the
 * lower right corner.
 *
 * This function will build the stats object used to score the 
 * splitting lines. It will also call helper function buildTree.
 */
twoDtree::twoDtree(PNG & imIn){
	stats stat_obj(imIn);
	pair<int,int> ul (0,0);
	pair<int,int> lr (imIn.width() - 1, imIn.height() - 1);

	width = imIn.width();
	height = imIn.height();
	root = buildTree(stat_obj, ul, lr);
}

/*
 * Private helper function for the constructor. Recursively builds
 * the tree according to the specification of the constructor.
 * @param s Contains the data used to split the rectangles
 * @param ul upper left point of current node's rectangle.
 * @param lr lower right point of current node's rectangle.
 */
twoDtree::Node * twoDtree::buildTree(stats & s, pair<int,int> ul, pair<int,int> lr) {
	Node* node =  new Node(ul, lr, s.getAvg(ul, lr));
	
	if (ul == lr)				// base case (Leaf node, with just 1 pixel)
		return node;
	
	pair<int,int> new_ul = ul;
	pair<int,int> new_lr = lr;
	computeUlLr(s, new_ul, new_lr);			// computes new_ul and new_lr, where new_ul is the upper-left coordinate
											// of the right child, and new_lr is the lower-right coordinate of the left child
	node->left = buildTree(s, ul, new_lr);
	node->right = buildTree(s, new_ul, lr);
	return node;
}

// Private helper function for buildTree
// @param s Contains the data used to split the rectangles
// @param ul upper left point of current node's rectangle.
// @param lr lower right point of current node's rectangle.
//
// Invariant: computeUlLr never gets called when (ul.first == lr.first && ul.second == lr.second)
// meaning that computeUlLr never gets called with ul == lr
void twoDtree::computeUlLr(stats & s, pair<int,int>& ul, pair<int,int>& lr) {	
	long lowest_score;
	pair<int,int> lowest_score_lr;
	pair<int,int> lowest_score_ul;
	
	if (ul.first != lr.first)		// Row and general rectangle case
	{
		lowest_score_lr = {ul.first, lr.second};		// Initialize local variables
		lowest_score_ul = {ul.first + 1, ul.second};
		lowest_score = s.getScore(ul, lowest_score_lr) + s.getScore(lowest_score_ul, lr);
		
		for (int x = ul.first+1; x <= lr.first - 1; ++x)	// Scrub along bottom x axis, compute getScores of each 
		{													// respective rectangle. note that we add 1 to initial x because we
			pair<int,int> new_lr (x, lr.second);			// are guaranteed that we have already gone through the first case
			pair<int,int> new_ul (x + 1, ul.second);
			long curr_score = s.getScore(ul, new_lr) + s.getScore(new_ul, lr);
			if (curr_score <= lowest_score)
			{
				lowest_score = curr_score;
				lowest_score_lr = new_lr;
				lowest_score_ul = new_ul;
			}
		}
	}
	
	if (ul.second != lr.second)			// Column and general rectangle case
	{
		if (ul.first == lr.first)		// If we haven't already went through the first if statement
		{								// then initialize the lowest_score, lowest_score_lr, lowest_score_ul
			lowest_score_lr = {lr.first, ul.second};
			lowest_score_ul = {ul.first, ul.second + 1};
			lowest_score = s.getScore(ul, lowest_score_lr) + s.getScore (lowest_score_ul, lr);
		}
		
		for (int y = ul.second; y <= lr.second - 1; ++y)	// Scrub along right-most y axis, compute getScores 
		{													// of each respective rectangle. Note that we aren't
			pair<int,int> new_lr (lr.first, y);				// guaranteed that y has already gone through initial case
			pair<int,int> new_ul (ul.first, y + 1);
			long curr_score = s.getScore(ul, new_lr) + s.getScore(new_ul, lr);
			if (curr_score <= lowest_score)
			{
				lowest_score = curr_score;
				lowest_score_lr = new_lr;
				lowest_score_ul = new_ul;
			}
		}
	}
	
	lr = lowest_score_lr;		// lr coordinate of left-child (ul is constant)
	ul = lowest_score_ul;		// ul coordinate of right_child (lr is constant)
}
	
/**
* Render returns a PNG image consisting of the pixels
* stored in the tree. may be used on pruned trees. Draws
* every leaf node's rectangle onto a PNG canvas using the 
* average color stored in the node.
*/
PNG twoDtree::render(){
	PNG img (width, height);
	Node* curr = root;
	render(img, curr);
	return img;
}

// Recursive helper for render
void twoDtree::render(PNG & img, Node* curr)
{
	if (curr->left == NULL && curr->right == NULL)		// If we've reached a leaf node, set the respective pixel on the
	{													// image to the colour of the average colour of the leaf node
		pair<int,int> ul = curr->upLeft;
		pair<int,int> lr = curr->lowRight;
		
		for (int i = ul.first; i <= lr.first; ++i)
		{
			for (int j = ul.second; j <= lr.second; ++j)
			{
				RGBAPixel* pixel = img.getPixel(i,j);
				*pixel = curr->avg;
			}
		}
	}
	
	else							// If we haven't reached a leaf node, the keep going till we do
	{
		if (curr->left != NULL)
			render(img, curr->left);
		
		if (curr->right != NULL)
			render(img, curr->right);
	}
}

/*
*  Prune function trims subtrees as high as possible in the tree.
*  A subtree is pruned (cleared) if at least pct of its leaves are within 
*  tol of the average color stored in the root of the subtree. 
*  Pruning criteria should be evaluated on the original tree, not 
*  on a pruned subtree. (we only expect that trees would be pruned once.)
*  
* You may want a recursive helper function for this one.
* If pct of the leaf nodes of the tree have an avg color within tol of the node's avg color
* clear all leaf nodes
*/
void twoDtree::prune(double pct, int tol){
	pruneAll(root, pct, tol);
}

// Recursive helper function for prune()
// @param subtree The current node of the recursion
// @param pct The given percentage of leaf nodes that must be prunable to prune the tree
// @param tol The tolerance for the difference between the distance between the colours of the leaf node and root node
void twoDtree::pruneAll(twoDtree::Node* subtree, double pct, int tol)
{
	double leaf_nodes_prunable = 0;
	double leaf_nodes_total = 0;
	shouldBePruned(subtree, subtree->avg, tol, leaf_nodes_prunable, leaf_nodes_total);
	
	if (subtree->left == NULL && subtree->right == NULL)		// If at leaf node
		return;													// Note: we need this because we don't want to divide 
																// by 0, since leaf_nodes_total would return 0.
	else if (leaf_nodes_prunable / leaf_nodes_total >= pct)		// If we can prune the current subtree
	{
		clearAll(subtree->left);
		clearAll(subtree->right);
		subtree->left = NULL;
		subtree->right = NULL;
	}
	
	else														// If we are neither at a leaf node nor can we prune the current subtree
	{
		if (subtree->left != NULL)
			pruneAll(subtree->left, pct, tol);
		
		if (subtree->right != NULL)
			pruneAll(subtree->right, pct, tol);
	}
}

// Private helper function for pruneAll()
// Correctly updates the values for leaf_nodes_prunable and leaf_nodes_total
// and passes them to calling function by reference
// @param subtree The current node in the recursion
// @param root_pixel The average colour of the pixel at the root of the tree
// @param tol The tolerance for the difference between the distance between the colours of the leaf node and root node
// @param leaf_nodes_prunable The number of prunable leaf nodes
// @param leaf_nodes_total The total number of leaf nodes in the subtree
// Note: A leaf node is prunable if the distance between the colours is less than or equal to tol
// Distances between colors are computed as the sum, over each color channel, of the pixel value differences, squared.
//
// INVARIANT: Initial subtree is not a leaf node, so the number of leaf_nodes_total update will never be 0
void twoDtree::shouldBePruned(Node* subtree, RGBAPixel root_pixel, int tol, double & leaf_nodes_prunable, double & leaf_nodes_total)
{
	if (subtree->left == NULL && subtree->right == NULL)	// If we have reached a leaf node, then determine if prunable
	{
		leaf_nodes_total++;
		RGBAPixel subtree_pixel = subtree->avg;
		long red_sqr_dif = (long) pow((double) (root_pixel.r - subtree_pixel.r), 2);
		long green_sqr_dif = (long) pow((double) (root_pixel.g - subtree_pixel.g), 2);
		long blue_sqr_dif = (long) pow((double) (root_pixel.b - subtree_pixel.b), 2);
		long dist_between_colors = red_sqr_dif + green_sqr_dif + blue_sqr_dif;
		
		if (dist_between_colors <= tol)
			leaf_nodes_prunable++;
	}
	
	else
	{
		if (subtree->left != NULL)		// If there is a left child, then it's not a leaf node, so recurse on left child
			shouldBePruned (subtree->left, root_pixel, tol, leaf_nodes_prunable, leaf_nodes_total);
		
		if (subtree->right != NULL)		// If there is a right child, then it's not a leaf node, so recurse on right child
			shouldBePruned (subtree->right, root_pixel, tol, leaf_nodes_prunable, leaf_nodes_total);
	}
}

/**
    * Destroys all dynamically allocated memory associated with the
    * current twoDtree class. Complete for PA3.
   * You may want a recursive helper function for this one.
    */
void twoDtree::clear() {
    clearAll(root);
	width = 0;
	height = 0;
}

// Recursive helper function for clear()
void twoDtree::clearAll(Node* subRoot){
    if (subRoot == NULL)
        return;
	
    else {
        clearAll(subRoot->left);
        clearAll(subRoot->right);
        delete subRoot;
    }
}


/**
   * Copies the parameter orig twoDtree into the current twoDtree.
   * Does not free any memory. Called by copy constructor and op=.
   * You may want a recursive helper function for this one.
   * @param other The twoDtree to be copied.
   */
void twoDtree::copy(const twoDtree & orig){
	root = copyAll(orig.root);
	width = orig.width;
	height = orig.height;
}

// Recursive helper function for copy()
twoDtree::Node* twoDtree::copyAll(Node* tree){
	Node* curr = new Node(tree->upLeft, tree->lowRight, tree->avg);
	
    if (tree->left != NULL)
		curr->left = copyAll(tree->left);
	
	if (tree->right != NULL)
		curr->right = copyAll(tree->right);
	
	return curr;
}