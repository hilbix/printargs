/* $Header$
 *
 * Usage: printargs [args]
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 *
 * $Log$
 * Revision 1.1  2007-10-03 23:41:56  tino
 * First version
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

/* make a small exe if compiled with `diet gcc -Os'
 */
static char	out_buf[1024];
static int	out_pos, out_fd;

static void
outflush(void)
{
  int	i;
  
  for (i=0; i<out_pos; )
    {
      int	put;

      put	= write(out_fd, out_buf+i, out_pos-i);
      if (put>=0)
        {
	  i	+= put;
	  continue;
	}
      if (put<0 && errno==EINTR)
        continue;
      exit(1);
    }
  out_pos	= 0;
}

static void
outfd(int fd)
{
  if (out_fd!=fd)
    outflush();
  out_fd	= fd;
}

static void
outc(char c)
{
  if (out_pos>=sizeof out_buf)
    outflush();
  out_buf[out_pos++]	= c;
}

static void
outs(const char *s)
{
  while (*s)
    outc(*s++);
}

static void
outul_n(unsigned long u, unsigned base, int len)
{
  if ((u/base)>0 || --len>0)
    outul_n(u/base, base, len);
  outc("0123456789abcdef"[u%base]);
}

static void
outul(unsigned long u)
{
  outul_n(u, 10, 0);
}

int
main(int argc, char **argv)
{
  const char	*s;
  int		i;

  outfd(2);
  outs("uid\t");	outul(getuid());	outs("\n");
  outs("euid\t");	outul(geteuid());	outs("\n");
  outs("gid\t");	outul(getgid());	outs("\n");
  outs("egid\t");	outul(getegid());	outs("\n");

  for (i=0; i<argc; )
    {
      outs("arg");
      outul(i);
      outs(":\t");
      outfd(1);
      outs("$\'");
      for (s=argv[i]; *s; s++)
	{
	  if (((signed char)*s)<33 || *s=='\'' || *s=='$')
	    {
	      outc('\\');
	      outul_n((unsigned char)*s, 8, 3);
	    }
	  else
	    switch (*s)
	      {
	      case '\\':
		outc('\\');
	      default:
		outc(*s);
		break;
	      }
	}
      outc('\'');
      if (++i<argc)
	outc(' ');
      outfd(2);
      outc('\n');
    }
  outflush();
  return 0;
}
