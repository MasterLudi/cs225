/**
 * @file quadtree.h
 * quadtree class definition.
 * @date Spring 2008
 * @date Modified Summer 2014
 */

#ifndef QUADTREE_H_
#define QUADTREE_H_

#include <iostream>
#include "epng.h"

namespace cs225
{

/**
 * A tree structure that is used to compress epng::png images.
 */
class quadtree
{
  public:
	void cout_average (uint64_t x, uint64_t y);

	/* The no parameters constructor takes no arguments, and produces an empty quadtree object, 
	 * i.e.one which has no associated node objects, and in which root_ is nullptr.
	 * */
	quadtree ();

	/* This cons<tructor's purpose is to build a quadtree representing the upper-left d by d 
	 * block of the source image.This effectively crops the source image into a d by d 
	 * square.You may assume that d is a power of two, and that the width and height of 
	 * source are each at least d.Perhaps, to implement this, you could leverage the 
	 * functionality of another function you have written in mp_qtree.1
	 * Parameters
	 * source	
	 * The source image to base this quadtree on
	 * resolution	The width and height of the sides of the image to be represented
	 * */

 	quadtree (const epng::png &source, uint64_t resolution);
 
	/* Copy constructor.Simply sets this quadtree to be a copy of the parameter.
	 * Parameters
	 * other	The quadtree to make a copy of
	 * */
 	quadtree (const quadtree &other);
 
	/* Move constructor.
	 * Parameters
	 * other	The quadtree to move into this one. Should be left empty.
	 * */
 	quadtree (quadtree &&other);
 
	/* Destructor; frees all memory associated with this quadtree.
	 * This should be able to be defaulted.
	 * */
 	~quadtree ()=default;

	/* Assignment operator; frees memory associated with this quadtree and sets its 
	 * contents to be equal to the parameter's.
	 * Parameters
	 * other	The quadtree to make a copy of
	 *
	 * Returns
	 * A reference to the current quadtree
	 * */
	quadtree & operator= (quadtree other);
 
	/* Swaps the current quadtree with the given one.	
	 * Parameters	
	 * other	The tree to swap with
	 * */
	void swap (quadtree &other);

/*Deletes the current contents of this quadtree object, then turns it into a quadtree object representing the upper-left d by d block of source.*/
/*You may assume that d is a power of two, and that the width and height of source are each at least d.*/
/*Parameters*/
/*source	The source image to base this quadtree on*/
/*resolution	The width and height of the sides of the image to be represented*/
void build_tree (const epng::png &source, uint64_t resolution);

	/* Gets the epng::rgba_pixel corresponding to the pixel at coordinates (x, y) 
	 * in the image which the quadtree represents.	Note that the quadtree may not 
	 * contain a node specifically corresponding to this pixel (due, for instance, 
	 * to pruning - see below). In this case, operator() will retrieve the pixel 
	 * (i.e. the color) of the square region within which the smaller query grid cell 
	 * would lie. (That is, it will return the element of the nonexistent leaf's deepest 
	 * surviving ancestor.)	If the supplied coordinates fall outside of the bounds of the 
	 * underlying image, or if the current quadtree is "empty" (i.e., it was created by
	 * the default constructor) a std::out_of_range exception should be thrown.	
	 *
	 * Parameters	
	 * x	The x coordinate of the pixel to be retrieved	
	 * y	The y coordinate of the pixel to be retrieved	
	 *
	 * Returns	The pixel at the given (x, y) location
	 * */
	const epng::rgba_pixel & operator() (uint64_t x, uint64_t y) const;

	/* Returns the underlying epng::png object represented by the quadtree.		
	 * This function effectively "decompresses" the quadtree. A quadtree object, 
	 * in memory, may take up less space than the underlying bitmap image, but we 
	 * cannot simply look at the quadtree and tell what image it represents. 
	 * By converting the quadtree back into a bitmap image, we lose the compression, 
	 * but gain the ability to view the image directly.		
	 * If the current quadtree is "empty" (i.e., it was created by the default constructor), 
	 * a std::runtime_error should be thrown.		
	 *
	 * Returns 		The decompressed epng::png image this quadtree represents
	 * */
		epng::png decompress () const;

	/* Rotates the quadtree object's underlying image clockwise by 90 degrees. 
	 * (Note that this should be done using pointer manipulation, not by 
	 * attempting to swap the element fields of nodes.
	 * */
	void rotate_clockwise ();

	/* Compresses the image this quadtree represents. If the color values 
	 * of the leaves of a subquadtree don't vary by much, we might as well 
	 * represent the entire subtree by, say, the average color value of 
	 * those leaves. We may use this information to effectively "compress" 
	 * the image, by strategically trimming the quadtree.		Consider a 
	 * node \(n\) and the subtree, \(T_n\) rooted at \(n\), and let \(avg\) 
	 * denote the component-wise average color value of all the leaves of \(T_n\). 
	 * Component-wise average means that every internal node in the tree 
	 * calculates its value by averaging its immediate children. This implies 
	 * that the average must be calculated in a "bottom-up" manner.
	 * Due to rounding errors, using the component-wise average is not equivalent 
	 * to using the true average of all leaves in a subtree. If a node \(n\) is 
	 * pruned, the children of \(n\) and the subtrees for which they are the roots
	 * are removed from the quadtree. Node \(n\) is pruned if the color value of 
	 * no leaf in \(T_n\), differs from \(avg\) by more than tolerance.
	 * (Note: for all average calculations, just truncate the value to integer.)
	 * 		We define the "difference" between two colors, \((r_1, g_1, b_1)\) 
	 * 	and \((r_2, g_2, b_2)\), to be \((r_2 - r_1)^2 + (g_2 - g_1)^2 + (b_2 - b_1)^2\).
	 * 	To be more complete, if the tolerance condition is met at a node \(n\), then 
	 * 	the block of the underlying image which \(n\) represents contains only pixels 
	 * 	which are "nearly" the same color. For each such node \(n\), we remove from 
	 * 	the quadtree all four children of \(n\), and their respective subtrees (an 
	 * 	operation we call a pruning). This means that all of the leaves that were 
	 * 	deleted, corresponding to pixels whose colors were similar, are now replaced 
	 * 	by a single leaf with color equal to the average color over that square region.	
	 * 	The prune function, given a tolerance value, prunes the quadtree as extensively 
	 * 	as possible. (Note, however, that we do not want the prune function to do an 
	 * 	"iterative" prune. It is conceivable that by pruning some mid-level node \(n\), 
	 * 	an ancestor \(p\) of \(n\) then becomes prunable, due to the fact that the prune 
	 * 	changed the leaves descended from \(p\). Your prune function should evaluate the 
	 * 	prunability of each node based on the presence of all nodes, and then delete the
	 * 	subtrees based at nodes deemed prunable.)		
	 * 	Note* 		You should start pruning from the root of the quadtree.		
	 *
	 * 	Parameters		
	 * 	tolerance	The integer tolerance between two nodes that determines 
	 * 				whether the subtree can be pruned.*/
	void prune (uint32_t tolerance);

/* This function is similar to prune; however, it does not actually prune the quadtree.
 * Rather, it returns a count of the total number of leaves the quadtree would have if 
 * it were pruned as in the prune function.
 *
 * Parameters
 * tolerance	The integer tolerance between two nodes that determines whether the subtree can be pruned.
 *
 * Returns
 * How many leaves this quadtree would have if it were pruned with the given tolerance. */
	uint64_t pruned_size (uint32_t tolerance) const;

/* Calculates and returns the minimum tolerance necessary to guarantee that upon pruning the tree, 
 * no more than num_leaves leaves remain in the quadtree. Essentially, this function is an inverse 
 * of pruned_size; for any quadtree object tree, and for any positive integer tolerance it should 
 * be true that tree.pruned_size(tree.ideal_prune(num_leaves)) <= num_leaves. Once you understand 
 * what this function is supposed to do, you will probably notice that there is an "obvious" 
 * implementation. This is probably not the implementation you want to use! There is a fast way to 
 * implement this function, and a slow way; you will need to find the fast way. (If you doubt that 
 * it makes a significant difference, the tests in the given main.cpp should convince you.) 
 *
 * Parameters 
 * num_leaves	
 * The number of leaves you want to remain in the tree after prune is called. 
 *
 * Returns 
 * The minimum tolerance needed to guarantee that there are no more than num_leaves remaining in the tree.
 * Note The "obvious" implementation involves a sort of linear search over all possible tolerances. 
 * What if you tried a binary search instead?
 * */
 	uint32_t ideal_prune (uint64_t num_leaves) const;
 
 	private:
    /**
     * A simple class representing a single node of a quadtree.
     * You may want to add to this class; in particular, it could
     * probably use a constructor or two...
     */
    class node
    {
      public:

        std::unique_ptr<node> northwest;
        std::unique_ptr<node> northeast;
        std::unique_ptr<node> southwest;
        std::unique_ptr<node> southeast;

        uint64_t x;
        uint64_t y;
        uint64_t res;

        epng::rgba_pixel element; // the pixel stored as this node's "data"

        // constructor
      	node(uint64_t res_, epng::rgba_pixel elem_) 
		{ 
			/*x = x_;*/
			/*y = y_;*/
			res = res_;
			element = elem_;
		};
    };

    std::unique_ptr<node> root_; // the root of the tree

	/*uint64_t size;*/

    /**
     * recursive helper function for build_tree 
     */
	void build_tree_recursive (const epng::png &source, const uint64_t resolution, node * subroot, const uint64_t x_, const uint64_t y_);
	/*auto build_tree_helper (const epng::png &source, const uint64_t res_) -> std::unique_ptr<node>;*/

	void average_children (node * subroot);

	auto copy (node *  subroot) -> std::unique_ptr<node>;

	epng::rgba_pixel& get_pixel(node * subroot, uint64_t x_, uint64_t y_, uint64_t res_, uint64_t x, uint64_t y) const;

	void paint(epng::png& canvas_, node* subroot, uint64_t x_, uint64_t y_, uint64_t res_) const;

	void rotate_children(node* subroot);
	
	uint32_t get_pix_diff(const epng::rgba_pixel& pix1, const epng::rgba_pixel& pix2) const;

	void prune_recursive(uint32_t tol_, node* subroot);

	/*uint32_t get_tolerance (node* subroot, node* child, node* ancestor) const;*/
	uint32_t get_tolerance (node* subroot, node* comp) const;

	/*bool get_tolerance (node* subroot, node* child, uint32_t tol) const;*/

	uint64_t pruned_size_recursive(uint32_t tol_, node* subroot) const;

	uint32_t find_tolerance(uint64_t num_leaves, uint32_t min, uint32_t max) const;

/**** Do not remove this line or copy its contents here! ****/ 
#include "quadtree_given.h" 
}; 
} 

#endif
