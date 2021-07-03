
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;

// http://eli.thegreenplace.net/2011/05/17/the-curiously-recurring-template-pattern-in-c

template<class child>
class MyBase {
public:
	void myinterface() {
		static_cast<child*>(this)->implementation();
	}
};

class MyDerived : public MyBase<MyDerived> {
public:
	void implementation() { // note this function is not virtual!
		cout << " MyDerived\n";
	}
};

template<typename T = void>
class MyDerived2 : public MyBase<MyDerived2<T>> {
public:
	void implementation() { // note this function is not virtual!
		cout << " T = " << typeid(T).name() << endl;
		cout << " MyDerived2\n";
	}
};


template<typename T = void>
class MyDerived3 : public MyDerived2<T> {
public:
	void implementation() { // note this function is not virtual and it is NOT called by MyBase.interface!
		cout << " T = " << typeid(T).name() << endl;
		cout << " MyDerived3\n";
	}
};


struct TreeNode
{
	enum class Kind { RED, BLUE };

	TreeNode(Kind kind_, TreeNode* left_ = NULL, TreeNode* right_ = NULL)
		: kind(kind_), left(left_), right(right_)
	{}

	Kind kind;
	TreeNode *left, *right;
};


template <typename Derived>
class GenericVisitor
{
public:
	void visit_preorder(TreeNode* node)
	{
		if (node) {
			dispatch_node(node);
			visit_preorder(node->left);
			visit_preorder(node->right);
		}
	}

	void visit_inorder(TreeNode* node)
	{
		if (node) {
			visit_inorder(node->left);
			dispatch_node(node);
			visit_inorder(node->right);
		}
	}

	void visit_postorder(TreeNode* node)
	{
		if (node) {
			visit_postorder(node->left);
			visit_postorder(node->right);
			dispatch_node(node);
		}
	}

	void handle_RED(TreeNode* node)
	{
		cout << "Generic handle RED\n";
	}

	void handle_BLUE(TreeNode* node)
	{
		cout << "Generic handle BLUE\n";
	}

private:
	// Convenience method for CRTP
	//
	Derived& derived()
	{
		return *static_cast<Derived*>(this);
	}

	void dispatch_node(TreeNode* node)
	{
		switch (node->kind) {
		case TreeNode::RED:
			derived().handle_RED(node);
			break;
		case TreeNode::BLUE:
			derived().handle_BLUE(node);
			break;
		default:
			assert(0);
		}
	}
};

class SpecialVisitor1 : public GenericVisitor<SpecialVisitor1>
{
public:
	void handle_RED(TreeNode* node)
	{
		cout << "RED is special1\n";
	}
};

class SpecialVisitor2 : public GenericVisitor<SpecialVisitor2>
{
public:
	void handle_BLUE(TreeNode* node)
	{
		cout << "BLUE is special2\n";
	}
};
int testCRTP()
{
	MyDerived2<> d;
	//d.myinterface();

	MyDerived2<int> d2;
	//d2.myinterface();
	MyDerived3<float> d3;
	d3.myinterface();
	d3.implementation();

	MyBase<MyDerived>  b;
	b.myinterface();

	MyBase<MyDerived2<double>>  b2;
	b2.myinterface();


	vector<MyBase<int> *> mb;
	//mb.push_back(new MyDerived());


	vector<TreeNode> tn{ TreeNode(TreeNode::Kind::BLUE), TreeNode(TreeNode::Kind::RED) , TreeNode(TreeNode::Kind::BLUE) };
	/* GenericVisitor<void> *gv = {
		new SpecialVisitor1(),
	    new SpecialVisitor2()
	};
	*/
	

	return 0;
}


//https://www.fluentcpp.com/2018/05/22/how-to-transform-a-hierarchy-of-virtual-methods-into-a-crtp/
// CRTP with multileverl inheritances

/*

JQL: This might not be working as advertized, 1/20/2019
*/
template<typename Derived>
struct CRTP_Top
{
	void bigAndSlow() const
	{
		static_cast<Derived const&>(*this).helperfunction1();
		static_cast<Derived const&>(*this).helperfunction2();
	}
};

// This intermediate level can not be instantiated independently
template<typename Derived>
struct CRTP_Mid : public CRTP_Top<Derived>
{
	void helperfunction1() const { cout << "CRTP_Mid::helperfunction1()\n"; }
};

struct CRTP_Lowest : public CRTP_Mid<CRTP_Lowest>
{
	void helperfunction2() const { cout << "CRTP_Lowest::helperfunction2()\n"; }
};

struct CRTP_Lowest2 : public CRTP_Top<CRTP_Lowest2>
{
	void helperfunction1() const { cout << "CRTP_Lowest2::helperfunction1()\n"; }
	void helperfunction2() const { cout << "CRTP_Lowest2::helperfunction2()\n"; }
};
/*
7/3/21
HP 
*/

int testCRTP2()
{
	CRTP_Lowest cl;
	cl.bigAndSlow();
	CRTP_Lowest2 c2;
	c2.bigAndSlow();
	CRTP_Mid<CRTP_Mid<void>> mid;
	//mid.bigAndSlow(); // not compile

	return 0;
}

/* 7/3/21 No updates

*/
