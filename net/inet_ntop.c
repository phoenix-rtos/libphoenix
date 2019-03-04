#include <arpa/inet.h>
#include <string.h>

const char *inet_ntop(int af, const void *restrict src,
		       char *restrict dst, socklen_t size) 
{
	if (af == AF_INET6) {
		memset(dst, 0, size);
		memcpy(dst, "::", strlen("::"));
		return dst;
	}
	return NULL;
}
