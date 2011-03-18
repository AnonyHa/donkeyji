struct mod_t {
	int (*mod_init)();
	int (*mod_run)();
	int (*mod_exit)();
};
