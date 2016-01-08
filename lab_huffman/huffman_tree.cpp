/**
 * @file huffman_tree.cpp
 * Implementation of a Huffman Tree class.
 *
 * @author Chase Geigle
 * @author Chloe Kim
 * @date Summer 2012
 */

#include <algorithm>
#include <iostream>
#include <queue>
#include <utility>

#include "huffman_tree.h"

using namespace std;

huffman_tree::huffman_tree(vector<frequency> frequencies)
{
    std::stable_sort(frequencies.begin(), frequencies.end());
    build_tree(frequencies);
    vector<bool> path;
    build_map(root_.get(), path);
}

huffman_tree::huffman_tree(const huffman_tree& other)
{
    copy(other);
}

huffman_tree::huffman_tree(binary_file_reader& bfile)
{
    root_ = read_tree(bfile);
    vector<bool> path;
    build_map(root_.get(), path);
}

huffman_tree& huffman_tree::operator=(huffman_tree rhs)
{
    swap(rhs);
    return *this;
}

void huffman_tree::swap(huffman_tree& other)
{
    std::swap(root_, other.root_);
    std::swap(bits_map_, other.bits_map_);
}

void huffman_tree::copy(const huffman_tree& rhs)
{
    root_ = copy(rhs.root_.get());
}

auto huffman_tree::copy(const node* current) -> std::unique_ptr<node>
{
    if (!current)
        return nullptr;
    auto n = std::make_unique<node>(current->freq);
    n->left = copy(current->left.get());
    n->right = copy(current->right.get());
    return n;
}

void huffman_tree::build_tree(const vector<frequency>& frequencies)
{
    // queue containing leaf nodes
    std::queue<std::unique_ptr<node>> single_queue;
    // queue containing internal nodes
    std::queue<std::unique_ptr<node>> merge_queue;

	// make nodes and place them in single_queue
	for (size_t i=0; i<frequencies.size(); ++i) {
		std::unique_ptr<node> n = std::unique_ptr<node>{new node{frequencies[i]}};
		single_queue.push(std::move(n));
	}
	
	// build tree
	while (single_queue.size() + merge_queue.size() != 1) {

		auto child1 = remove_smallest(single_queue, merge_queue);
		auto child2 = remove_smallest(single_queue, merge_queue);
		frequency parFreq(child1->freq.count() + child2->freq.count());
		auto parent = std::unique_ptr<node>{new node{parFreq}};
		parent->left = std::move(child1);
		parent->right = std::move(child2);
		merge_queue.push(std::move(parent));

	}

	// only one left as root
	if (single_queue.size() == 1) {
		root_ = std::move(single_queue.front());
		single_queue.pop();
	}
	if (merge_queue.size() == 1) {
		root_ = std::move(merge_queue.front());
		merge_queue.pop();
	}

}

void huffman_tree::build_map(const node* current, vector<bool>& path)
{
    // Base case: leaf node.
    if (!current->left && !current->right)
    {
        bits_map_[current->freq.character()] = path;
        return;
    }

    // Move left
    path.push_back(false);
    build_map(current->left.get(), path);
    path.pop_back();

    // Move right
    path.push_back(true);
    build_map(current->right.get(), path);
    path.pop_back();
}

void huffman_tree::print_in_order() const
{
    print_in_order(root_.get());
    cout << endl;
}

void huffman_tree::print_in_order(const node* current) const
{
    if (!current)
        return;
    print_in_order(current->left.get());
    cout << current->freq.character() << ":" << current->freq.count() << " ";
    print_in_order(current->right.get());
}

auto huffman_tree::remove_smallest(
    std::queue<std::unique_ptr<node>>& single_queue,
    std::queue<std::unique_ptr<node>>& merge_queue) -> std::unique_ptr<node>
{
	std::unique_ptr<node> smallest = nullptr;

	if (single_queue.empty() && merge_queue.empty())
		return smallest;
	else if (single_queue.empty()) {
		smallest = std::move(merge_queue.front());
		merge_queue.pop();
	}
	else if (merge_queue.empty()) {
		smallest = std::move(single_queue.front());
		single_queue.pop();
	}
	
	else if (single_queue.front()->freq.count() <= merge_queue.front()->freq.count()) {
		smallest = std::move(single_queue.front());
		single_queue.pop();
	} 
	else if (single_queue.front()->freq.count() > merge_queue.front()->freq.count()) {
		smallest = std::move(merge_queue.front());
		merge_queue.pop();
	}
	return smallest;
}

string huffman_tree::decode_file(binary_file_reader& bfile)
{
    stringstream ss;
    decode(ss, bfile);
    return ss.str();
}

void huffman_tree::decode(stringstream& ss, binary_file_reader& bfile)
{
    auto current = root_.get();
    while (bfile.has_bits())
    {
		auto nextb = bfile.next_bit();
		if (nextb == 0)
			current = current->left.get();
		if (nextb == 1)
			current = current->right.get();
		if (!(current->left) && !(current->right)) {
			ss << current->freq.character();
			current = root_.get();
		}
    }
}

void huffman_tree::write(const string& data, binary_file_writer& bfile)
{
    for (const auto& c : data)
        write(c, bfile);
}

void huffman_tree::write(char c, binary_file_writer& bfile)
{
    vector<bool> bits = bits_for_char(c);
    for (const auto& b : bits)
        bfile.write_bit(b);
}

vector<bool> huffman_tree::bits_for_char(char c)
{
    return bits_map_[c];
}

void huffman_tree::write_tree(binary_file_writer& bfile)
{
    write_tree(root_.get(), bfile);
}

void huffman_tree::write_tree(node* current, binary_file_writer& bfile)
{
	if (!(current->right) && !(current->left)) {
		bfile.write_bit(true);
		bfile.write_byte(current->freq.character());
		return;
	} else {
		bfile.write_bit(false);
		write_tree(current->left.get(), bfile);
		write_tree(current->right.get(), bfile);
	}

}

auto huffman_tree::read_tree(binary_file_reader& bfile) -> std::unique_ptr<node>
{
	std::unique_ptr<node> ret = nullptr;
	bool nbi;
	uint8_t nby;
	char* ch;
	
	if (bfile.has_bits()) {

		nbi = bfile.next_bit();	
		if (nbi == 1) {
			nby = bfile.next_byte();
			ch = (char*)&nby;
			frequency f{*ch, 0};
			auto n = std::unique_ptr<node>{new node{f}};
			return std::move(n);
		} else {
			ret = std::unique_ptr<node>{new node{0}};
			ret->left = read_tree(bfile);
			ret->right = read_tree(bfile);
		}
	} 
	
	return std::move(ret);

}

// class for generic printing

template <typename node>
class huffman_tree_node_descriptor
    : public GenericNodeDescriptor<huffman_tree_node_descriptor<node>>
{
  public:
    huffman_tree_node_descriptor(const node* root) : subroot(root)
    {/* nothing */
    }

    string key() const
    {
        std::stringstream ss;
        char ch = subroot->freq.character();
        int freq = subroot->freq.count();

        // print the sum of the two child frequencies
        if (ch == '\0')
            ss << freq;
        // print the leaf containing a character and its count
        else
        {
            if (ch == '\n')
                ss << "\\n";
            else
                ss << ch;
            ss << ":" << freq;
        }
        return ss.str();
    }

    bool isNull() const
    {
        return !subroot;
    }
    huffman_tree_node_descriptor left() const
    {
        return huffman_tree_node_descriptor(subroot->left.get());
    }
    huffman_tree_node_descriptor right() const
    {
        return huffman_tree_node_descriptor(subroot->right.get());
    }

  private:
    const node* subroot;
};

int huffman_tree::height(const node* subroot) const
{
    if (!subroot)
        return -1;
    return 1 +
           std::max(height(subroot->left.get()), height(subroot->right.get()));
}

void huffman_tree::print(std::ostream& out) const
{
    int h = height(root_.get());
    if (h > max_print_height_)
    {
        out << "Tree is too big to print. Try with a small file (e.g. "
               "data/small.txt)" << endl;
        return;
    }

    printTree(huffman_tree_node_descriptor<node>(root_.get()), out);
}
