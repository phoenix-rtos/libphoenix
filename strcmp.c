int strcmp(const char *x, const char *y)
{
	for (; *x != '\0' && *y != '\0'; ++x, ++y) {
		if (*x < *y)
			return -1;
		else if (*x > *y)
			return 1;
	}

	return 0;
}
