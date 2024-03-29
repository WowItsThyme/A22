/*
************************************************************
* COMPILERS COURSE - Algonquin College
* Code version: Summer, 2022
* Author: Svillen Ranev - Paulo Sousa
* Professors: Paulo Sousa
************************************************************
*/

/*
************************************************************
* File name: scanner.h
* Compiler: MS Visual Studio 2022
* Author: Paulo Sousa
* Course: CST 8152 � Compilers, Lab Section: [011, 012, 013]
* Assignment: A22, A32.
* Date: Jul 01 2022
* Professor: Paulo Sousa
* Purpose: This file contains all functionalities from Scanner.
* Function list: (...).
************************************************************
*/

/* TO_DO: Adjust the function header */

 /* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
  * to suppress the warnings about using "unsafe" functions like fopen()
  * and standard sting library functions defined in string.h.
  * The define does not have any effect in Borland compiler projects.
  */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

  /* #define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

#ifndef BUFFER_H_
#include "Reader.h"
#endif

#ifndef SCANNER_H_
#include "Scanner.h"
#endif

/*
----------------------------------------------------------------
TO_DO: Global vars definitions
----------------------------------------------------------------
*/

/* Global objects - variables */
/* This buffer is used as a repository for string literals. */
extern ReaderPointer stringLiteralTable;	/* String literal table */
jer_intg line;								/* Current line number of the source code */
extern jer_intg errorNumber;				/* Defined in platy_st.c - run-time error number */

extern jer_intg stateType[];
extern jer_char* keywordTable[];
extern PTR_ACCFUN finalStateTable[];
extern jer_intg transitionTable[][TABLE_COLUMNS];

/* Local(file) global objects - variables */
static ReaderPointer lexemeBuffer;			/* Pointer to temporary lexeme buffer */
static ReaderPointer sourceBuffer;			/* Pointer to input source buffer */

/*
 ************************************************************
 * Intitializes scanner
 *		This function initializes the scanner using defensive programming.
 ***********************************************************
 */
 /* TO_DO: Follow the standard and adjust datatypes */

jer_intg startScanner(ReaderPointer psc_buf) {
	/* in case the buffer has been read previously  */
	readerRecover(psc_buf);
	readerClear(stringLiteralTable);
	line = 1;
	sourceBuffer = psc_buf;
	return EXIT_SUCCESS; /*0*/
}

/*
 ************************************************************
 * Process Token
 *		Main function of buffer, responsible to classify a char (or sequence
 *		of chars). In the first part, a specific sequence is detected (reading
 *		from buffer). In the second part, a pattern (defined by Regular Expression)
 *		is recognized and the appropriate function is called (related to final states
 *		in the Transition Diagram).
 ***********************************************************
 */

Token tokenizer(void) {

	/* TO_DO: Follow the standard and adjust datatypes */

	Token currentToken = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	jer_char c;	/* input symbol */
	jer_intg state = 0;		/* initial state of the FSM */
	jer_intg lexStart;		/* start offset of a lexeme in the input char buffer (array) */
	jer_intg lexEnd;		/* end offset of a lexeme in the input char buffer (array)*/

	jer_intg lexLength;		/* token length */
	jer_intg i;				/* counter */
	jer_char newc;			/* new char */

	while (1) { /* endless loop broken by token returns it will generate a warning */
		c = readerGetChar(sourceBuffer);
		int check = 0;
		/* ------------------------------------------------------------------------
			Part 1: Implementation of token driven scanner.
			Every token is possessed by its own dedicated code
			-----------------------------------------------------------------------
		*/

		/* TO_DO: All patterns that do not require accepting functions */
		switch (c) {

			/* Cases for spaces */
		case ' ':
		case '\t':
		case '\f':
			break;
		case '\n':
			line++;
			break;
			/* Cases for symbols */
		case '+':
			currentToken.code = ART_T;
			currentToken.attribute.arithmeticOperator = OP_ADD;
			return currentToken;
		case '-':
			currentToken.code = ART_T;
			currentToken.attribute.arithmeticOperator = OP_SUB;
			return currentToken;
		case '*':
			currentToken.code = ART_T;
			currentToken.attribute.arithmeticOperator = OP_MUL;
			return currentToken;
		case '/':
			currentToken.code = ART_T;
			currentToken.attribute.arithmeticOperator = OP_DIV;
			return currentToken;
		case '<':
			newc = readerGetChar(sourceBuffer);
			currentToken.code = RLO_T;
			if (newc == '=') {
				currentToken.attribute.relationalOperator = OP_EGT;
			}
			else {
				currentToken.attribute.relationalOperator = OP_LT;
			}
			return currentToken;
		case '>':
			newc = readerGetChar(sourceBuffer);
			currentToken.code = RLO_T;
			if (newc == '=') {
				currentToken.attribute.relationalOperator = OP_ELT;
			}
			else {
				currentToken.attribute.relationalOperator = OP_GT;
			}
			return currentToken;
		case '!':
			newc = readerGetChar(sourceBuffer);
			currentToken.code = RLO_T;
			if (newc == '=') {
				currentToken.attribute.relationalOperator = OP_NE;
			}
			else {
				currentToken.code = LGO_T;
				currentToken.attribute.logicalOperator = OP_NOT;
			}
			return currentToken;
		case '=':
			newc = readerGetChar(sourceBuffer);
			if (newc == '=') {
				currentToken.code = RLO_T;
				currentToken.attribute.relationalOperator = OP_EQ;
			}
			else {
				currentToken.code = ASS_T;
				readerRetract(sourceBuffer);
			}
			return currentToken;
		case ')':
			currentToken.code = RPR_T;
			return currentToken;
		case '(':
			currentToken.code = LPR_T;
			return currentToken;
		case ':':
			currentToken.code = SMC_T;
			return currentToken;
		case ',':
			currentToken.code = COM_T;
			return currentToken;
		case '&':
			newc = readerGetChar(sourceBuffer);
			if (newc == '&') {
				currentToken.code = LGO_T;
				currentToken.attribute.logicalOperator = OP_AND;
			}
			readerRetract(sourceBuffer);
			return currentToken;
		case '|':
			newc = readerGetChar(sourceBuffer);
			if (newc == '|') {
				currentToken.code = LGO_T;
				currentToken.attribute.logicalOperator = OP_NOT;
			}
			readerRetract(sourceBuffer);
			return currentToken;
			/* Comments */
		case '#':
			newc = readerGetChar(sourceBuffer);
			if (newc == '#')
				check = 1;
			do {
				c = readerGetChar(sourceBuffer);
				if (c == CHARSEOF0 || c == CHARSEOF255) {
					readerRetract(sourceBuffer);
					//return currentToken;
				}
				if (c == '#')
					check++;
				else if (c == '\n' && check == 0)
					break;
			} while (c != CHARSEOF0 && c != CHARSEOF255 && check != 3);
			break;
			/* Cases for END OF FILE */
		case CHARSEOF0:
			currentToken.code = SEOF_T;
			currentToken.attribute.seofType = SEOF_0;
			return currentToken;
		case CHARSEOF255:
			currentToken.code = SEOF_T;
			currentToken.attribute.seofType = SEOF_255;
			return currentToken;

			/* ------------------------------------------------------------------------
				Part 2: Implementation of Finite State Machine (DFA) or Transition Table driven Scanner
				Note: Part 2 must follow Part 1 to catch the illegal symbols
				-----------------------------------------------------------------------
			*/

			/* TO_DO: Adjust / check the logic for your language */

		default: // general case
			state = nextState(state, c);
			lexStart = readerGetPosRead(sourceBuffer) - 1;
			readerSetMark(sourceBuffer, lexStart);
			int pos = 0;
			while (stateType[state] == NOFS) {
				c = readerGetChar(sourceBuffer);
				state = nextState(state, c);
				pos++;
			}
			if (stateType[state] == FSWR)
				readerRetract(sourceBuffer);
			lexEnd = readerGetPosRead(sourceBuffer);
			lexLength = lexEnd - lexStart;
			lexemeBuffer = readerCreate((jer_intg)lexLength + 2, 0, MODE_FIXED);
			if (!lexemeBuffer) {
				fprintf(stderr, "Scanner error: Can not create buffer\n");
				exit(1);
			}
			int y = sourceBuffer->position.read;
			int t = 0;
			readerRestore(sourceBuffer);
			for (i = 0; i < sourceBuffer->position.wrte; i++) {
				char n = readerGetChar(sourceBuffer);
				if (i >= lexStart && i < lexEnd) {
					t++;
					if (state == 2 || state == 3 || state == 5 || state == 7 || state == 10) {
						readerAddChar(lexemeBuffer, n);
					}
				}
			}
			sourceBuffer->position.read = y;
			readerAddChar(lexemeBuffer, READER_TERMINATOR);
			currentToken = (*finalStateTable[state])(readerGetContent(lexemeBuffer, 0));
			readerRestore(lexemeBuffer); //xxx
			return currentToken;
		} // switch

	} //while

} // tokenizer


/*
 ************************************************************
 * Get Next State
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	(*) assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:
	(*) Assertion failed: test, file filename, line linenum.
	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem.
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUG is used - see the top of the file.
 ***********************************************************
 */
 /* TO_DO: Just change the datatypes */

jer_intg nextState(jer_intg state, jer_char c) {
	jer_intg col;
	jer_intg next;
	col = nextClass(c);
	next = transitionTable[state][col];
	if (DEBUG)
		printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
	assert(next != FS);
	if (DEBUG)
		if (next == FS) {
			printf("Scanner Error: Illegal state:\n");
			printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
			exit(1);
		}
	return next;
}

/*
 ************************************************************
 * Get Next Token Class
	* Create a function to return the column number in the transition table:
	* Considering an input char c, you can identify the "class".
	* For instance, a letter should return the column for letters, etc.
 ***********************************************************
 */
 /* DONE: Use your column configuration */

 /* Adjust the logic to return next column in TT */
 /*	[A-z](0), [0-9](1),	_(2), ((3), "(4), .(5), SEOF(6), other(7) */

jer_intg nextClass(jer_char c) {
	jer_intg val = -1;
	switch (c) {
	case CHRCOL2:
		val = 2;
		break;
	case CHRCOL3:
		val = 3;
		break;
	case CHRCOL4:
		val = 4;
		break;
	case CHRCOL5:
		val = 5;
		break;
	case CHARSEOF0:
	case CHARSEOF255:
		val = 6;
		break;
	default:
		if (isalpha(c))
			val = 0;
		else if (isdigit(c))
			val = 1;
		else
			val = 6;
	}
	return val;
}


/*
 ************************************************************
 * Acceptance State Function IL
 *		Function responsible to identify IL (integer literals).
 * - It is necessary respect the limit (ex: 2-byte integer in C).
 * - In the case of larger lexemes, error shoul be returned.
 * - Only first ERR_LEN characters are accepted and eventually,
 *   additional three dots (...) should be put in the output.
 ***********************************************************
 */
 /* TO_DO: Adjust the function for IL */

Token funcIL(jer_char lexeme[]) {
	Token currentToken = { 0 };
	jer_long tlong;
	if (lexeme[0] != '\0' && strlen(lexeme) > NUM_LEN) {
		currentToken = (*finalStateTable[ESNR])(lexeme);
	}
	else {
		tlong = atol(lexeme);
		if (tlong >= 0 && tlong <= SHRT_MAX) {
			currentToken.code = INL_T;
			currentToken.attribute.intValue = (jer_intg)tlong;
		}
		else {
			currentToken = (*finalStateTable[ESNR])(lexeme);
		}
	}
	return currentToken;
}

Token funcFL(jer_char lexeme[]) {
	Token currentToken = { 0 };
	jer_doub tlong;

	if ( strlen(lexeme) > NUM_LEN) {
		currentToken = (*finalStateTable[ESNR])(lexeme);
	}
	else {
		tlong = atof(lexeme);
		if (tlong >= 0 && tlong <= SHRT_MAX) {
			currentToken.code = FL_T;
			currentToken.attribute.floatValue = (jer_doub)tlong;
		}
		else {
			currentToken = (*finalStateTable[ESNR])(lexeme);
		}
	}
	return currentToken;
}


/*
 ************************************************************
 * Acceptance State Function ID
 *		In this function, the pattern for IDs must be recognized.
 *		Since keywords obey the same pattern, is required to test if
 *		the current lexeme matches with KW from language.
 *	- Remember to respect the limit defined for lexemes (VID_LEN) and
 *	  set the lexeme to the corresponding attribute (vidLexeme).
 *    Remember to end each token with the \0.
 *  - Suggestion: Use "strncpy" function.
 ***********************************************************
 */
 /* TO_DO: Adjust the function for ID */

Token funcID(jer_char lexeme[]) {
	Token currentToken = { 0 };
	size_t length = strlen(lexeme);
	jer_char lastch = lexeme[length - 1];
	jer_intg isID = JER_FALSE;
	switch (lastch) {
	case MNIDPREFIX:
		currentToken.code = MNID_T;
		isID = JER_TRUE;
		break;
	default:
		// Test Keyword
		currentToken = funcKEY(lexeme);
		break;
	}
	if (isID == JER_TRUE) {
		lexeme[length - 1] = '\0';
		strncpy(currentToken.attribute.idLexeme, lexeme, VID_LEN);
		currentToken.attribute.idLexeme[VID_LEN] = CHARSEOF0;
	}
	return currentToken;
}



/*
************************************************************
 * Acceptance State Function SL
 *		Function responsible to identify SL (string literals).
 * - The lexeme must be stored in the String Literal Table
 *   (stringLiteralTable). You need to include the literals in
 *   this structure, using offsets. Remember to include \0 to
 *   separate the lexemes. Remember also to incremente the line.
 ***********************************************************
 */
 /* TO_DO: Adjust the function for SL */

Token funcSL(jer_char lexeme[]) {
	Token currentToken = { 0 };
	jer_intg i = 0, len = (jer_intg)strlen(lexeme);
	currentToken.attribute.contentString = readerGetPosWrte(stringLiteralTable);
	for (i = 1; i < len - 1; i++) {
		if (lexeme[i] == '\n')
			line++;
		if (!readerAddChar(stringLiteralTable, lexeme[i])) {
			currentToken.code = RTE_T;
			strcpy(currentToken.attribute.errLexeme, "Run Time Error:");
			errorNumber = RTE_CODE;
			return currentToken;
		}
	}
	if (!readerAddChar(stringLiteralTable, CHARSEOF0)) {
		currentToken.code = RTE_T;
		strcpy(currentToken.attribute.errLexeme, "Run Time Error:");
		errorNumber = RTE_CODE;
		return currentToken;
	}
	readerAddChar(stringLiteralTable, ' ');
	currentToken.code = STR_T;
	return currentToken;
}


/*
************************************************************
 * This function checks if one specific lexeme is a keyword.
 * - Tip: Remember to use the keywordTable to check the keywords.
 ***********************************************************
 */
 /* TO_DO: Adjust the function for Keywords */

Token funcKEY(jer_char lexeme[]) {
	Token currentToken = { 0 };
	jer_intg kwindex = -1, j = 0;
	for (j = 0; j < KWT_SIZE; j++)
		if (strcmp(lexeme, &keywordTable[j][0]) == 0)
		{
			kwindex = j;
		}
	if (kwindex != -1) {
		currentToken.code = KW_T;
		currentToken.attribute.codeType = kwindex;
	}
	else {
		currentToken.code = VAR_T;
		strcpy(currentToken.attribute.idLexeme, lexeme);
	}
	return currentToken;
}


/*
************************************************************
 * Acceptance State Function Error
 *		Function responsible to deal with ERR token.
 * - This function uses the errLexeme, respecting the limit given
 *   by ERR_LEN. If necessary, use three dots (...) to use the
 *   limit defined. The error lexeme contains line terminators,
 *   so remember to increment line.
 ***********************************************************
 */
 /* TO_DO: Adjust the function for Errors */

Token funcErr(jer_char lexeme[]) {
	Token currentToken = { 0 };
	jer_intg i = 0, len = (jer_intg)strlen(lexeme);
	if (len > ERR_LEN) {
		strncpy(currentToken.attribute.errLexeme, lexeme, ERR_LEN - 3);
		currentToken.attribute.errLexeme[ERR_LEN - 3] = CHARSEOF0;
		strcat(currentToken.attribute.errLexeme, "...");
	}
	else {
		strcpy(currentToken.attribute.errLexeme, lexeme);
	}
	for (i = 0; i < len; i++)
		if (lexeme[i] == '\n')
			line++;
	currentToken.code = ERR_T;
	return currentToken;
}


/*
 ************************************************************
 * The function prints the token returned by the scanner
 ***********************************************************
 */

jer_void printToken(Token t) {
	extern jer_char* keywordTable[]; /* link to keyword table in */
	switch (t.code) {
	case RTE_T:
		printf("RTE_T\t\t%s", t.attribute.errLexeme);
		/* Call here run-time error handling component */
		if (errorNumber) {
			printf("%d", errorNumber);
			exit(errorNumber);
		}
		printf("\n");
		break;
	case ERR_T:
		printf("ERR_T\t\t%s\n", t.attribute.errLexeme);
		break;
	case SEOF_T:
		printf("SEOF_T\t\t%d\t\n", t.attribute.seofType);
		break;
	case MNID_T:
		printf("MNID_T\t\t%s\n", t.attribute.idLexeme);
		break;
	case VAR_T:
		printf("VAR_T\t\t\t %s\n", t.attribute.idLexeme);
		break;
	case RLO_T:
		printf("RLO_T\t\t\t \n");
		break;
	case ART_T:
		printf("ART_T\t\t\t \n");
		break;
	case LGO_T:
		printf("LGO_T\t\t\t \n");
		break;
	case INL_T:
		printf("INL_T\t\t%d\n", t.attribute.intValue);
		break;
	case FL_T:
		printf("FL_T\t\t%g\n", t.attribute.floatValue);
		break;
	case STR_T:
		printf("STR_T\t\t%d\t ", (jer_intg)t.attribute.codeType);
		printf("%s\n", readerGetContent(stringLiteralTable, (jer_intg)t.attribute.codeType));
		break;
	case LPR_T:
		printf("LPR_T\t\t(\n");
		break;
	case RPR_T:
		printf("RPR_T\n");
		break;
	case SMC_T:
		printf("SMC_T\n");
		break;
	case COM_T:
		printf("COM_T\n");
		break;
	case RBR_T:
		printf("RBR_T\n");
		break;
	case KW_T:
		printf("KW_T\t\t%s\n", keywordTable[t.attribute.codeType]);
		break;
	case ASS_T:
		printf("ASS_T\n");
		break;
	default:
		printf("Scanner error: invalid token code: %d\n", t.code);
	}
}

/*
TO_DO: (If necessary): HERE YOU WRITE YOUR ADDITIONAL FUNCTIONS (IF ANY).
*/
