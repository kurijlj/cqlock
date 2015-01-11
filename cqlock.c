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
 * Mid (midtone) theme was inspired by solarized theme
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
/* required by strrchr */
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
#define COLOR_DIM_WHITE  "\x1b[38;5;252m"
#define COLOR_DARK_WHITE "\x1b[38;5;245m"
#define COLOR_LIGHT_GRAY "\x1b[38;5;237m"
#define COLOR_DARK_GRAY  "\x1b[38;5;235m"
#define COLOR_BLACK      "\x1b[38;5;0m"
#define COLOR_RESET      "\x1b[0m"


/**
 * Name under which this program was invoked.
 */
const char *exec_name;


const char * (*theme) [2];

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


static struct argp argp = {0, 0, 0, doc};




const char *
isAM (const struct tm * ct)
{
	const char * r = (*theme) [1];
	if (12 > ct->tm_hour)
	{
		r = (*theme) [0];
	}

	return r;
}

const char *
isPM (const struct tm * ct)
{
	const char * r = (*theme) [1];
	if (12 <= ct->tm_hour)
	{
		r = (*theme) [0];
	}

	return r;
}

const char *
isPast (const struct tm * ct)
{
	const char * r = (*theme) [1];
	if (30 >= ct->tm_min)
	{
		r = (*theme) [0];
	}

	return r;
}

const char *
isTo (const struct tm * ct)
{
	const char * r = (*theme) [1];
	if (30 < ct->tm_min)
	{
		r = (*theme) [0];
	}

	return r;
}

const char *
isFiveMin (const struct tm * ct)
{
	const char * r = (*theme) [1];
	if ((5 <= ct->tm_min && 10 > ct->tm_min) ||
		(25 <= ct->tm_min && 30 > ct->tm_min) ||
		(35 <= ct->tm_min && 40 > ct->tm_min) ||
		(55 <= ct->tm_min))
	{
		r = (*theme) [0];
	}

	return r;
}

const char *
isTenMin (const struct tm * ct)
{
	const char * r = (*theme) [1];
	if ((10 <= ct->tm_min && 15 > ct->tm_min) ||
		(50 <= ct->tm_min && 55 > ct->tm_min))
	{
		r = (*theme) [0];
	}

	return r;
}

const char *
isQuarter (const struct tm * ct)
{
	const char * r = (*theme) [1];
	if ((15 <= ct->tm_min && 20 > ct->tm_min) ||
		(45 <= ct->tm_min && 50 > ct->tm_min))
	{
		r = (*theme) [0];
	}

	return r;
}

const char *
isTwentyMin (const struct tm * ct)
{
	const char * r = (*theme) [1];
	if ((20 <= ct->tm_min && 30 > ct->tm_min) ||
		(35 <= ct->tm_min && 45 > ct->tm_min))
	{
		r = (*theme) [0];
	}

	return r;
}

const char *
isThirtyMin (const struct tm * ct)
{
	const char * r = (*theme) [1];
	if (30 <= ct->tm_min && 35 > ct->tm_min)
	{
		r = (*theme) [0];
	}

	return r;
}

const char *
isHour (const struct tm * ct, int hr)
{
	const char * r = (*theme) [1];
	if ((hr <= ct->tm_hour && (hr + 1) > ct->tm_hour) ||
		((hr + 12) <= ct->tm_hour && (hr + 13) > ct->tm_hour))
	{
		r = (*theme) [0];
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

	argp_parse (&argp, argc, argv, 0, 0, 0);

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

	const char *light[] = {COLOR_BLACK, COLOR_DIM_WHITE};
	const char *mid[]   = {COLOR_DARK_WHITE, COLOR_LIGHT_GRAY};
	const char *dark[]  = {COLOR_WHITE, COLOR_DARK_GRAY};
	const char reset[] = COLOR_RESET;

	struct tm *current_clock;
	time_t simple_clock;

	theme = &dark;
	simple_clock = time (NULL);
	current_clock = localtime (&simple_clock);

	printf ("\n\n");
	printf (_("\t%sI  T  %sL  %sI  S  %sB  F  %sA  M  %sP  M%s\n"),
		(*theme) [0],
		(*theme) [1],
		(*theme) [0],
		(*theme) [1],
		isAM (current_clock),
		isPM (current_clock),
		reset);
	printf (_("\t%sA  C  %sQ  U  A  R  T  E  R  %sD  C%s\n"),
		(*theme) [1],
		isQuarter (current_clock),
		(*theme) [1],
		reset);
	printf (_("\t%sT  W  E  N  T  Y  %sF  I  V  E  %sX%s\n"),
		isTwentyMin (current_clock),
		isFiveMin (current_clock),
		(*theme) [1],
		reset);
	printf (_("\t%sH  A  L  F  %sB  %sT  E  N  %sF  %sT  O%s\n"),
		isThirtyMin (current_clock),
		(*theme) [1],
		isTenMin (current_clock),
		(*theme) [1],
		isTo (current_clock),
		reset);
	printf (_("\t%sP  A  S  T  %sE  R  U  %sN  I  N  E%s\n"),
		isPast (current_clock),
		(*theme) [1],
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
		isHour (current_clock, 0),
		reset);
	printf (_("\t%sT  E  N  %sS  E  O' C  L  O  C  K%s\n"),
		isHour (current_clock, 10),
		(*theme) [1],
		reset);
	printf ("\n\n\n");

	return EXIT_SUCCESS;
}
