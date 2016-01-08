/**
 * @file parser.cpp
 * @date Summer 2014
 */

#include <cassert>
#include "parser.h"
#include "operation.h"
#include "number.h"
#include "terminal.h"
#include "binary_op.h"
#include "unary_op.h"

namespace cs225
{

/**
 * A "private class" for helping us parse expressions. Maintains all of the
 * state necessary to perform Dijkstra's shunting-yard algorithm.
 *
 * @see https://en.wikipedia.org/wiki/Shunting-yard_algorithm
 */
class parser
{
  public:
    /**
     * @param tokens The tokens to parse
     * @return the root of the parse tree for that expression
     */
    std::unique_ptr<node> parse(queue<std::unique_ptr<token>>& tokens);

  private:
    /**
     * The stack to hold the operator tokens encountered. This is the "side
     * rail" in the shunting yard algorithm.
     */
    stack<std::unique_ptr<token>> ops_;

    /**
     * What is this for? It is a mystery (and a hint)!
     */
    stack<std::unique_ptr<node>> nodes_;

	stack<std::unique_ptr<token>> tmp;

	void handle_token(std::unique_ptr<token> tok);
	//void build_output_stack();
	
	queue<std::unique_ptr<token>> out;

	void print();

	std::unique_ptr<node>  make_tree();
	std::unique_ptr<node> make_node(std::unique_ptr<token> tok);
	void fill_tree_from_right(node* subRoot);
	std::unique_ptr<node> root;
};

std::unique_ptr<node> parse(queue<std::unique_ptr<token>>& tokens)
{
    parser p;
    return p.parse(tokens);
}

std::unique_ptr<node> parser::parse(queue<std::unique_ptr<token>>& tokens)
{
    while (!tokens.empty())
    {
        auto& tok = tokens.front();
        tokens.pop();
		
		handle_token(std::move(tok));

    }
	
	cout << "parser::parse()::cleaning up remaining operators..." << endl;
	while (!ops_.empty())
	{
		auto& ops = ops_.top();
		ops_.pop();
		out.push(std::move(ops));
	}
	
	cout << "parse()::stacking to nodes_ reverse-reverse-polish..." << endl;
	// stack to nodes stack reversely
	while (!out.empty())
	{
		auto& tmp = out.front();
		out.pop();
		nodes_.push(make_node(std::move(tmp)));

	}

	cout << "made nodes" << endl;

	root = std::move(make_tree());
	
	cout << "returning root" << endl;

	return std::move(root);
}

void parser::print()
{
	while (!out.empty())
	{
		const auto& tok = out.front();
		switch (tok->type())
		{
			case token_type::TEXT:
				std::cout << "text(" << tok->text() << ") ";
				break;
			case token_type::NUMBER:
				std::cout << "num(" << tok->as_number()->value()
						  << ") ";
				break;
			case token_type::OPERATION:
				std::cout << "op(" << tok->text() << ") ";
				break;
		}
		out.pop();
	}
 
}
void parser::handle_token(std::unique_ptr<token> tok)
{
	int preced;
	int preced_prev;

	if (tok->type() == token_type::NUMBER) {
		out.push(std::move(tok));
			
	} else if (tok->type() == token_type::OPERATION) {

		// check if previous ops on the op stack has lower preced 
		preced = (tok->as_operation())->precedence();
		
		while (true) {
			// if ops_ is empty, just put token on the ops_ stack
			if (ops_.empty()) {
				ops_.push(std::move(tok));
				break;
			}
			auto& prev = ops_.top();
			if (prev->type() == token_type::TEXT) {
				ops_.push(std::move(tok));
				break;
			}
			else if (prev->type() == token_type::OPERATION) {
				preced_prev = (prev->as_operation())->precedence();
				// pop if current preced is smaller than prev_preced
				//     or curr preced == prev preced AND curr preced is left associative
				if ((preced_prev > preced) || ((tok->as_operation())->associativity() 
							== associativity::LEFT && (preced_prev == preced))) {
					ops_.pop();
					out.push(std::move(prev));

				} else {
					ops_.push(std::move(tok));
					break;
				}
			}
		}

	} else if (tok->type() == token_type::TEXT) {
		if (tok->text() == "(") {
			ops_.push(std::move(tok));
		// if ")" pop ops_ stack and push it to output queue until you find "("
		} else if (tok->text() == ")") {
			while (true) {
				// pop the stack
				if (ops_.empty())
					throw std::runtime_error{"unbalanced parens"};

				auto& prev = ops_.top();
				
				if (prev->text() == "(") {
					ops_.pop();
					break;
				} else {
					ops_.pop();
					out.push(std::move(prev));
				}
			}
		}
	}
}

std::unique_ptr<node> parser::make_tree(void)
{
	if (!nodes_.empty()) {
		unique_ptr<node> root = std::move(nodes_.top());
		nodes_.pop();

		fill_tree_from_right(root.get());

		if (!nodes_.empty()) throw std::runtime_error{"extra number(s)"};
		return root;
	}

	return nullptr;

}

void parser::fill_tree_from_right(node* subRoot)
{

	if (subRoot->type() == node_type::TERMINAL) {
		cout << "subRoot.type() == node_type::TERMINAL" <<endl;;
		return;

	} else if (subRoot->type() == node_type::UNARY_OP) {
		cout << "subRoot.type() == node_type::UNARY_OP" <<endl;;
		if (nodes_.empty()) throw std::runtime_error{"extra operators"};
		std::unique_ptr<node>& next = nodes_.top();
		nodes_.pop();
		subRoot->set_child(std::move(next));
		fill_tree_from_right(subRoot->get_child());

	} else if (subRoot->type() == node_type::BINARY_OP) {
		cout << "subRoot.type() == node_type::BINARY_OP" <<endl;;
		if (!nodes_.empty()) {
			std::unique_ptr<node>& right = nodes_.top();
			nodes_.pop();
			subRoot->set_child_right(std::move(right));
			fill_tree_from_right(subRoot->get_child_right());
		}
		if (!nodes_.empty()) {
			std::unique_ptr<node>& left = nodes_.top();
			nodes_.pop();
			subRoot->set_child_left(std::move(left));
			fill_tree_from_right(subRoot->get_child_left());
			//cout << "----not above lines" << endl;
		}
		if ((nodes_.empty()) && (subRoot->get_child_left() == nullptr || subRoot->get_child_right() == nullptr)) {
			throw std::runtime_error{"extra operators"};
		}
	}

	return;

}

std::unique_ptr<node> parser::make_node(std::unique_ptr<token> tok)
{
	
	std::unique_ptr<node> n1;
	std::unique_ptr<node> n2;
	token_type typ = tok->type();
	std::string tex = tok->text();

	if (typ == token_type::OPERATION) {
		if (tok->as_operation()->args() == 1) {
			n1 = make_unary_op(tex, std::move(n1));
		} else if (tok->as_operation()->args() == 2) {
			n1 = make_binary_op(tex, std::move(n1), std::move(n2));
		}
	} else if (typ == token_type::NUMBER) {
		n1 = make_term(tok->as_number()->value());

	} else if (typ == token_type::TEXT)
		throw std::runtime_error{"unbalanced parens"};

	return n1;

}

}
