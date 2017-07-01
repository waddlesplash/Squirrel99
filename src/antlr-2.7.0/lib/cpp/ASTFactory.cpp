/**
 * <b>SOFTWARE RIGHTS</b>
 * <p>
 * ANTLR 2.6.0 MageLang Insitute, 1998
 * <p>
 * We reserve no legal rights to the ANTLR--it is fully in the
 * public domain. An individual or company may do whatever
 * they wish with source code distributed with ANTLR or the
 * code generated by ANTLR, including the incorporation of
 * ANTLR, or its output, into commerical software.
 * <p>
 * We encourage users to develop software with ANTLR. However,
 * we do ask that credit is given to us for developing
 * ANTLR. By "credit", we mean that if you use ANTLR or
 * incorporate any source code into one of your programs
 * (commercial product, research project, or otherwise) that
 * you acknowledge this fact somewhere in the documentation,
 * research report, etc... If you like ANTLR and have
 * developed a nice tool with the output, please mention that
 * you developed it using ANTLR. In addition, we ask that the
 * headers remain intact in our source code. As long as these
 * guidelines are kept, we expect to continue enhancing this
 * system and expect to make other tools available as they are
 * completed.
 * <p>
 * The ANTLR gang:
 * @version ANTLR 2.6.0 MageLang Insitute, 1998
 * @author Terence Parr, <a href=http://www.MageLang.com>MageLang Institute</a>
 * @author <br>John Lilley, <a href=http://www.Empathy.com>Empathy Software</a>
 * @author <br><a href="mailto:pete@yamuna.demon.co.uk">Pete Wells</a>
 */

#include "antlr/ASTFactory.hpp"

#include "antlr/CommonAST.hpp"

ANTLR_BEGIN_NAMESPACE(antlr)

/** AST Support code shared by TreeParser and Parser.
 *  We use delegation to share code (and have only one
 *  bit of code to maintain) rather than subclassing
 *  or superclassing (forces AST support code to be
 *  loaded even when you don't want to do AST stuff).
 *
 *  Typically, setASTNodeType is used to specify the
 *  type of node to create, but you can override
 *  create to make heterogeneous nodes etc...
 */

ASTFactory::ASTFactory()
{
	nodeFactory = &CommonAST::factory;
}
/** Add a child to the current AST */
void ASTFactory::addASTChild(ASTPair& currentAST, RefAST child)
{
	if (child) {
		if (!currentAST.root) {
			// Make new child the current root
			currentAST.root = child;
		}
		else {
			if (!currentAST.child) {
				// Add new child to current root
				currentAST.root->setFirstChild(child);
			}
			else {
				currentAST.child->setNextSibling(child);
			}
		}
		// Make new child the current child
		currentAST.child = child;
		currentAST.advanceChildToEnd();
	}
}
/** Create a new empty AST node; if the user did not specify
 *  an AST node type, then create a default one: CommonAST.
 */
RefAST ASTFactory::create()
{
	RefAST node = nodeFactory(); //new CommonASTNode();
	node->setType(Token::INVALID_TYPE);
	return node;
}
RefAST ASTFactory::create(int type)
{
	RefAST t = create();
	t->initialize(type,"");
	return t;
}
RefAST ASTFactory::create(int type, const ANTLR_USE_NAMESPACE(std)string& txt)
{
	RefAST t = create();
	t->initialize(type,txt);
	return t;
}
/** Create a new empty AST node; if the user did not specify
 *  an AST node type, then create a default one: CommonAST.
 */
RefAST ASTFactory::create(RefAST tr)
{
	if (!tr) return nullAST;		// create(null) == null
	RefAST t = create();
	t->initialize(tr);
	return t;
}
RefAST ASTFactory::create(RefToken tok)
{
	RefAST t = create();
	t->initialize(tok);
	return t;
}
/** Copy a single node.  clone() is not used because
 *  we want to return an AST not a plain object...a type
 *  safety issue.  Further, we want to have all AST node
 *  creation go through the factory so creation can be
 *  tracked.  Returns null if t is null.
 */
RefAST ASTFactory::dup(RefAST t)
{
	return create(t);		// if t==null, create returns null
}
/** Duplicate tree including siblings of root. */
RefAST ASTFactory::dupList(RefAST t)
{
	RefAST result = dupTree(t);         // if t == null, then result==null
	RefAST nt = result;
	while (t) {									// for each sibling of the root
		t = t->getNextSibling();
		nt->setNextSibling(dupTree(t));	// dup each subtree, building new tree
		nt = nt->getNextSibling();
	}
	return result;
}
/**Duplicate a tree, assuming this is a root node of a tree--
 * duplicate that node and what's below; ignore siblings of root node.
 */
RefAST ASTFactory::dupTree(RefAST t)
{
	RefAST result = dup(t);		// make copy of root
	// copy all children of root.
	if (t) {
		result->setFirstChild( dupList(t->getFirstChild()) );
	}
	return result;
}
/** Make a tree from a list of nodes.  The first element in the
 *  array is the root.  If the root is null, then the tree is
 *  a simple list not a tree.  Handles null children nodes correctly.
 *  For example, build(a, b, null, c) yields tree (a b c).  build(null,a,b)
 *  yields tree (nil a b).
 */
RefAST ASTFactory::make(ANTLR_USE_NAMESPACE(std)vector<RefAST> nodes)
{
	if ( nodes.size()==0 )
		return RefAST(nullASTptr);
	RefAST root = nodes[0];
	RefAST tail = RefAST(nullASTptr);
	if (root) {
		root->setFirstChild(RefAST(nullASTptr));	// don't leave any old pointers set
	}
	// link in children;
	for (unsigned int i=1; i<nodes.size(); i++) {
		if ( !nodes[i] ) continue;	// ignore null nodes
		if ( !root ) {
			// Set the root and set it up for a flat list
			root = tail = nodes[i];
		}
		else if ( !tail ) {
			root->setFirstChild(nodes[i]);
			tail = root->getFirstChild();
		}
		else {
			tail->setNextSibling(nodes[i]);
			tail = tail->getNextSibling();
		}
		// Chase tail to last sibling
		while (tail->getNextSibling()) {
			tail = tail->getNextSibling();
		}
	}
	return root;
}
/** Make a tree from a list of nodes, where the nodes are contained
  * in an ASTArray object
  */
RefAST ASTFactory::make(ASTArray* nodes)
{
	RefAST ret = make(nodes->array);
	delete nodes;
	return ret;
}
/** Make an AST the root of current AST */
void ASTFactory::makeASTRoot(ASTPair& currentAST, RefAST root)
{
	if (root) {
		// Add the current root as a child of new root
		root->addChild(currentAST.root);
		// The new current child is the last sibling of the old root
		currentAST.child = currentAST.root;
		currentAST.advanceChildToEnd();
		// Set the new root
		currentAST.root = root;
	}
}
void ASTFactory::setASTNodeFactory(factory_type factory)
{
	nodeFactory = factory;
}

ANTLR_END_NAMESPACE

