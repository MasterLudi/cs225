/**
 * @file avl_tree.cpp
 * Definitions of the binary tree functions you'll be writing for this lab.
 * You'll need to modify this file.
 */

#include "avl_tree.h"

template <class K, class V>
const V& avl_tree<K, V>::find(const K& key) const {
    return find(root_.get(), key);
}

template <class K, class V>
const V& avl_tree<K, V>::find(const node* subtree, const K& key) const {
    if (!subtree)
    {
        throw std::out_of_range{"invalid key"};
    }
    else if (key == subtree->key)
    {
        return subtree->value;
    }
    else if (key < subtree->key)
    {
        return find(subtree->left.get(), key);
    }
    else
    {
        return find(subtree->right.get(), key);
    }
}

template <class K, class V>
void avl_tree<K, V>::rotate_left(std::unique_ptr<node>& t)
{
    *_out << __func__ << endl; // Outputs the rotation name (don't remove this)
	auto pivot = std::move(t->right);
	t->right = std::move(pivot->left);
	std::swap(t, pivot);
	
	pivot->height = heightOrNeg1(pivot.get()) - 1;
	t->height = heightOrNeg1(t.get()) + 1;

	t->left = std::move(pivot);

}

template <class K, class V>
void avl_tree<K, V>::rotate_left_right(std::unique_ptr<node>& t)
{
    *_out << __func__ << endl; // Outputs the rotation name (don't remove this)
    // Implemented for you:
    rotate_left(t->left);
    rotate_right(t);
}

template <class K, class V>
void avl_tree<K, V>::rotate_right(std::unique_ptr<node>& t)
{
    *_out << __func__ << endl; // Outputs the rotation name (don't remove this)
    /// @todo Your code here
	auto pivot = std::move(t->left);
	t->left = std::move(pivot->right);
	std::swap(t, pivot);

	pivot->height = heightOrNeg1(pivot.get()) - 1;
	t->height = heightOrNeg1(t.get()) + 1;

	t->right = std::move(pivot);


}

template <class K, class V>
void avl_tree<K, V>::rotate_right_left(std::unique_ptr<node>& t)
{
    *_out << __func__ << endl; // Outputs the rotation name (don't remove this)
    /// @todo Your code here
	rotate_right(t->right);
	rotate_left(t);
}

template <class K, class V>
void avl_tree<K, V>::insert(K key, V value)
{
    insert(root_, std::move(key), std::move(value));
}

template <class K, class V>
void avl_tree<K, V>::insert(std::unique_ptr<node>& subtree, K key, V value)
{
	if (!subtree) {
            subtree = std::unique_ptr<node>{new node{std::move(key), std::move(value)}};
	}
	else if (key < subtree->key)
		insert(subtree->left, key, value);
	else
		insert(subtree->right, key, value);
	
	rebalance(subtree);
}

template <class K, class V>
void avl_tree<K, V>::rebalance(std::unique_ptr<node>& subroot)
{
	auto balance = heightOrNeg1(subroot->left.get()) - heightOrNeg1(subroot->right.get());
	if (balance == 2)
		rebalance_left(subroot);
	else if (balance == -2)
		rebalance_right(subroot);
	subroot->height = 1 + std::max(heightOrNeg1(subroot->left.get()), heightOrNeg1(subroot->right.get()));
}

template <class K, class V>
void avl_tree<K, V>::rebalance_left(std::unique_ptr<node>& subroot)
{
	auto balance = heightOrNeg1(subroot->left->left.get()) - heightOrNeg1(subroot->left->right.get());
	if (balance == 1)
		rotate_right(subroot);
	else
		rotate_left_right(subroot);
}

template <class K, class V>
void avl_tree<K, V>::rebalance_right(std::unique_ptr<node>& subroot)
{
	auto balance = heightOrNeg1(subroot->right->left.get()) - heightOrNeg1(subroot->right->right.get());
	if (balance == -1)
		rotate_left(subroot);
	else
		rotate_right_left(subroot);
}
