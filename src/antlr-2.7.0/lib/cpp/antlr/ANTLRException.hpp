#ifndef INC_ANTLRException_hpp__
#define INC_ANTLRException_hpp__

/**
 * <b>SOFTWARE RIGHTS</b>
 * <p>
 * ANTLR 2.6.0 MageLang Insitute, 1999
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
 * @version ANTLR 2.6.0 MageLang Insitute, 1999
 * @author Terence Parr, <a href=http://www.MageLang.com>MageLang Institute</a>
 * @author <br>John Lilley, <a href=http://www.Empathy.com>Empathy Software</a>
 * @author <br><a href="mailto:pete@yamuna.demon.co.uk">Pete Wells</a>
 */

#include "antlr/config.hpp"
#include <exception>
#include <string>

ANTLR_BEGIN_NAMESPACE(antlr)

class ANTLRException : public ANTLR_USE_NAMESPACE(std)exception {
private:
	ANTLR_USE_NAMESPACE(std)string text;

public:
	ANTLRException();
	ANTLRException(const ANTLR_USE_NAMESPACE(std)string& s);
	virtual ~ANTLRException() throw();

	virtual ANTLR_USE_NAMESPACE(std)string toString() const;

	ANTLR_USE_NAMESPACE(std)string getMessage() const;

	virtual const char* what() const throw();
};

ANTLR_END_NAMESPACE

#endif //INC_ANTLRException_hpp__
