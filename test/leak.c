#include <stdio.h>
#include <stdlib.h>

char **readBuf()
{
	char **mBuf = (char **) malloc(sizeof(char *));
	*mBuf = (char *) malloc(8 * sizeof(char));
	**mBuf = '2';
	*(*mBuf + 1) = '\0';
	return mBuf;
}

void freeBuf(char **fBuf)
{
	char *z = *fBuf;
	free(z);
	free(fBuf);
}

void SerialReadBuf()
{
	int n;
	for (n = 0; n < 100; ++n) {
		char **buf = readBuf();
		char *tmp = *buf;
		if (*tmp != '\n')
			printf("%s", tmp);
		else
			continue;
		freeBuf(buf);
	}
}

int main()
{
	SerialReadBuf();
	return 0;
}
