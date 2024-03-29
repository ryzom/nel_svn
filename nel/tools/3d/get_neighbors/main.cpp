
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void	nameToXY(const char *str, int &x, int &y)
{
	if (strchr(str, '_') == NULL)
	{
		fprintf(stderr, "invalid zone name %s\n", str);
		abort();
	}

	x = 0;
	y = 0;

	while (*str != '_')
		y = y*10 + *(str++)-'0';
	y--;

	++str;

	x = (toupper(str[0])-'A')*26+(toupper(str[1])-'A');
}

void	XYToName(int x, int y, char *str)
{
	sprintf(str,"%d_%c%c ", y+1, 'A'+x/26, 'A'+x%26);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "invalid usage\n");
		abort();
	}

	char	dump[128];
	char	output[256];

	output[0] = '\0';

	int		x, y;
	nameToXY(argv[1], x, y);

	int		i, j;

	for (i=-1; i<=1; ++i)
	{
		for (j=-1; j<=1; ++j)
		{
			XYToName(x+i, y+j, dump);
			strcat(output, dump);
		}
	}

	fprintf(stdout, "%s", output);

	return 0;
}
