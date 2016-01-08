/**
 * @file quadtree.cpp
 * quadtree class implementation.
 * @date Spring 2008
 * @date Modified Summer 2014
 */

#include "quadtree.h"
#include <cmath>

using namespace std;

namespace cs225
{

quadtree::quadtree ()
{
	cout << "quadtree::quadtree ()" << endl;
	root_ = nullptr;
}

quadtree::quadtree (const epng::png &source, uint64_t resolution)
{
	cout << "quadtree::quadtree (const epng::png &source, uint64_t resolution)" << endl;
	root_ = nullptr;
	build_tree(source, resolution);
}

quadtree::quadtree (const quadtree &other)
{
	cout << "quadtree::quadtree (const quadtree &other)" << endl;
	root_ = copy(other.root_.get());
}

quadtree::quadtree (quadtree &&other)
{
	cout << "quadtree::quadtree (quadtree &&other)" << endl;
	std::unique_ptr<node> n;
	root_ = std::move(n);
	swap(other);
}

auto quadtree::copy (node *  subroot) -> std::unique_ptr<node>
{
	if (!subroot)
		return nullptr;
	
	auto ret = std::make_unique<node>(subroot->res, subroot->element);
	ret->northwest = copy(subroot->northwest.get());
	ret->northeast = copy(subroot->northeast.get());
	ret->southwest = copy(subroot->southwest.get());
	ret->southeast = copy(subroot->southeast.get());

	return ret;
}

quadtree & quadtree::operator= (quadtree other)
{
	cout << "quadtree & quadtree::operator= (quadtree other)" << endl;
	swap(other);

	return *this;
}


void quadtree::swap (quadtree &other)
{
	cout << "void quadtree::swap (quadtree &other)" << endl;
	std::swap(other.root_, root_);
}

void quadtree::build_tree (const epng::png &source, uint64_t resolution)
{
	epng::rgba_pixel elem;
	root_ = nullptr;
	root_ = std::make_unique<node>(resolution, elem);
	build_tree_recursive(source, resolution, root_.get(), 0, 0);
}

void quadtree::build_tree_recursive (const epng::png &source, const uint64_t res_, node * subroot, const uint64_t x_, const uint64_t y_)
{
	
	epng::rgba_pixel pix;
	
	// base case : leaf
	if (res_ == 1) {
		subroot->element = *source(x_, y_);
		subroot->northwest = nullptr;
		subroot->northeast= nullptr;
		subroot->southwest = nullptr;
		subroot->southeast = nullptr;
		return;
	}
	// recursive calls
	else {

		subroot->northwest = std::make_unique<quadtree::node>(res_/2, pix);
		subroot->northeast= std::make_unique<quadtree::node>(res_/2, pix);
		subroot->southwest= std::make_unique<quadtree::node>(res_/2, pix);
		subroot->southeast= std::make_unique<quadtree::node>(res_/2, pix);
		build_tree_recursive(source, res_/2, subroot->northwest.get(), x_, y_);
		build_tree_recursive(source, res_/2, subroot->northeast.get(), x_+res_/2, y_);
		build_tree_recursive(source, res_/2, subroot->southwest.get(), x_, y_+res_/2);
		build_tree_recursive(source, res_/2, subroot->southeast.get(), x_+res_/2, y_+res_/2);
		subroot->res = res_;
		average_children(subroot);
	}
}

void quadtree::average_children (node * subroot)
{
	
	// either no child or 4
	if (subroot->northwest) {
		//cout << "average_children::has child" << endl;
		epng::rgba_pixel ave;

		//node * tmp = subroot->northwest.get();

		//cout << "here" << endl;
		ave.red = ( subroot->northwest->element.red
				  + subroot->northeast->element.red
				  + subroot->southwest->element.red
				  + subroot->southeast->element.red ) /4;

		ave.green = ( subroot->northwest->element.green
				  + subroot->northeast->element.green 
				  + subroot->southwest->element.green 
				  + subroot->southeast->element.green ) /4;

		ave.blue = ( subroot->northwest->element.blue 
				  + subroot->northeast->element.blue 
				  + subroot->southwest->element.blue 
				  + subroot->southeast->element.blue ) /4;

		ave.alpha = ( subroot->northwest->element.alpha 
				  + subroot->northeast->element.alpha 
				  + subroot->southwest->element.alpha 
				  + subroot->southeast->element.alpha ) /4;
		
		//cout << ave << endl;
		subroot->element = ave;

	}
}

const epng::rgba_pixel & quadtree::quadtree::operator() (uint64_t x, uint64_t y) const
{
	//cout << "const epng::rgba_pixel & quadtree::quadtree::operator() (uint64_t x, uint64_t y) const" << endl;
	if (!root_) throw std::runtime_error{"quadtree is empty"};
	if ((x>=root_->res) || (y>=root_->res)) throw std::out_of_range{"(x,y) is out of range"};

	return get_pixel(root_.get(), x, y, root_->res, 0, 0);
}

epng::rgba_pixel& quadtree::get_pixel(node * subroot, uint64_t x_, uint64_t y_, uint64_t res_, uint64_t x, uint64_t y) const
{
	// base case
	if (!subroot->northwest) {
		return subroot->element; 

	} else {

		uint64_t d = res_/2;
		if ((x_ < x+d) && (y_ < y+d)) 
			return get_pixel(subroot->northwest.get(), x_, y_, d, x, y);
		else if ((x_ >= x+d) && (y_ < y+d))
			return get_pixel(subroot->northeast.get(), x_, y_, d, x+d, y);
		else if ((x_ < x+d) && (y_ >= y+d))
			return get_pixel(subroot->southwest.get(), x_, y_, d, x, y+d);
		else
			return get_pixel(subroot->southeast.get(), x_, y_, d, x+d, y+d);
	}
}

epng::png quadtree::decompress () const
{
	if (!root_) throw std::runtime_error{"empty tree"};

	epng::png canvas(root_->res, root_->res);
	paint(canvas, root_.get(), 0, 0, root_->res);	

	//cout << "returning canvas" << endl;
	return canvas;
}

void quadtree::paint(epng::png& canvas_, node* subroot, uint64_t x_, uint64_t y_, uint64_t res_) const
{
	if (!subroot) return;
	
	for (uint64_t i=0; i<res_; i++) {
		for (uint64_t j=0; j<res_; j++) {
			*canvas_(i, j) = get_pixel(root_.get(), i, j, res_, 0, 0);
		}
	}
}

void quadtree::rotate_children(node* subroot)
{
	if (!subroot) return;
	if (subroot->res == 1) return;

	std::swap(subroot->northwest, subroot->northeast);
	std::swap(subroot->southwest, subroot->southeast);
	std::swap(subroot->northwest, subroot->southeast);

	rotate_children(subroot->northwest.get());
	rotate_children(subroot->northeast.get());
	rotate_children(subroot->southwest.get());
	rotate_children(subroot->southeast.get());
}
	
void quadtree::rotate_clockwise ()
{
	rotate_children(root_.get());
}

void quadtree::prune (uint32_t tolerance)
{
	prune_recursive(tolerance, root_.get());
}

void quadtree::prune_recursive (uint32_t tol_, node* subroot)
{
	if (!(subroot->northwest)) return;

	uint32_t tol = get_tolerance(subroot, subroot);

	if (tol <= tol_) {
		subroot->northwest = nullptr;
		subroot->northeast = nullptr;
		subroot->southwest = nullptr;
		subroot->southeast = nullptr;
		return;
	} else {
		prune_recursive(tol_, subroot->northwest.get());
		prune_recursive(tol_, subroot->northeast.get());
		prune_recursive(tol_, subroot->southwest.get());
		prune_recursive(tol_, subroot->southeast.get());
	}
}

uint32_t quadtree::get_tolerance (node* subroot, node* comp) const
{
	if (!(subroot->northwest)) {
		return get_pix_diff(subroot->element, comp->element);
	}

	uint32_t nw, ne, sw, se;
	nw = get_tolerance(subroot->northwest.get(), comp);
	ne = get_tolerance(subroot->northeast.get(), comp);
	sw = get_tolerance(subroot->southwest.get(), comp);
	se = get_tolerance(subroot->southeast.get(), comp);

	if (nw < ne) nw = ne;
	if (nw < sw) nw = sw;
	if (nw < se) nw = se;
	return nw;


}

uint32_t quadtree::get_pix_diff (const epng::rgba_pixel& pix1, const epng::rgba_pixel& pix2) const
{
	int a_red;
	int a_green;
	int a_blue;
	a_red = (pix1.red - pix2.red);
	a_green = (pix1.green - pix2.green);
	a_blue = (pix1.blue - pix2.blue);

	int ret = pow(a_red, 2) + pow(a_green, 2) + pow(a_blue, 2);
	return ret;
}

uint64_t quadtree::pruned_size (uint32_t tolerance) const
{
	uint64_t ret = pruned_size_recursive(tolerance, root_.get());
	return ret;
}

uint64_t quadtree::pruned_size_recursive(uint32_t tol_, node* subroot) const
{
	if (!(subroot->northwest)) return 1;

	uint32_t tol = get_tolerance(subroot, subroot);
	if (tol <= tol_)
		return 1;
	
	uint64_t ret = pruned_size_recursive(tol_, subroot->northwest.get())
					+ pruned_size_recursive(tol_, subroot->northeast.get())
					+ pruned_size_recursive(tol_, subroot->southwest.get())
					+ pruned_size_recursive(tol_, subroot->southeast.get());
	return ret;

}

uint32_t quadtree::ideal_prune (uint64_t num_leaves) const
{
	// start with maximum tolerance and reduce it down by binary search
	return find_tolerance(num_leaves, 0, 255*255*3);
}

uint32_t quadtree::find_tolerance(uint64_t num_leaves, uint32_t min, uint32_t max) const
{
	uint32_t d = (max+min)/2;

	if (pruned_size(d) > num_leaves) {
		return find_tolerance(num_leaves, d, max);
	}
	else if (pruned_size(d) < num_leaves) {
		return find_tolerance(num_leaves, min, d);
	}
	else {
		while (pruned_size(d) == num_leaves) {
			d--;
		}
		return d+1;
	}

}

}
