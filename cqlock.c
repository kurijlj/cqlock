/* cqlock.c - display current time in a qlocktwo like fashion.
 *
 * Copyright (C) 2015 Ljubomir Kurij <kurijlj@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Terminal application to display current time in the qlocktwo fashion. It is C
 * implementation of script written for conky (http://pastebin.com/wK7JmG9H).
 * Mid (midtone) color scheme was inspired by solarized color scheme
 * (http://ethanschoonover.com/solarized) so it is best used with background
 * color #002B36.
 *
 * 2015-01-10 Ljubomir Kurij <kurijlj@gmail.com>
 *
 * * cqlock.c: created.
 */

/* required by argp engine */
#include <argp.h>
/* required by gettext and others */
#include <libintl.h>
/* required by setlocale */
#include <locale.h>
/* required by strrchr and strncmp */
#include <string.h>
/* required by EXIT_SUCCESS */
#include <stdlib.h>
/* required by struct tm, time_t, time and localtime */
#include <time.h>



#ifndef APP_NAME
#	define APP_NAME "cqlock"
#endif

#ifndef VERSION_STRING
#	define VERSION_STRING "1.0"
#endif

#ifndef YEAR_STRING
#	define YEAR_STRING "2015"
#endif

#ifndef AUTHOR_NAME
#	define AUTHOR_NAME "Ljubomir Kurij"
#endif

#ifndef AUTHOR_MAIL
#	define AUTHOR_MAIL "<kurijlj@gmail.com>"
#endif

#ifndef PATH_SEPARATOR
#	define PATH_SEPARATOR '/'
#endif


/**
 * Definitions required for localisation.
 */
#ifndef PACKAGE
#	define PACKAGE APP_NAME
#endif

/* Change this for release version. */
#ifndef LOCALEDIR
#	define LOCALEDIR "./"
#endif

#define _(str) gettext (str)
#define N_(str) (str)


/**
 * ANSI color escape codes.
 */
#define COLOR_WHITE      "\x1b[38;5;255m"
#define COLOR_DIM_WHITE  "\x1b[38;5;253m"
#define COLOR_DARK_WHITE "\x1b[38;5;245m"
#define COLOR_LIGHT_GRAY "\x1b[38;5;237m"
#define COLOR_DARK_GRAY  "\x1b[38;5;235m"
#define COLOR_BLACK      "\x1b[38;5;0m"
#define COLOR_RESET      "\x1b[0m"


/**
 * scheme - Structure to hold color scheme information.
 */
typedef struct
{
	const char *title;
	const char *accent;
	const char *frgrnd;
	const char *bkgrnd;
} ColorScheme;

/**
 * Define color schemes. Title must not be longer than 20 characters.
 */
static ColorScheme schemes[] = {
	{
		.title = "light",
		.accent = COLOR_BLACK,
		.frgrnd = COLOR_DIM_WHITE,
		.bkgrnd = COLOR_WHITE
	},
	{
		.title = "mid",
		.accent = COLOR_DARK_WHITE,
		.frgrnd = COLOR_LIGHT_GRAY,
		.bkgrnd = COLOR_DARK_GRAY
	},
	{
		.title = "dark",
		.accent = COLOR_WHITE,
		.frgrnd = COLOR_DARK_GRAY,
		.bkgrnd = COLOR_BLACK
	},
	{NULL}
};

/**
 * Points to scheme being used.
 */
static ColorScheme *scheme;

/**
 * Color reset sequence;
 */
const char reset[] = COLOR_RESET;

/**
 * validate_scheme_title - Validate if given string is title for one of
 * predefined color schemes.
 */
static unsigned short validate_scheme_title (const char *title)
{
	unsigned int c = 0;

	if (NULL != title)
	{
		while (NULL != schemes[c].title)
		{
			if (!strncmp (schemes[c].title, title, strnlen (schemes[c].title, 20)))
				return 1;
			c++;
		}
	}

	return 0;
}

static ColorScheme *scheme_by_title (const char *title)
{
	ColorScheme *r = &schemes[2];
	unsigned int c = 0;

	if (NULL != title)
	{
		while (NULL != schemes[c].title)
		{
			if (!strncmp (schemes[c].title, title, strnlen (schemes[c].title, 20)))
			{
				r = &schemes[c];
				break;
			}
			c++;
		}
	}

	return r;
}


/**
 * Name under which this program was invoked.
 */
const char *exec_name;


void
print_version (FILE *stream, struct argp_state *state)
{
	fprintf(stream, "%s%s%s%s%s%s%s%s", APP_NAME, " ", VERSION_STRING, " \
Copyright (C) ", YEAR_STRING, " ", AUTHOR_NAME, _(".\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\n"));
}


/**
 * argp engine initialization. For details wisit page:
 * https://www.gnu.org/software/libc/manual/html_node/Argp.html.
 */
const char *argp_program_version = APP_NAME " " VERSION_STRING;
void (*argp_program_version_hook) () = print_version;
const char *argp_program_bug_address = AUTHOR_MAIL;
static const char doc[] = N_(APP_NAME " - display current time in a qlocktwo \
like fashion.");

/**
 * Options we process.
 */
static struct argp_option options[] = {
	{
		"color-scheme",
		's',
		"SCHEME",
		/* OPTION_ARG_OPTIONAL, */
		0,
		"Select color scheme to be used for displaying time. \
Color schemes supported at this moment are: 'light', 'mid', 'dark'. Default \
color scheme is 'dark'."
	},
	{NULL}
};

/**
 * Used by main to communicate with parse_opt.
 */
struct arguments
{
	char *scheme_title;
};

/**
 * Parse a single option.
 */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
	/**
	 * Get the input argument from argp_parse, which we know is a pointer to our
	 * arguments structure.
	 */
	struct arguments *arguments = state->input;
	
	switch (key)
		{
		case 's':
			if (!validate_scheme_title (arg))
				argp_usage (state);
			arguments->scheme_title = arg;
			break;
		
		default:
			return ARGP_ERR_UNKNOWN;
		}

	return 0;
}

/**
 * Our argp parser.
 */
static struct argp argp = {options, parse_opt, NULL, doc};



/**
 * Clock display switch functions.
 */
const char *
isAM (const struct tm * ct)
{
	const char * r = scheme->frgrnd;
	if (12 > ct->tm_hour)
	{
		r = scheme->accent;
	}

	return r;
}

const char *
isPM (const struct tm * ct)
{
	const char * r = scheme->frgrnd;
	if (12 <= ct->tm_hour)
	{
		r = scheme->accent;
	}

	return r;
}

const char *
isOclock (const struct tm * ct)
{
	const char * r = scheme->frgrnd;
	if (0 <= ct->tm_min && 5 > ct->tm_min)
	{
		r = scheme->accent;
	}

	return r;
}

const char *
isPast (const struct tm * ct)
{
	const char * r = scheme->frgrnd;
	if (34 >= ct->tm_min && 5 <= ct->tm_min)
	{
		r = scheme->accent;
	}

	return r;
}

const char *
isTo (const struct tm * ct)
{
	const char * r = scheme->frgrnd;
	if (34 < ct->tm_min)
	{
		r = scheme->accent;
	}

	return r;
}

const char *
isFiveMin (const struct tm * ct)
{
	const char * r = scheme->frgrnd;
	if ((5 <= ct->tm_min && 10 > ct->tm_min) ||
		(25 <= ct->tm_min && 30 > ct->tm_min) ||
		(35 <= ct->tm_min && 40 > ct->tm_min) ||
		(55 <= ct->tm_min))
	{
		r = scheme->accent;
	}

	return r;
}

const char *
isTenMin (const struct tm * ct)
{
	const char * r = scheme->frgrnd;
	if ((10 <= ct->tm_min && 15 > ct->tm_min) ||
		(50 <= ct->tm_min && 55 > ct->tm_min))
	{
		r = scheme->accent;
	}

	return r;
}

const char *
isQuarter (const struct tm * ct)
{
	const char * r = scheme->frgrnd;
	if ((15 <= ct->tm_min && 20 > ct->tm_min) ||
		(45 <= ct->tm_min && 50 > ct->tm_min))
	{
		r = scheme->accent;
	}

	return r;
}

const char *
isTwentyMin (const struct tm * ct)
{
	const char * r = scheme->frgrnd;
	if ((20 <= ct->tm_min && 30 > ct->tm_min) ||
		(35 <= ct->tm_min && 45 > ct->tm_min))
	{
		r = scheme->accent;
	}

	return r;
}

const char *
isThirtyMin (const struct tm * ct)
{
	const char * r = scheme->frgrnd;
	if (30 <= ct->tm_min && 35 > ct->tm_min)
	{
		r = scheme->accent;
	}

	return r;
}

const char *
isHour (const struct tm * ct, int hr)
{
	const char * r = scheme->frgrnd;

	/*
	if ((hr <= ct->tm_hour && (hr + 1) > ct->tm_hour) ||
		((hr + 12) <= ct->tm_hour && (hr + 13) > ct->tm_hour))
	*/
	if ((((hr == ct->tm_hour) || ((12 == hr ? hr - 12 : hr + 12) == ct->tm_hour)) &&
		 (0 <= ct->tm_min && 34 >= ct->tm_min)) ||
		((((hr - 1) == ct->tm_hour) || ((hr + 11) == ct->tm_hour)) &&
		 (34 < ct->tm_min && 59 >= ct->tm_min)))
	{
		r = scheme->accent;
	}

	return r;
}




int
main (int argc, char **argv)
{
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (PACKAGE, "utf-8");
	textdomain (PACKAGE);


	/**
	 * Construct the name of the executable, without the directory part.
	 * The strrchr() function returns a pointer to the last occurrence of
	 * the character in the string.
	 */
	exec_name = strrchr (argv[0], PATH_SEPARATOR);
	if (!exec_name)
		exec_name = argv[0];
	else
		++exec_name;


	/**
	 * Parse options.
	 */
	struct arguments arguments;

	argp_parse (&argp, argc, argv, 0, 0, &arguments);


	/**
	 * Define color schemes.
	 */
	/*
	const char *light[] = {COLOR_BLACK, COLOR_DIM_WHITE};
	const char *mid[]   = {COLOR_DARK_WHITE, COLOR_LIGHT_GRAY};
	const char *dark[]  = {COLOR_WHITE, COLOR_DARK_GRAY};
	const char reset[] = COLOR_RESET;
	*/


	struct tm *current_clock;
	time_t simple_clock;


	/* scheme = &schemes[2]; */
	scheme = scheme_by_title (arguments.scheme_title);
	simple_clock = time (NULL);
	current_clock = localtime (&simple_clock);

	printf ("\n\n");
	printf (_("\t%sI  T  %sL  %sI  S  %sB  F  %sA  M  %sP  M%s\n"),
		scheme->accent,
		scheme->frgrnd,
		scheme->accent,
		scheme->frgrnd,
		isAM (current_clock),
		isPM (current_clock),
		reset);
	printf (_("\t%sA  C  %sQ  U  A  R  T  E  R  %sD  C%s\n"),
		scheme->frgrnd,
		isQuarter (current_clock),
		scheme->frgrnd,
		reset);
	printf (_("\t%sT  W  E  N  T  Y  %sF  I  V  E  %sX%s\n"),
		isTwentyMin (current_clock),
		isFiveMin (current_clock),
		scheme->frgrnd,
		reset);
	printf (_("\t%sH  A  L  F  %sB  %sT  E  N  %sF  %sT  O%s\n"),
		isThirtyMin (current_clock),
		scheme->frgrnd,
		isTenMin (current_clock),
		scheme->frgrnd,
		isTo (current_clock),
		reset);
	printf (_("\t%sP  A  S  T  %sE  R  U  %sN  I  N  E%s\n"),
		isPast (current_clock),
		scheme->frgrnd,
		isHour (current_clock, 9),
		reset);
	printf (_("\t%sO  N  E  %sS  I  X  %sT  H  R  E  E%s\n"),
		isHour (current_clock, 1),
		isHour (current_clock, 6),
		isHour (current_clock, 3),
		reset);
	printf (_("\t%sF  O  U  R  %sF  I  V  E  %sT  W  O%s\n"),
		isHour (current_clock, 4),
		isHour (current_clock, 5),
		isHour (current_clock, 2),
		reset);
	printf (_("\t%sE  I  G  H  T  %sE  L  E  V  E  N%s\n"),
		isHour (current_clock, 8),
		isHour (current_clock, 11),
		reset);
	printf (_("\t%sS  E  V  E  N  %sT  W  E  L  V  E%s\n"),
		isHour (current_clock, 7),
		isHour (current_clock, 12),
		reset);
	printf (_("\t%sT  E  N  %sS  E  %sO' C  L  O  C  K%s\n"),
		isHour (current_clock, 10),
		scheme->frgrnd,
		isOclock (current_clock),
		reset);
	printf ("\n\n\n");

	return EXIT_SUCCESS;
}
