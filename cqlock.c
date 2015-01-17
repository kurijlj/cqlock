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
 * Terminal application that displays current time in the qlocktwo fashion.
 * It is C implementation of script written for conky
 * (http://pastebin.com/wK7JmG9H).
 *
 * Color schemes were inspired by solarized color scheme
 * (http://ethanschoonover.com/solarized). Mid color scheme is best used with
 * background color #002B36.
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
 * Maximum scheme title length must not exceed 20 chars.
 */
#define CQLOCK_TITLE_LNGT 20


/**
 * scheme - Structure to hold color scheme information.
 */
typedef struct
{
	const char *title;       /* scheme title */
	const char *description; /* string that describes given color scheme
				    more closely or holds notes for using
				    scheme. Used when displaying schemes list. */
	const char *accent;      /* color escape code for accented characters */
	const char *frgrnd;      /* color escape code for unaccented characters */
	const char *bkgrnd;      /* color escape code for background
				    (not enabled yet) */
} ColorScheme;

/**
 * Define color schemes. Title must not be longer than 20 characters and it also
 * must be unique. It should be good to keep schemes sorted ascending by title,
 * too.
 */
static ColorScheme schemes[] = {
	{
		.title = "light-low",
		.description = N_("Best used on terminals that support only basic \
set of\n\t\tcolor codes."),
		.accent = "\x1b[1;34m",
		.frgrnd = "\x1b[1;30m",
		.bkgrnd = "\x1b[0;47m"
	},
	{
		.title = "mid-low",
		.description = N_("Best used on terminals that support only basic \
set of\n\t\tcolor codes."),
		.accent = "\x1b[1;36m",
		.frgrnd = "\x1b[0;37m",
		.bkgrnd = "\x1b[0;46m"
	},
	{
		.title = "dark-low",
		.description = N_("Best used on terminals that support only basic \
set of\n\t\tcolor codes."),
		.accent = "\x1b[1;37m",
		.frgrnd = "\x1b[1;30m",
		.bkgrnd = "\x1b[0;40m"
	},
	{
		.title = "light-high",
		.description = N_("Best used on terminals that support extended \
set of\n\t\tcolor codes."),
		.accent = "\x1b[38;5;33m",
		.frgrnd = "\x1b[38;5;250m",
		.bkgrnd = "\x1b[38;5;230m"
	},
	{
		.title = "mid-high",
		.description = N_("Best used on terminals that support extended \
set of\n\t\tcolor codes."),
		.accent = "\x1b[38;5;33m",
		.frgrnd = "\x1b[38;5;240m",
		.bkgrnd = "\x1b[38;5;234m"
	},
	{
		.title = "dark-high",
		.description = N_("Best used on terminals that support extended \
set of\n\t\tcolor codes."),
		.accent = "\x1b[38;5;255m",
		.frgrnd = "\x1b[38;5;235m",
		.bkgrnd = "\x1b[38;5;0m"
	},
	{NULL}
};

/**
 * Color reset sequence;
 */
const char reset[] = "\x1b[0m";

/**
 * Pointer to scheme being used.
 */
static ColorScheme *scheme;

/**
 * validate_scheme_title:
 * @title: title to search for in @schemes array
 *
 * It searches @schemes array for first occurrence of scheme with given title.
 * NULL is handled gracefully.
 *
 * Returns: 1 if given string corresponds to one of titles in the @schemes
 * array. Else it returns 0.
 */
static unsigned short
validate_scheme_title (const char *title)
{
	unsigned int c = 0;
	unsigned int r = 0;

	if (NULL != title)
	{
		while (NULL != schemes[c].title)
		{
			if (!strncmp (schemes[c].title,
				title,
				strnlen (schemes[c].title, CQLOCK_TITLE_LNGT)))
			{
				r = 1;
				break; /* No need to search more, so bail out. */
			}
			c++;
		}
	}

	return r;
}

/**
 * scheme_by_title:
 * @title: title to select scheme by
 *
 * It searches @schemes array for first occurrence of scheme with given title.
 * NULL is handled gracefully.
 *
 * Returns: pointer to scheme with given title if such exists. Else it returns
 * pointer to default scheme.
 */
static ColorScheme *
scheme_by_title (const char *title)
{
	ColorScheme *r = &schemes[2]; /* Assign default scheme. */
	unsigned int c = 0;

	if (NULL != title)
	{
		while (NULL != schemes[c].title)
		{
			if (!strncmp (schemes[c].title,
				title,
				strnlen (schemes[c].title, CQLOCK_TITLE_LNGT)))
			{
				r = &schemes[c];
				break; /* No need to search more, so bail out. */
			}
			c++;
		}
	}

	return r;
}

/**
 * list_color_schemes:
 *
 * Prints list of supported color schemes to stdout.
 */
static void
list_color_schemes ()
{
	unsigned int c = 0;

	printf (" scheme\t\tdescription\n");
	printf (" ==========\
\t=====================================================\n");

	while (NULL != schemes[c].title)
	{
		printf (" %s\t%s\n", schemes[c].title, schemes[c].description);
		c++;
	}

	printf ("\n");
}


/**
 * Name under which this program was invoked.
 */
const char *exec_name;


/**
 * print_version:
 * @stream: FILE pointer to stream to output version message to
 * @state: pointer to a struct argp_state, which contains information about the
 *         state of the option parsing
 *
 * Prints application's version information to @stream.
 */
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
 * Define options to process.
 */
static struct argp_option options[] = {
	{
		"list-schemes",
		'l',
		NULL,
		OPTION_ARG_OPTIONAL,
		N_("Display list of supported color schemes.")
	},
	{
		"color-scheme",
		's',
		N_("SCHEME"),
		0,
		N_("Select color scheme to be used for displaying time. For full \
list of supported color schemes run application with -l option. Default color \
scheme is dark-low.")
	},
	{NULL}
};

/**
 * Used by main to communicate with parse_opt.
 */
struct arguments
{
	unsigned int list_schemes;
	char *scheme_title;
};

/**
 * parse_opt:
 * @key: key of an option to process
 * @arg: given value of an option to process
 * @state: state points to a struct argp_state, containing useful information
 *         about the current parsing state for use by parser
 *
 * Parses a single option.
 *
 * Returns: 0 for success, ARGP_ERR_UNKNOWN if the value of key is not handled
 * by this parser function.
 */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
	/**
	 * Get the input argument from argp_parse, which we know is a pointer to
	 * our arguments structure.
	 */
	struct arguments *arguments = state->input;
	
	switch (key)
		{
		case 'l':
			arguments->list_schemes = 1;
			break;
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
 * Light on functions for displaying time.
 */

/**
 * light_am:
 * @ct: pointer to struct tm containing current time information
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns "AM" on clock display by sending correct color escape string to output
 * function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up "AM" on clock display.
 */
const char *
light_am (const struct tm * ct, const ColorScheme *scheme)
{
	const char * r = scheme->frgrnd;
	if (12 > ct->tm_hour)
	{
		r = scheme->accent;
	}

	return r;
}

/**
 * light_pm:
 * @ct: pointer to struct tm containing current time information
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns "PM" on clock display by sending correct color escape string to output
 * function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up "PM" on clock display.
 */
const char *
light_pm (const struct tm * ct, const ColorScheme *scheme)
{
	const char * r = scheme->frgrnd;
	if (12 <= ct->tm_hour)
	{
		r = scheme->accent;
	}

	return r;
}

/**
 * light_oclock:
 * @ct: pointer to struct tm containing current time information
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns "O'CLOCK" on clock display by sending correct color escape string to
 * output function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up "O'CLOCK" on clock display.
 */
const char *
light_oclock (const struct tm * ct, const ColorScheme *scheme)
{
	const char * r = scheme->frgrnd;
	if (0 <= ct->tm_min && 5 > ct->tm_min)
	{
		r = scheme->accent;
	}

	return r;
}

/**
 * light_past:
 * @ct: pointer to struct tm containing current time information
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns "PAST" on clock display by sending correct color escape string to
 * output function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up "PAST" on clock display.
 */
const char *
light_past (const struct tm * ct, const ColorScheme *scheme)
{
	const char * r = scheme->frgrnd;
	if (34 >= ct->tm_min && 5 <= ct->tm_min)
	{
		r = scheme->accent;
	}

	return r;
}


/**
 * light_to:
 * @ct: pointer to struct tm containing current time information
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns "TO" on clock display by sending correct color escape string to
 * output function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up "TO" on clock display.
 */
const char *
light_to (const struct tm * ct, const ColorScheme *scheme)
{
	const char * r = scheme->frgrnd;
	if (34 < ct->tm_min)
	{
		r = scheme->accent;
	}

	return r;
}

/**
 * light_five_min:
 * @ct: pointer to struct tm containing current time information
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns "FIVE" minutes on clock display by sending correct color escape string
 * to output function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up "FIVE" minutes on clock display.
 */
const char *
light_five_min (const struct tm * ct, const ColorScheme *scheme)
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

/**
 * light_ten_min:
 * @ct: pointer to struct tm containing current time information
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns "TEN" minutes on clock display by sending correct color escape string
 * to output function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up "TEN" minutes on clock display.
 */
const char *
light_ten_min (const struct tm * ct, const ColorScheme *scheme)
{
	const char * r = scheme->frgrnd;
	if ((10 <= ct->tm_min && 15 > ct->tm_min) ||
		(50 <= ct->tm_min && 55 > ct->tm_min))
	{
		r = scheme->accent;
	}

	return r;
}

/**
 * light_quarter:
 * @ct: pointer to struct tm containing current time information
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns "QUARTER" on clock display by sending correct color escape string
 * to output function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up "QUARTER" minutes on clock display.
 */
const char *
light_quarter (const struct tm * ct, const ColorScheme *scheme)
{
	const char * r = scheme->frgrnd;
	if ((15 <= ct->tm_min && 20 > ct->tm_min) ||
		(45 <= ct->tm_min && 50 > ct->tm_min))
	{
		r = scheme->accent;
	}

	return r;
}

/**
 * light_twenty_min:
 * @ct: pointer to struct tm containing current time information
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns "TWENTY" minutes on clock display by sending correct color escape string
 * to output function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up "TWENTY" minutes on clock display.
 */
const char *
light_twenty_min (const struct tm * ct, const ColorScheme *scheme)
{
	const char * r = scheme->frgrnd;
	if ((20 <= ct->tm_min && 30 > ct->tm_min) ||
		(35 <= ct->tm_min && 45 > ct->tm_min))
	{
		r = scheme->accent;
	}

	return r;
}

/**
 * light_half_hour:
 * @ct: pointer to struct tm containing current time information
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns "HALF" on clock display by sending correct color escape string
 * to output function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up "HALF" hour on clock display.
 */
const char *
light_half_hour (const struct tm * ct, const ColorScheme *scheme)
{
	const char * r = scheme->frgrnd;
	if (30 <= ct->tm_min && 35 > ct->tm_min)
	{
		r = scheme->accent;
	}

	return r;
}

/**
 * light_hour:
 * @ct: pointer to struct tm containing current time information
 * @hr: which hour to test for
 * @scheme: pointer to color scheme to use for displaying data
 *
 * Turns given hour, if correct, on clock display by sending correct color
 * escape string to output function.
 *
 * Returns: pointer to constant string containing color escape sequence for
 * lighting up given hour on clock display.
 */
const char *
light_hour (const struct tm * ct, int hr, const ColorScheme *scheme)
{
	const char * r = scheme->frgrnd;

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
	arguments.list_schemes = 0; /* Default is FALSE (don't list schemes). */

	argp_parse (&argp, argc, argv, 0, 0, &arguments);

	/* If users selected display schemes option, display list of color
	 * schemes and bail out.
	 */
	if (arguments.list_schemes)
	{
		list_color_schemes ();
		exit (EXIT_SUCCESS);
	}

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
		light_am (current_clock, scheme),
		light_pm (current_clock, scheme),
		reset);
	printf (_("\t%sA  C  %sQ  U  A  R  T  E  R  %sD  C%s\n"),
		scheme->frgrnd,
		light_quarter (current_clock, scheme),
		scheme->frgrnd,
		reset);
	printf (_("\t%sT  W  E  N  T  Y  %sF  I  V  E  %sX%s\n"),
		light_twenty_min (current_clock, scheme),
		light_five_min (current_clock, scheme),
		scheme->frgrnd,
		reset);
	printf (_("\t%sH  A  L  F  %sB  %sT  E  N  %sF  %sT  O%s\n"),
		light_half_hour (current_clock, scheme),
		scheme->frgrnd,
		light_ten_min (current_clock, scheme),
		scheme->frgrnd,
		light_to (current_clock, scheme),
		reset);
	printf (_("\t%sP  A  S  T  %sE  R  U  %sN  I  N  E%s\n"),
		light_past (current_clock, scheme),
		scheme->frgrnd,
		light_hour (current_clock, 9, scheme),
		reset);
	printf (_("\t%sO  N  E  %sS  I  X  %sT  H  R  E  E%s\n"),
		light_hour (current_clock, 1, scheme),
		light_hour (current_clock, 6, scheme),
		light_hour (current_clock, 3, scheme),
		reset);
	printf (_("\t%sF  O  U  R  %sF  I  V  E  %sT  W  O%s\n"),
		light_hour (current_clock, 4, scheme),
		light_hour (current_clock, 5, scheme),
		light_hour (current_clock, 2, scheme),
		reset);
	printf (_("\t%sE  I  G  H  T  %sE  L  E  V  E  N%s\n"),
		light_hour (current_clock, 8, scheme),
		light_hour (current_clock, 11, scheme),
		reset);
	printf (_("\t%sS  E  V  E  N  %sT  W  E  L  V  E%s\n"),
		light_hour (current_clock, 7, scheme),
		light_hour (current_clock, 12, scheme),
		reset);
	printf (_("\t%sT  E  N  %sS  E  %sO' C  L  O  C  K%s\n"),
		light_hour (current_clock, 10, scheme),
		scheme->frgrnd,
		light_oclock (current_clock, scheme),
		reset);
	printf ("\n\n\n");

	return EXIT_SUCCESS;
}
