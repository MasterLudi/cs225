/**
 * @file huffman_tree.h
 * Definition of a Huffman Tree class.
 *
 * @author Chase Geigle
 * @date Summer 2012
 */

#ifndef HUFFMAN_TREE_H_
#define HUFFMAN_TREE_H_

#include <cstddef>
#include <vector>
#include <queue>
#include <utility>
#include <sstream>
#include <map>
#include <string>
#include <ostream>

#include "printtree.h"
#include "frequency.h"
#include "binary_file_reader.h"
#include "binary_file_writer.h"

/**
 * huffman_tree: class that represents a Huffman tree for encoding and
 * decoding files with Huffman coding.
 *
 * @author Chase Geigle
 * @date Summer 2012
 */
class huffman_tree
{
  public:
    /**
     * Creates a huffman_tree from a given set of frequency objects.
     *
     * @param frequencies The frequency objects for this tree.
     */
    huffman_tree(std::vector<frequency> frequencies);

    /**
     * Creates a huffman_tree from a binary file that has been written
     * to compress the tree information.
     *
     * @param bfile The binary file to read our compressed tree
     * information from
     */
    huffman_tree(binary_file_reader& bfile);

    /**
     * Copy constructor for Huffman Trees.
     *
     * @param other The huffman_tree to copy.
     */
    huffman_tree(const huffman_tree& other);

    /**
     * Move constructor for Huffman Trees.
     *
     * @param other The huffman_tree to move from.
     */
    huffman_tree(huffman_tree&& other) = default;

    /**
     * Destructor for Huffman Trees.
     */
    ~huffman_tree() = default;

    /**
     * Assignment operator for huffman_tree.
     *
     * @param rhs The right hand side of the assignment statement.
     * @return A reference for performing chained assignments.
     */
    huffman_tree& operator=(huffman_tree rhs);

    /**
     * Swaps the current huffman_tree with the argument.
     *
     * @param other The huffman_tree to swap with
     */
    void swap(huffman_tree& other);

    /**
     * Decodes a given file into its string contents.
     *
     * @param file binary_file_reader to read bits from.
     * @return The decoded contents of the file.
     */
    std::string decode_file(binary_file_reader& bfile);

    /**
     * Writes a string of data to the binary file using Huffman coding.
     *
     * @param data The string to be written.
     * @param bfile The binary file to write the string to.
     */
    void write(const std::string& data, binary_file_writer& bfile);

    /**
     * Writes a signle character to the binary file using Huffman
     * coding.
     *
     * @param c The character to be written.
     * @param bfile The binary file to write the character to.
     */
    void write(char c, binary_file_writer& bfile);

    /**
     * Writes a compressed version of the tree to the file.
     *
     * @param bfile The binary file to be written to.
     */
    void write_tree(binary_file_writer& bfile);

    /**
     * Prints each element in the tree in an in-order traversal.
     */
    void print_in_order() const;

    /**
     * Prints the tree to a stream.
     * @param out The stream to print to
     */
    void print(std::ostream& out) const;

  private:
    /**
     * node class: internal representation of the Huffman tree.
     * Done using linked memory.
     *
     * @author Chase Geigle
     * @date Summer 2012
     */
    class node
    {
      public:
        /// Data for this node: contains a character and a count.
        frequency freq;
        /// Left child of this node
        std::unique_ptr<node> left;
        /// Right child of this node
        std::unique_ptr<node> right;

        /**
         * Builds a new node with the given frequency as data.
         *
         * @param theFreq The frequency to build this node
         * with.
         */
        node(frequency theFreq) : freq{theFreq}
        {
            // nothing
        }

        /**
         * Builds a new node with the given integer as its
         * frequency counter, used for building internal nodes.
         *
         * @param frequency The frequency for this internal node.
         */
        node(int frequency) : freq{frequency}
        {
            // nothing
        }
    };

    /**
     * Private helper function that copies another huffman_tree.
     *
     * @param other The huffman_tree to be copied.
     */
    void copy(const huffman_tree& other);

    /**
     * Recursive, private helper function that copies a given subtree
     * of another huffman_tree.
     *
     * @param current The root of the subtree in the other huffman_tree
     * to be copied.
     * @return A pointer to the root of the new, copied subtree.
     */
    std::unique_ptr<node> copy(const node* current);

    /**
     * Recursive, private helper function that frees all memory
     * associated with a subtree of the huffman_tree.
     *
     * @param current The root of the subtree to free data from.
     */
    void clear(node* current);

    /**
     * Helper function used by the constructor to build a huffman_tree
     * for a collection of frequency data. Each Frequency object
     * represents a character and how often it appears in the data to
     * be encoded.
     *
     * @param frequencies The set of frequency objects to build the
     * tree with.
     */
    void build_tree(const std::vector<frequency>& frequencies);

    /**
     * Helper function used by the constructor to build a huffman_tree
     * from a compressed version of the tree written in a binary file.
     *
     * @param bfile The binary file we are reading.
     * @return A pointer to the root of the subtree built.
     */
    std::unique_ptr<node> read_tree(binary_file_reader& bfile);

    /**
     * Recursive helper function used by the constructor to build a map
     * of characters to their encoded values based on the tree
     * structure built.
     *
     * @param current The current node we are visiting.
     * @param path The current path we have taken to get to this node.
     * Used to store the encoded value for the characters of the tree.
     */
    void build_map(const node* current, std::vector<bool>& path);

    /**
     * Private helper for printing a tree in order.
     * @param current The current subroot
     */
    void print_in_order(const node* current) const;

    /**
     * Helper function: finds the smallest element on the two queues
     * and removes it. In the event that there is a tie, it should remove
     * the front of the **single_queue**.
     *
     * @param single_queue The first queue to inspect.
     * @param merge_queue The second queue to inspect.
     * @return A pointer to the smallest node that used to be at
     * the front of one of the queues.
     */
    std::unique_ptr<node>
        remove_smallest(std::queue<std::unique_ptr<node>>& single_queue,
                        std::queue<std::unique_ptr<node>>& merge_queue);

    /**
     * Determines the encoded value for a given character.
     *
     * @param c The character to find the encoded value for.
     * @return The encoded value for that character.
     */
    std::vector<bool> bits_for_char(char c);

    /**
     * Helper function that recursively decodes a file by traversing
     * the tree based on the bits read.
     *
     * @param ss The stringstream being used to build the decoded
     * output string.
     * @param bfile The binary file we are decoding.
     */
    void decode(std::stringstream& ss, binary_file_reader& bfile);

    /**
     * Helper function to write the tree out to a binary file in a
     * compressed way.
     *
     * @param current The root of the subtree we are currently writing.
     * @param bfile The fiel we are writing to.
     */
    void write_tree(node* current, binary_file_writer& bfile);

    /**
     * Private helper to get the height of the huffman_tree.
     * @param subroot Where we're currently at.
     * @return the height of the tree
     */
    int height(const node* subroot) const;

    /**
     * Maximum height of trees to enable printing for
     * (chosen by fair dice roll)
     */
    const static int max_print_height_ = 9;

    /// Root of the tree
    std::unique_ptr<node> root_;
    /// Standard map that maps characters to their encoded values
    std::map<char, std::vector<bool>> bits_map_;
};
#endif
