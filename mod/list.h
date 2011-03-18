struct node_t {
	struct node_t* pre;
	struct node_t* next;
	void* data;
};

struct list_t {
	struct node_t* head;
	struct node_t* tail;
	unsigned int len;
};

//-----------------------------------------------------
struct node_t* node_new(void* data);


//-----------------------------------------------------
struct list_t* list_new();
int list_add(struct list_t* list, struct node_t* node);
int list_del(struct list_t* list, struct node_t* node);
int list_free(struct list_t* list);
int list_print(struct list_t* list);
