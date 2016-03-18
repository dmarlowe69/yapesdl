#ifndef _TYPES_H
#define _TYPES_H

#if defined(__EMSCRIPTEN__)
#include <SDL2/SDL.h>
#elif defined(_MSC_VER)
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#ifdef __GNUC__ /*__GNUC__*/
typedef unsigned long long ClockCycle;
#define TSTATE_T_MID (((long long) -1LL)/2ULL)
#define TSTATE_T_LEN "Lu"
#else
typedef unsigned __int64 ClockCycle;
#define TSTATE_T_MID (((__int64) -1L)/2UL)
#define TSTATE_T_LEN "lu"
#endif

typedef struct _MEM_PATCH {
	unsigned int addr;
	unsigned char byte;
} mem_patch;

enum RvarTypes {
	RVAR_TOGGLE,
	RVAR_INT,
	RVAR_HEX,
	RVAR_STRING,
	RVAR_STRING_FLIPLIST,
	RVAR_VARIANT
};

typedef struct _RVAR {
	char menuName[32];
	char resourceName[32];
	void (*callback)(void *);
	void *value;
	RvarTypes type;
	const char *(*label)();
} rvar_t;
extern rvar_t *settings[];

// Simple linked list class
template<typename T>
class LinkedList {
public:
	LinkedList() {
		count++;
	}
	~LinkedList() {
		count--;
	}
	static T *getRoot() { return root; }
	T *getNext() { return next; }
	void add(T *ss) {
		if (root) {
			last = last->next = ss;
		} else {
			last = root = ss;
		}
		next = 0;
	}
	void remove(T *ss) {
		T *prevNode;
		T *nextNode;
		T *node = root;
		if (!node)
			return;

	   prevNode = root;
		do {
			nextNode = node->next;
			if (node == ss) {
				if (node == root) {
					root = nextNode;
					if (!root)
						last = 0;
				}
				if (node == last)
					last = prevNode;
				if (prevNode) {
					prevNode->next = nextNode;
					if (nextNode == last)
						last = nextNode;
				}
			}
			prevNode = node;
			node = nextNode;
		} while (node);
	}
   // void cascadeCall()
private:
	T *next;
	static T *root;
	static T *last;
	static unsigned int count;
};

class Resettable : public LinkedList<Resettable> {
public:
	Resettable() {
		add(this);
	}
	~Resettable() {
		remove(this);
	}
	void resetAll(bool hard = false) {
		Resettable *rs = getRoot();
		while (rs) {
			rs->reset(hard);
			rs->getNext();
		}
	}
	virtual void reset(bool hard) = 0;
};

//template<> unsigned int LinkedList<Resettable>::count = 0;
//template<> Resettable* LinkedList<Resettable>::root = 0;
//template<> Resettable* LinkedList<Resettable>::last = 0;

#endif // _TYPES_H
