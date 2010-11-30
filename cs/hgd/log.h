struct huge_log
{
	int log_fd;
};

int log_init();
int log_close();

int log_debug();
int log_warn();
int log_err();
int log_msg();
