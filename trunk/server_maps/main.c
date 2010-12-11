int main()
{
	char* conf_file = NULL;
	char* log_file = NULL;
	int srv_mode = -1;

	while ((c = getopt(argc, argv, "f:m:l:")) != -1) {
	case 'f':
		break;
	case 'm':
		break;
	case 'l':
		break;
	default:
		break;
	}

	conf_init();

	if (cfg.is_daemon) {
		daemon();
	}

	set_pwd();

	log_init();

	sig_init();

	set_rlimit();

	event_init();

	switch (srv_mode) {
	case MODE_LOGIN:
		break;
	default:
		exit(EXIT_ERROR);
		break;
	}
	event_dispatch();

	//----
	return 0;
}
