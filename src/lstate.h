/*
** $Id: lstate.h,v 2.24.1.2 2008/01/03 15:20:39 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/

#ifndef lstate_h
#define lstate_h

#include "lua.h"

#include "lobject.h"
#include "ltm.h"
#include "lzio.h"



struct lua_longjmp;  /* defined in ldo.c */


/* table of globals */
#define gt(L)	(&L->l_gt)

/* registry */
#define registry(L)	(&G(L)->l_registry)


/* extra stack space to handle TM calls and some other extras */
#define EXTRA_STACK   5


#define BASIC_CI_SIZE           8

#define BASIC_STACK_SIZE        (2*LUA_MINSTACK)



typedef struct stringtable {
  GCObject **hash; // ����TString,�ȼ�����hashֵ,����stringtable��hash������ֵ,����λ���Ѿ���Ԫ��,������������;һ��GCObject�ı���������ʵ�Ǹ�TString*����
  lu_int32 nuse;  /* number of elements */ // ���е��ַ�������
  int size;	// ��ϣ���С
} stringtable;


/*
** informations about a call
*/// ������Ϣ
typedef struct CallInfo {
  StkId base;  /* base for this function */
  StkId func;  /* function index in the stack */
  StkId	top;  /* top for this function */
  const Instruction *savedpc;// ���������ж�ʱ��¼��ǰ�հ�ִ�е���pcλ��
  int nresults;  /* expected number of results from this function */// ����ֵ������-1Ϊ���ⷵ�ظ���
  int tailcalls;  /* number of tail calls lost under this entry */// ��¼β���ô�����Ϣ�����ڵ���
} CallInfo;		// ÿ�εݹ���ö�������һ��CallInfo����ȫ��CallInfoջ�Ĵ�С���ޣ�����2���ݣ����������Ϊ16834(2^14<LUAI_MAXCALLS(20000))
// β������ָ��return��ֱ�ӵ��ú���������������������β�������ڱ���ʱ���������ģ��ж����Ĳ�����OP_TAILCALL����������е�ִ�д�����lvm.c 603-634


#define curr_func(L)	(clvalue(L->ci->func))
#define ci_func(ci)	(clvalue((ci)->func))
#define f_isLua(ci)	(!ci_func(ci)->c.isC)
#define isLua(ci)	(ttisfunction((ci)->func) && f_isLua(ci))


/*
** `global state', shared by all threads of this state
*/ // ȫ��״̬��������̹߳��� ��Ҫ������gc��صĶ���
typedef struct global_State {
  stringtable strt;  /* hash table for strings */ // ר�����ڴ���ַ�����ȫ��hash����
  lua_Alloc frealloc;  /* function to reallocate memory */// �ڴ���亯��
  void *ud;         /* auxiliary data to `frealloc' */// frealloc�ĸ�������
  lu_byte currentwhite;
  lu_byte gcstate;  /* state of garbage collector */
  int sweepstrgc;  /* position of sweep in `strt' */
  GCObject *rootgc;  /* list of all collectable objects */
  GCObject **sweepgc;  /* position of sweep in `rootgc' */
  GCObject *gray;  /* list of gray objects */
  GCObject *grayagain;  /* list of objects to be traversed atomically */
  GCObject *weak;  /* list of weak tables (to be cleared) */
  GCObject *tmudata;  /* last element of list of userdata to be GC */
  Mbuffer buff;  /* temporary buffer for string concatentation */
  lu_mem GCthreshold;
  lu_mem totalbytes;  /* number of bytes currently allocated */
  lu_mem estimate;  /* an estimate of number of bytes actually in use */
  lu_mem gcdept;  /* how much GC is `behind schedule' */
  int gcpause;  /* size of pause between successive GCs */
  int gcstepmul;  /* GC `granularity' */
  lua_CFunction panic;  /* to be called in unprotected errors */
  TValue l_registry;	// ��ӦLUAREGISTRYINDEX��ȫ��table
  struct lua_State *mainthread;
  UpVal uvhead;  /* head of double-linked list of all open upvalues */ // ����lua�������������ջ��upvalue�����ͷ
  struct Table *mt[NUM_TAGS];  /* metatables for basic types */// Lua5.0�����ԣ��������͵�Ԫ��
  TString *tmname[TM_N];  /* array with tag-method names */// Ԫ���������ַ�������
} global_State;


/*
** `per thread' state
*/ // �����߳�״̬ typedef TValue *StkId lua_state��ʾһ��lua�����������per-thread�ģ�Ҳ����һ��Э��
struct lua_State {
  CommonHeader;
  // ջ���
  StkId top;  /* first free slot in the stack */// ָ��ǰ�߳�ջ��ջ��ָ�룬ָ��ջ�ϵĵ�һ�����е�slot
  StkId base;  /* base of current function */// ָ��ǰ�������е���Ի�λ�ã��ο��հ�
  StkId stack_last;  /* last free slot in the stack */// ջ��ʵ�����һ��λ�ã�ջ�ĳ����Ƕ�̬�����ģ� ջ�ϵ����һ�����е�slot
  StkId stack;  /* stack base */// ջ�� ����ջ��ջ��

  global_State *l_G;	// ָ��ȫ��״̬��ָ��
  // �������
  lu_byte status;	// �߳̽ű���״̬����Lua.h L42
  CallInfo *ci;  /* call info for current function */// ��ǰ�߳����еĺ���������Ϣ
  const Instruction *savedpc;  /* `savedpc' of current function */// ��������ǰ����¼��һ��������pcλ��
  CallInfo *end_ci;  /* points after end of ci array*/// ָ��������ջ��ջ��
  CallInfo *base_ci;  /* array of CallInfo's */// ָ��������ջ��ջ��
  // ��Ҫ�õ��ĳ��ȡ���С��CǶ��������
  int stacksize;	// ջ�Ĵ�С
  int size_ci;  /* size of array `base_ci' */// ��������ջ�Ĵ�С
  unsigned short nCcalls;  /* number of nested C calls */// ��ǰC�����ĵ��õ����
  unsigned short baseCcalls;  /* nested C calls when resuming coroutine */// ���ڼ�¼ÿ���߳�״̬��C����������ȵĸ�����Ա
  lu_byte hookmask;	// �Ƿ����ĳ�����Ӻ��������룬��Lua.h L310
  lu_byte allowhook;// �Ƿ�����hook
  int basehookcount;// �û����õĳ���LUA_MASKCOUNT��Ӧ�Ĺ��Ӻ�����Ҫִ�е�ָ����
  int hookcount;// �������Ӧ�������е�ָ����
  lua_Hook hook;// �û�ע��Ĺ��Ӻ���
  // ȫ�ֻ������
  TValue l_gt;  /* table of globals */// ��ǰ�߳�ִ�е�ȫ�ֻ�����
  TValue env;  /* temporary place for environments */// ��ǰ���еĻ�����
  // GC���
  GCObject *openupval;  /* list of open upvalues in this stack */ // ��ǰ��ջ�ϵ�����open��uvalue
  GCObject *gclist;	// ����gc
  // ���������
  struct lua_longjmp *errorJmp;  /* current error recover point */// ��������ʱ�ĳ���תλ��
  ptrdiff_t errfunc;  /* current error handling function (stack index) */// �û�ע��Ĵ���ص�����
};


#define G(L)	(L->l_G)


/*
** Union of all collectable objects
*/
union GCObject {// ÿ��GCObject�ṹ�嶼��һ��CommonHeaderλ���ʼ����
  GCheader gch;
  union TString ts;
  union Udata u;
  union Closure cl;
  struct Table h;
  struct Proto p;
  struct UpVal uv;	// upvalueҲ�ǿ�GC��
  struct lua_State th;  /* thread */
}; // �ⲿ�ֻ������ݵ�ʱ��,���ݲ���(��gch��)����,�������gc������(gch) 


/* macros to convert a GCObject into a specific value */
#define rawgco2ts(o)	check_exp((o)->gch.tt == LUA_TSTRING, &((o)->ts))
#define gco2ts(o)	(&rawgco2ts(o)->tsv)
#define rawgco2u(o)	check_exp((o)->gch.tt == LUA_TUSERDATA, &((o)->u))
#define gco2u(o)	(&rawgco2u(o)->uv)
#define gco2cl(o)	check_exp((o)->gch.tt == LUA_TFUNCTION, &((o)->cl))
#define gco2h(o)	check_exp((o)->gch.tt == LUA_TTABLE, &((o)->h))
#define gco2p(o)	check_exp((o)->gch.tt == LUA_TPROTO, &((o)->p))
#define gco2uv(o)	check_exp((o)->gch.tt == LUA_TUPVAL, &((o)->uv))
#define ngcotouv(o) \
	check_exp((o) == NULL || (o)->gch.tt == LUA_TUPVAL, &((o)->uv))
#define gco2th(o)	check_exp((o)->gch.tt == LUA_TTHREAD, &((o)->th))

/* macro to convert any Lua object into a GCObject */
#define obj2gco(v)	(cast(GCObject *, (v)))


LUAI_FUNC lua_State *luaE_newthread (lua_State *L);
LUAI_FUNC void luaE_freethread (lua_State *L, lua_State *L1);

#endif

