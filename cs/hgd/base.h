struct base_session
	{};

struct huge_base {
	struct base_session* session_list;
	int session_list_size;
};
