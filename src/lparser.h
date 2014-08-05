/*
** $Id: lparser.h,v 1.57.1.1 2007/12/27 13:02:25 roberto Exp $
** Lua Parser �﷨����
** See Copyright Notice in lua.h
*/

#ifndef lparser_h
#define lparser_h

#include "llimits.h"
#include "lobject.h"
#include "lzio.h"


/*
** Expression descriptor
*/
// ���ʽ����
typedef enum {
  VVOID,	/* no value */
  VNIL,
  VTRUE,
  VFALSE,
  VK,		/* info = index of constant in `k' */// k�ڳ���������
  VKNUM,	/* nval = numerical value */// ��ֵ
  VLOCAL,	/* info = local register */// �ֲ��Ĵ���
  VUPVAL,       /* info = index of upvalue in `upvalues' */// upvalues�ϵ�����
  VGLOBAL,	/* info = index of table; aux = index of global name in `k' */// info = ������; aux = k��ȫ�����Ƶ�����
  VINDEXED,	/* info = table register; aux = index register (or `k') */// info = ��Ĵ���; aux = �����Ĵ�������k��
  VJMP,		/* info = instruction pc */// ָ��pc
  VRELOCABLE,	/* info = instruction pc */
  VNONRELOC,	/* info = result register */// ����Ĵ���
  VCALL,	/* info = instruction pc */
  VVARARG	/* info = instruction pc */
} expkind;
// ���ʽ����
typedef struct expdesc {
  expkind k;
  union {
    struct { int info, aux; } s; // info��ʾ����Ӧ������(��ָ������code(proto�ṹ��)��)����˵��ʾΪ��Ӧconstant������(proto��TValue *k������������)
    lua_Number nval;
  } u;
  int t;  /* patch list of `exit when true' */// ��ʱ�˳��Ĳ����б�
  int f;  /* patch list of `exit when false' */// ��ʱ�˳��Ĳ����б�
} expdesc;


typedef struct upvaldesc {
  lu_byte k;
  lu_byte info;
} upvaldesc;


struct BlockCnt;  /* defined in lparser.c */


/* state needed to generate code for a given function */
typedef struct FuncState {	// �������ĺ�����״̬
  Proto *f;  /* current function header */// ��ǰ����ͷ
  Table *h;  /* table to find (and reuse) elements in `k' */// �������ң������ã�k��Ԫ�صı�
  struct FuncState *prev;  /* enclosing function */// �պϺ���
  struct LexState *ls;  /* lexical state */// �ʷ�����״̬��
  struct lua_State *L;  /* copy of the Lua state */// Lua״̬�ĸ���
  struct BlockCnt *bl;  /* chain of current blocks */// ��ǰ����
  int pc;  /* next position to code (equivalent to `ncode') */// �������һ��λ�ã��ȼ���ncode��
  int lasttarget;   /* `pc' of last `jump target' */// ��һ��jump target��pc
  int jpc;  /* list of pending jumps to `pc' */// ������ת��pc�б�
  int freereg;  /* first free register */// ��һ�����еļĴ���
  int nk;  /* number of elements in `k' */// k�ڵ�Ԫ�ظ���
  int np;  /* number of elements in `p' */// p�ڵ�Ԫ�ظ���
  short nlocvars;  /* number of elements in `locvars' */// locvars�ڵ�Ԫ�ظ���
  lu_byte nactvar;  /* number of active local variables */// ����ľֲ������ĸ���
  upvaldesc upvalues[LUAI_MAXUPVALUES];  /* upvalues */
  unsigned short actvar[LUAI_MAXVARS];  /* declared-variable stack */// �����������Ķ�ջ
} FuncState;


LUAI_FUNC Proto *luaY_parser (lua_State *L, ZIO *z, Mbuffer *buff,
                                            const char *name);


#endif
