#include <time.h>
#include <iostream>
using namespace std;

typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char byte;

// -----------------
// MemPooll
// -----------------
struct MemBlock {
	// 没有记录_unitSize，只是开辟内存时传入
	ushort _size;
	ushort _free;
	ushort _first;
	MemBlock* _pNext;
	byte _data[1];// 调用new之后，_data即为内存块开始的地址，若用byte* _data就需要人为去初始化_data，不如这样方便

	static void* operator new(size_t, ushort unitCnt, ushort unitSize) {
		return ::operator new(sizeof(MemBlock) + unitCnt * unitSize);
	}

	static void operator delete(void* p, size_t) {
		::operator delete(p);
	}

	MemBlock(ushort unitCnt=1, ushort unitSize=0);
	~MemBlock() {}
};

MemBlock::MemBlock(ushort unitCnt, ushort unitSize)
{
	// 生成MemBlock对象时，默认将第一个单元块分配出去，所以此时的_free = unitCnt - 1, _first = 1
	_size = unitCnt * unitSize;
	_free = unitCnt - 1;
	_first = 1;
	_pNext = 0;
	byte* pData = _data;

	for (ushort i=1; i<unitCnt; i++) {
		*reinterpret_cast<ushort*>(pData) = i;
		pData += unitSize;
	}
}


const ushort MEMPOOL_ALIGNMENT = 2;
class MemPool
{
private:
	MemBlock* _pBlock;
	ushort _unitSize;
	ushort _initSize;
	ushort _growSize;
public:
	MemPool(ushort unitSize, ushort initSize=1024, ushort growSize=1024);
	~MemPool();
	void* allocMem();
	void freeMem(void* pFree);
};

MemPool::MemPool(ushort unitSize, ushort initSize, ushort growSize)
{
	_pBlock = NULL;// 构造函数时并不真正分配内存
	_initSize = initSize;
	_growSize = growSize;

	if (unitSize > 4) {
		_unitSize = (_unitSize + (MEMPOOL_ALIGNMENT-1)) & ~(MEMPOOL_ALIGNMENT-1);
	} else if (unitSize <= 2) {
		_unitSize = 2;
	} else {
		_unitSize = 4;
	}
}

void* MemPool::allocMem()
{
	if (_pBlock == NULL) {
		_pBlock = new(_initSize, _unitSize)MemBlock(_initSize, _unitSize);

		if (_pBlock == NULL) {
			return NULL;
		}

		return (void*)_pBlock->_data;
	}

	MemBlock* pmb = _pBlock;

	while (pmb && !pmb->_free) {
		pmb = pmb->_pNext;
	}

	if (pmb != NULL) {
		byte* pFree = pmb->_data + (pmb->_first * _unitSize);
		pmb->_first = *((ushort*)pFree);
		pmb->_free--;
		return (void*)pFree;
	} else {
		if (_growSize == 0) {
			return NULL;
		}

		pmb = new(_growSize, _unitSize)MemBlock(_growSize, _unitSize);

		if (pmb == NULL) {
			return NULL;
		}

		pmb->_pNext = _pBlock;
		_pBlock = pmb;
		return (void*)(pmb->_data);
	}
}

void MemPool::freeMem(void* pFree)
{
	MemBlock* pmb = _pBlock;
	MemBlock* pmb_pre = _pBlock;

	if (pmb == NULL) {
		return;
	}

	while (((ulong)pmb->_data > (ulong)pFree) || ((ulong)pFree >= ((ulong)pmb->_data + pmb->_size))) {
		pmb_pre = pmb;
		pmb = pmb->_pNext;

		if (pmb == NULL) {
			return;
		}
	}

	pmb->_free++;
	*((ushort*)pFree) = pmb->_first;
	pmb->_first = (ushort)(((ulong)pFree - (ulong)pmb->_data)/_unitSize);

	// 让释放的这个block尽可能靠尽头结点，以保证下次使用时最快找到空闲的block
	if (pmb->_free * _unitSize == pmb->_size) {// 本块全部为空
		if (pmb != _pBlock) {// 不在链表头时，则返还给系统堆
			pmb_pre->_pNext = pmb->_pNext;
			delete pmb;
		}
	} else {// 本块不是全部为空
		if (pmb != _pBlock) {// 不在链表头时，移动到链表头
			pmb_pre->_pNext = pmb->_pNext;
			pmb->_pNext = _pBlock->_pNext;
			_pBlock = pmb;
		}
	}
}

MemPool::~MemPool()
{
	MemBlock* pmb = _pBlock;
	MemBlock* pmb_next;

	while (pmb) {
		pmb_next = pmb->_pNext;
		delete pmb;
		pmb = pmb_next;
	}
}

// -----------------
// for test MemPool
// -----------------
class Node
{
private:
	int _a;
public:
	static MemPool* _pmp;
public:
	static int initMemPool() {
		_pmp = new MemPool(sizeof(Node), 1024*2*2*2, 512*2*2*2);
	}

	Node(int aa) {
		_a = aa;
	}

	~Node() {
		_a = 0;
	}

	static void* operator new(size_t) {
		return (void*)_pmp->allocMem();
	}

	static void operator delete(void* p, size_t) { // 没有size_t参数，效率会降低
		return _pmp->freeMem(p);
	}
};
MemPool* Node::_pmp = NULL;

class Node2
{
private:
	int _a;
public:
	Node2(int aa) {
		_a = aa;
	}
};

const int len = 100000;
int main()
{
	Node::initMemPool();
	Node** pp = new Node*[len];
	Node2** pp2 = new Node2*[len];

	// use pool
	clock_t start = clock();

	for (int i=0; i<len; i++) {
		pp[i] = new Node(1);
	}

	clock_t end = clock();
	cout<<"new time: "<<end-start<<endl;
	start = clock();

	for (int i=0; i<len; i++) {
		delete pp[i];
	}

	end = clock();
	cout<<"delete time: "<<end-start<<endl;

	// not use pool
	start = clock();

	for (int i=0; i<len; i++) {
		pp2[i] = new Node2(1);
	}

	end = clock();
	cout<<"new time: "<<end-start<<endl;
	start = clock();

	for (int i=0; i<len; i++) {
		delete pp2[i];
	}

	end = clock();
	cout<<"delete time: "<<end-start<<endl;

	return 0;
}
