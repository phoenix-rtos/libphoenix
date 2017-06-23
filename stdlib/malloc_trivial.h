struct malloc_common {
	handle_t mutex;
	list_t page_sets;
};

extern struct malloc_common malloc_common;
