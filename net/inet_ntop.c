#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

const char *inet_ntop(int af, const void *restrict src,
		       char *restrict dst, socklen_t size) 
{
	if (af == AF_INET) {
		if (size < 16 || dst == NULL || src == NULL)
			return NULL;

		snprintf(dst, size, "%u.%u.%u.%u",
				(unsigned int)((unsigned char *)src)[0],
				(unsigned int)((unsigned char *)src)[1],
				(unsigned int)((unsigned char *)src)[2],
				(unsigned int)((unsigned char *)src)[3]);

		return dst;
	} else if (af == AF_INET6) {
		memset(dst, 0, size);
		memcpy(dst, "::", strlen("::"));

		return dst;
	}

	return NULL;
}
