#ifndef _CORE_H
#define _CORE_H

#include <map>
using namespace std;

typedef struct _us {
	int uid;
	int score;
} us;

typedef struct _score_node {
	int score;
	int cnt;
	struct _score_node* next_score;
	struct _uid_node* uid_head;
} score_node;

typedef struct _idx_node {
	int cnt;
	struct _score_node* score_head;
} idx_node;

typedef struct _uid_node {
	int uid;
	struct _uid_node* next_uid;
	struct _uid_node* pre_uid;
	struct _score_node* owner_score;
} uid_node;

typedef struct _idx_array {
	struct _idx_node* array;
	int len;
} idx_array;

//typedef _sub_hash{
//}sub_hash;

//typedef _hash_tbl{
//}hash_tbl;

//暂用stl的map
typedef map<int, uid_node*> hash_tbl;


//---------------------------
//外部接口
//---------------------------
int rank_insert(int uid, int score);
int rank_update(int uid, int score);
int rank_get_rank(int uid);
int rank_get_topn(us top[], int topn);

//---------------------------
//static interface
//---------------------------
static void bind_score_uid(score_node* sn, uid_node* un);
static void unbind_score_uid(score_node* sn, uid_node* un);
static score_node* score_new(idx_array* ia, int score);
static score_node* score_get(idx_array* ia, int score);
static uid_node* uid_new(hash_tbl* ht, int uid);
static uid_node* uid_get(hash_tbl* ht, int uid);

#endif
