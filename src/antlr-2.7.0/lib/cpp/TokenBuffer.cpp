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

#include "antlr/TokenBuffer.hpp"

ANTLR_BEGIN_NAMESPACE(antlr)

/**A Stream of Token objects fed to the parser from a TokenStream that can
 * be rewound via mark()/rewind() methods.
 * <p>
 * A dynamic array is used to buffer up all the input tokens.  Normally,
 * "k" tokens are stored in the buffer.  More tokens may be stored during
 * guess mode (testing syntactic predicate), or when LT(i>k) is referenced.
 * Consumption of tokens is deferred.  In other words, reading the next
 * token is not done by conume(), but deferred until needed by LA or LT.
 * <p>
 *
 * @see antlr.Token
 * @see antlr.TokenStream
 * @see antlr.TokenQueue
 */

/** Create a token buffer */
TokenBuffer::TokenBuffer(TokenStream& input_)
: input(input_)
{ nMarkers=0; markerOffset=0; numToConsume=0; }

/** Mark another token for deferred consumption */
void TokenBuffer::consume()
{ numToConsume++; }

/** Ensure that the token buffer is sufficiently full */
void TokenBuffer::fill(int amount)
{
	syncConsume();
	// Fill the buffer sufficiently to hold needed tokens
	while (queue.entries() < amount + markerOffset) {
		// Append the next token
		queue.append(input.nextToken());
	}
}

/** Get a lookahead token value */
int TokenBuffer::LA(int i)
{
	fill(i);
	return queue.elementAt(markerOffset+i-1)->type;
}

/** Get a lookahead token */
RefToken TokenBuffer::LT(int i)
{
	fill(i);
	return queue.elementAt(markerOffset+i-1);
}

/**Return an integer marker that can be used to rewind the buffer to
 * its current state.
 */
int TokenBuffer::mark()
{
	syncConsume();
	nMarkers++;
	return markerOffset;
}

/**Rewind the token buffer to a marker.
 * @param mark Marker returned previously from mark()
 */
void TokenBuffer::rewind(int mark)
{
	syncConsume();
	markerOffset=mark;
	nMarkers--;
}

/** Sync up deferred consumption */
void TokenBuffer::syncConsume()
{
	while (numToConsume > 0) {
		if (nMarkers > 0) {
			// guess mode -- leave leading tokens and bump offset.
			markerOffset++;
		} else {
			// normal mode -- remove first token
			queue.removeFirst();
		}
		numToConsume--;
	}
}

ANTLR_END_NAMESPACE

