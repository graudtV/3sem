/********************************************************************************
 * This file helps to print colored text in console on Linux and Mac OS			*
 ********************************************************************************
 * Usage examples																*
 * 1) Most convenient way to emphasize some words - 							*
 *	- CONSOLE_<COLOR> functions (CONSOLE_RED(), CONSOLE_GREEN(), etc.)			*
 * 	ex:	printf("Programm " CONSOLE_ERROR("failed") "\n");						*
 *	// CONSOLE_ERROR() prints text using default color for errors				*
 * 2) The same as point 1, but with more settings - 							*
 *	- CONSOLE() function														*
 *  ex:	printf( CONSOLE("f l e x", PURPLE, YELLOW, BOLD) "\n" );				*
 *  // PURPLE - text color, YELLOW - background color, BOLD - attribute			*
 * 3) If you need to set one color for many printf() calls						*
 *		or more than one attribute -											*
 *	- CONSOLE_COLOR_<COLOR>, CONSOLE_BGCOLOR_<COLOR>, CONSOLE_ATTRIBUTE_<ATT>	*
 *	ex:																			*
 *	printf("\n");																*
 *	printf( CONSOLE_COLOR_GREEN CONSOLE_BGCOLOR_BLUE CONSOLE_ATTRIBUTE_BOLD );	*
 *	printf(" π                   |π      \n");									*
 *	printf(" ∫ cos(x)dx = sin(x) |  = 0; \n");									*
 *	printf(" 0                   |0      ");									*
 *	printf( CONSOLE_DFLT );														*
 *	printf("\n\n");																*
 ********************************************************************************
 * Available text and background colors:										*
 *	- BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, DARKBLUE, WHITE					*
 * Available attributes:														*
 *	- BOLD, UNDERLINED, BLINK, INVERTED, INVISIBLE								*
 * Special presets (both colors and attributes):								*
 *	- ERROR, INFO, WARNING														*
 ********************************************************************************
 * Author - Graudt V. https://github.com/graudtV
 ********************************************************************************/

#ifndef _CONSOLE_COLORS_H_
#define _CONSOLE_COLORS_H_

/* Colors */
#define CONSOLE_COLOR_NONE	// doesn't change text color
#define CONSOLE_COLOR_BLACK				"\033[30m"
#define CONSOLE_COLOR_RED				"\033[31m"
#define CONSOLE_COLOR_GREEN				"\033[32m"
#define CONSOLE_COLOR_YELLOW			"\033[33m"
#define CONSOLE_COLOR_BLUE				"\033[34m"
#define CONSOLE_COLOR_PURPLE			"\033[35m"
#define CONSOLE_COLOR_DARKBLUE			"\033[36m"
#define CONSOLE_COLOR_WHITE 			"\033[37m"

/* Attributes */
#define CONSOLE_ATTRIBUTE_NONE	// doens't change attributes
#define CONSOLE_ATTRIBUTE_BOLD			"\033[1m"
#define CONSOLE_ATTRIBUTE_UNDERLINED	"\033[4m"
#define CONSOLE_ATTRIBUTE_BLINK			"\033[5m"
#define CONSOLE_ATTRIBUTE_INVERTED		"\033[7m"
#define CONSOLE_ATTRIBUTE_INVISIBLE		"\033[8m"

/* Background colors */
#define CONSOLE_BGCOLOR_NONE	// doesn't change background color
#define CONSOLE_BGCOLOR_BLACK			"\033[40m"
#define CONSOLE_BGCOLOR_RED				"\033[41m"
#define CONSOLE_BGCOLOR_GREEN			"\033[42m"
#define CONSOLE_BGCOLOR_YELLOW			"\033[43m"
#define CONSOLE_BGCOLOR_BLUE			"\033[44m"
#define CONSOLE_BGCOLOR_PURPLE			"\033[45m"
#define CONSOLE_BGCOLOR_DARKBLUE		"\033[46m"
#define CONSOLE_BGCOLOR_WHITE 			"\033[47m"

/* Default color, background color and attribute for console */
#define CONSOLE_DFLT "\033[m"

/* Default colors and attributes for frequent messages */
/* error message */
#define CONSOLE_COLOR_ERROR 		CONSOLE_COLOR_RED
#define CONSOLE_BGCOLOR_ERROR		CONSOLE_BGCOLOR_NONE
#define CONSOLE_ATTRIBUTE_ERROR 	CONSOLE_ATTRIBUTE_BOLD
/* info message */
#define CONSOLE_COLOR_INFO 			CONSOLE_COLOR_GREEN
#define CONSOLE_BGCOLOR_INFO		CONSOLE_BGCOLOR_NONE
#define CONSOLE_ATTRIBUTE_INFO		CONSOLE_ATTRIBUTE_NONE
/* warning message */
#define CONSOLE_COLOR_WARNING		CONSOLE_COLOR_PURPLE
#define CONSOLE_BGCOLOR_WARNING		CONSOLE_BGCOLOR_NONE
#define CONSOLE_ATTRIBUTE_WARNING	CONSOLE_ATTRIBUTE_BOLD

/* Adds color and attributes settings to given text */
#define CONSOLE(text, color, bgcolor, attribute) CONSOLE_COLOR_##color CONSOLE_BGCOLOR_##bgcolor CONSOLE_ATTRIBUTE_##attribute text CONSOLE_DFLT

/* Macro-functions for more convenient interface */
/* Equivalent to CONSOLE() macro with corresponding parameters */
#define CONSOLE_BLACK(text)			CONSOLE(text, BLACK,	NONE,	NONE)
#define CONSOLE_RED(text)			CONSOLE(text, RED,		NONE,	NONE)
#define CONSOLE_GREEN(text) 		CONSOLE(text, GREEN,	NONE,	NONE)
#define CONSOLE_YELLOW(text)		CONSOLE(text, YELLOW,	NONE,	NONE)
#define CONSOLE_BLUE(text)			CONSOLE(text, BLUE,		NONE,	NONE)
#define CONSOLE_PURPLE(text)		CONSOLE(text, PURPLE,	NONE,	NONE)
#define CONSOLE_DARKBLUE(text)		CONSOLE(text, DARKBLUE,	NONE,	NONE)
#define CONSOLE_WHITE(text)			CONSOLE(text, WHITE,	NONE,	NONE)

#define CONSOLE_BOLD(text)			CONSOLE(text, NONE,		NONE,	BOLD)
#define CONSOLE_UNDERLINED(text)	CONSOLE(text, NONE,		NONE,	UNDERLINED)
#define CONSOLE_BLINK(text)			CONSOLE(text, NONE,		NONE,	BLINK)
#define CONSOLE_INVERTED(text)		CONSOLE(text, NONE,		NONE,	INVERTED)
#define CONSOLE_INVISIBLE(text)		CONSOLE(text, NONE,		NONE,	INVISIBLE)

#define CONSOLE_ERROR(text)			CONSOLE(text, ERROR, 	ERROR,	 ERROR)
#define CONSOLE_INFO(text)			CONSOLE(text, INFO, 	INFO,	 INFO)
#define CONSOLE_WARNING(text)		CONSOLE(text, WARNING,	WARNING, WARNING)

#endif // _CONSOLE_COLORS_H_
