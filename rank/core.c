/*
typedef long int dword;
typedef int word;
typedef unsigned char byte;
*/

typedef _score_node{
	int score;
	int cnt;
	void* next_score;
	void* uid_head;
}score_node;

typedef _idx_node{
	int cnt;
	void* score_head;
}idx_node;

typedef _uid_node{
	int uid;
	void* next_uid;
	void* pre_uid;
	void* owner_score;
}uid_node;

typedef _idx_array{
	idx_node* array;
	int len;
}idx_array;

typedef _sub_hash{}sub_hash;
typedef _hash_tbl{}hash_tbl;

idx_array* gia = NULL;//single
hash_tbl* ght = NULL;//single

rank_core* rank_init()
{}

int rank_insert(uid, score)
{
	uid_node* un = uid_get(ght, uid);
	if (un != NULL) {
		return rank_update(uid, score);
	}

	un = uid_new(ght, uid);

	score_node* sn = score_get(gia, score);
	if (sn == NULL) {
		sn = score_new(gia, score);
	}

	//bind score node and uid node
	_bind_score_uid(sn, un);
}

int _bind_score_uid(score_node* sn, uid_node* un)
{}

int _unbind_score_uid(score_node* sn, uid_node* un)
{}

int rank_update(uid, score)
{
	uid_node* un = uid_get(ght, uid);
	if (un == NULL)
		rank_insert(uid, score);

	score_node* old_sn = un->owner_score;

	if (score == old_sn->score)
		return;

	score_node* new_sn = score_get(gia, score);
	if (new_sn == NULL)
		new_sn = score_new(gia, score);


	_unbind_score_uid(old_sn, un);

	_bind_score_uid(new_sn, un);

	return 0;
}

int rank_get_rank(uid)
{
	uid_node* un = uid_get(uid);
	if (un == NULL)
		return -1;

	score_node* sn = un->owner_score;
	int score = sn->score;

	int idx = score / 1000;
	int rank = 0;
	for (int i=10000-1; i>idx; i--) {
		rank += gia->array[i]->cnt;
	}
	score_node* head = gia->array[idx]->score_head;

	score_node* tmp = head;
	while (tmp != sn) {
		rank += tmp->cnt;
		tmp = tmp->next_score;
	}
	return rank;
}

typedef struct _us{
	int uid;
	int score;
}us;

int rank_get_topn(us top[], int topn)
{
	int up = 0;		
	int cnt = 0;
	// 找到上下的边界
	for (int i=10000-1; i>=0; i--) {
		cnt += gia->array[i]->cnt;
		if (cnt >= topn) {
			cnt -= gia->array[i]->cnt;
			up = i;
			break;
		}
	}

	//找到左右的边界
	score_node* sn = gia->array[i]->score_head;
	score_node* right;
	while (sn != NULL) {
		cnt += sn->cnt;
		if (cnt >= topn) {
			right = sn;
		}
	}
}

//--------------------------------------------

score_node* score_new(idx_array* ia, score v)
{}

score_new* score_get(idx_array* ia, score v)
{}

//--------------------------------------------
uid_node* uid_new(hash_tbl* ht, uid v)
{}

uid_node* uid_get(hash_tbl* ht, uid v)
{}
