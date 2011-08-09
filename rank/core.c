#include "typedef.h"
#include <stdlib.h>
#include <map>
using namespace std;

#define IDX_LEN 10000
#define BASE 1000


struct _score_node;
struct _uid_node;

typedef struct _us{
	int uid;
	int score;
}us;

typedef struct _score_node{
	int score;
	int cnt;
	struct _score_node* next_score;
	struct _uid_node* uid_head;
}score_node;

typedef struct _idx_node{
	int cnt;
	struct _score_node* score_head;
}idx_node;

typedef struct _uid_node{
	int uid;
	struct _uid_node* next_uid;
	struct _uid_node* pre_uid;
	struct _score_node* owner_score;
}uid_node;

typedef struct _idx_array{
	struct _idx_node* array;
	int len;
}idx_array;

//typedef _sub_hash{
//}sub_hash;

//typedef _hash_tbl{
//}hash_tbl;

//暂用stl的map
typedef map<int, uid_node*> hash_tbl;

int rank_insert(int uid, int score);
int rank_update(int uid, int score);
int rank_get_rank(int uid);
int rank_get_topn(us top[], int topn);
void _bind_score_uid(score_node* sn, uid_node* un);
void _unbind_score_uid(score_node* sn, uid_node* un);
score_node* score_new(idx_array* ia, int score);
score_node* score_get(idx_array* ia, int score);
uid_node* uid_new(hash_tbl* ht, int uid);
uid_node* uid_get(hash_tbl* ht, int uid);

//global 
void* mem_alloc(size_t size);

void mem_free(void* mem);

//全局变量
idx_array* gia = NULL;//single
hash_tbl* ght = NULL;//single

//--------------------------------------------
//--------------------------------------------
uid_node* uid_new(hash_tbl* ht, int uid)
{
	uid_node* un = (uid_node*)mem_alloc(sizeof(uid_node));
	un->pre_uid = NULL;
	un->next_uid = NULL;
	un->owner_score = NULL;
	un->uid = uid;

	//ht[uid] = un;
	ht->insert(pair<int, uid_node*>(uid, un));
	return un;
}

uid_node* uid_get(hash_tbl* ht, int uid)
{
	map<int, uid_node*>::const_iterator ptr;
	ptr = ht->find(uid);
	return ptr->second;
}
//-----------------------------------------
//数据结构初始化
//-----------------------------------------
int rank_init()
{
	//idx 初始化
	gia = (idx_array*)mem_alloc(sizeof(idx_array));
	gia->array = (idx_node*)mem_alloc(IDX_LEN * sizeof(idx_node));
	gia->len = IDX_LEN;

	//idx 节点初始化
	for (int i=0; i<gia->len; i++) {
		gia->array[i].cnt = 0;
		gia->array[i].score_head = NULL;
	}

	//to do: hash初始化
	ght = new hash_tbl();//global 
}


//-----------------------------------------
//插入uid，score
//-----------------------------------------

int rank_update(int uid, int score)
{
	uid_node* un = uid_get(ght, uid);
	if (un == NULL)
		rank_insert(uid, score);

	score_node* old_sn = (score_node*)un->owner_score;

	if (score == old_sn->score)
		return 0;

	score_node* new_sn = score_get(gia, score);
	if (new_sn == NULL)
		new_sn = score_new(gia, score);

	_unbind_score_uid(old_sn, un);

	_bind_score_uid(new_sn, un);

	return 0;
}
int rank_insert(int uid, int score)
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

void _bind_score_uid(score_node* sn, uid_node* un)
{
	sn->cnt += 1;

	if (sn->uid_head == NULL) {//作为链表头
		sn->uid_head = un;

		un->next_uid = NULL;
		un->pre_uid = NULL;
		un->owner_score = sn;
		return;
	}

	//插入作为链表头
	un->next_uid = sn->uid_head;
	un->pre_uid = NULL;

	sn->uid_head->pre_uid = un;
	sn->uid_head = un;
	return;
}

void _unbind_score_uid(score_node* sn, uid_node* un)
{
	sn->cnt -= 1;

	if (sn->uid_head == un) {//un是链表头
		sn->uid_head = un->next_uid;
		if (un->next_uid != NULL)//不是尾
			un->next_uid->pre_uid = NULL;

		//清理un
		un->pre_uid = NULL;
		un->next_uid = NULL;
		un->owner_score = NULL;
		return;
	}

	un->pre_uid->next_uid = un->next_uid;
	if (un->next_uid != NULL)//不是尾
		un->next_uid->pre_uid = un->pre_uid;

	//清理un
	un->pre_uid = NULL;
	un->next_uid = NULL;
	un->owner_score = NULL;
	return;
}


int rank_get_rank(int uid)
{
	uid_node* un = uid_get(ght, uid);
	if (un == NULL)
		return -1;

	score_node* sn = un->owner_score;
	int score = sn->score;

	int idx = score / 1000;
	int rank = 0;
	for (int i=10000-1; i>idx; i--) {
		rank += gia->array[i].cnt;
	}
	score_node* head = gia->array[idx].score_head;

	score_node* tmp = head;
	while (tmp != sn) {
		rank += tmp->cnt;
		tmp = tmp->next_score;
	}
	return rank;
}


int rank_get_topn(us top[], int topn)
{
	int up = 0;		
	int cnt = 0;
	// 找到上下的边界
	for (int i=10000-1; i>=0; i--) {
		cnt += gia->array[i].cnt;
		if (cnt >= topn) {
			cnt -= gia->array[i].cnt;
			up = i;
			break;
		}
	}

	//找到左右的边界
	score_node* sn = gia->array[up].score_head;
	score_node* right;
	while (sn != NULL) {
		cnt += sn->cnt;
		if (cnt >= topn) {
			right = sn;
		}
	}
}

//--------------------------------------------

score_node* score_new(idx_array* ia, int score)
{
	score_node* sn = score_get(ia, score);
	if (sn != NULL) {
		return sn;
	}
	int idx = score / BASE;
	if (idx >= ia->len) {
		idx = ia->len - 1;
	}

	score_node* cur = ia->array[idx].score_head;//头节点
	score_node* pre = cur;

	//找到比score小的节点
	while (cur != NULL) {
		if (cur->score > score) {
			pre = cur;
			cur = cur->next_score;
		} else {
			break;
		}
	}
	//新的score节点，初始化
	score_node* new_sn = (score_node*)mem_alloc(sizeof(score_node));
	new_sn->score = score;
	new_sn->cnt = 0;
	new_sn->uid_head = NULL;
	new_sn->next_score = NULL;

	if (pre == cur) {//作为头结点
		ia->array[idx].score_head = new_sn;
	} else {
		//插入
		pre->next_score = new_sn;
		new_sn->next_score = cur;
	}

	//更新idx节点
	ia->array[idx].cnt += 1;
	return new_sn;
}

//--------------------------------------------
//if not get, return NULL
//--------------------------------------------
score_node* score_get(idx_array* ia, int score)
{
	int idx = score / BASE;
	if (idx >= ia->len) {
		idx = ia->len - 1;
	}

	score_node* cur = ia->array[idx].score_head;

	//遍历一个有序链表
	while (cur != NULL) {
		if (cur->score > score) {
			cur = cur->next_score;
		} else if (cur->score == score) {
			return cur;
		} else {
			return NULL;
		}
	}
	return NULL;
}

