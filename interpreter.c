#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "symtab.h"
#include "turtle.h"


void prgm(void);

LVAL lval;
int lookahead;

enum { NEG };

/* Expression Data Structures */

typedef struct BinaryExpr{
  
  struct Expr *right;
  struct Expr *left;

} BinaryExpr;

/*
  sym holds IDENT 
  real holds REAL
  b holds Binary Expression arguments
  u holds the argument for a unary operator
*/

typedef struct Expr {
  int op;

  union {
    float real;
    SymTab* sym;
    BinaryExpr b;
    struct Expr* u;
  } e;

} Expr;

/* End Expression Data Structures */

Expr * expr(void);
Expr * bool(void);

/* Statement Data Structures */

typedef struct Assign {
  SymTab *lval; 
  Expr *rval;
} Assign;

typedef struct While {
  Expr *cond; 
  struct Stmt *body;
} While;

typedef struct If_Stmt {

  Expr *cond;
  struct Stmt *then;
  struct Stmt *ifels;
  struct Stmt *els;

} If_Stmt;

/*
  type is the type of Statement
  
  a holds assignment arguments
  w holds while arguments
  e holds the expression for an action statement
  i holds an if statement

  list is the next statement within a block

 */

typedef struct Stmt {
  int type; /* type of statement*/

  union {
    Assign a;
    While w;
    Expr * e;
    If_Stmt i;
  } s;

  struct Stmt *list;

} Stmt;

/* End Statement Data Structures*/

Stmt * stmt();

/* We call this when we get an unexpected token */
void SyntaxError()
{
  fprintf(stderr, "Syntax error on line %d\n", lineno);
  fprintf(stderr, "Dumping Image to stdout and exitting\n");
  turtleDumpImage(stdout);
  exit(2);
}


/* We call the match function to get the next token */
void match(int expected)
{
  if (lookahead == expected)
    {
      lookahead = nextToken(&lval);
      return;
    }
  
  fprintf(stderr, "A matching error has occured ( expected %d, not %d)\n", 
	  expected, lookahead);
  SyntaxError();
  
}



/* Expression Data Structure Constructors */

/*
 * REAL (Constants)
 */

Expr * createConstExpr(float val) {

  Expr * n = (Expr*) malloc(sizeof(Expr));

  n->op = REAL_;
  n->e.real = val;
  
  return n;
}

/*
 * IDENT (Variables)
 */


Expr * createVarExpr(SymTab * s){

  Expr * n = (Expr*) malloc(sizeof(Expr));

  n->op = IDENT_;
  n->e.sym = s;

  return n;

}

/*
 * Binary Expressions ( expr1 +-* / expr2 )
 */
 
Expr * createBinaryExpr(int o, Expr * expr1, Expr * expr2){

  Expr * n = (Expr *)malloc(sizeof(Expr));

  n->op = o;
  n->e.b.left = expr1;
  n->e.b.right = expr2;

  return n;

}

/*
 * Unary expressions ( - expr1), ignored for +
 */

Expr * createUnaryExpr(int o, Expr * expr1){

  Expr * e = (Expr *)malloc(sizeof(Expr));
  
  e->op = o;
  e->e.u = expr1;

  return e;
}

/* End EDSC */




/* factor constructor
 *
 * factor -> (- | +) Factor |  ( expr ) | IDENT | REAL
 */

Expr * factor()
{

  Expr * f;

  switch(lookahead)
    {
    case REAL_:
      f = createConstExpr(lval.f);
      match(REAL_);
      break;


    case IDENT_:
      f = createVarExpr(symLookup(lval.s));
      match(IDENT_);
      break;
      

    case '+':
      match('+');
      f = factor();

      break;

    case '-':
      match('-');
      f = createUnaryExpr(NEG, factor());
      
      break;

    case '(':
      match('(');
      f = expr();
      
      if(lookahead != ')')
	SyntaxError();
      
      match(')');
      
      break;

    default:
      SyntaxError();

    }

  return f;

}

/* Term constructor
 *
 * term -> factor { ( * | / ) factor } 
 */

Expr * term()
{
  Expr * f;
  int op;

  f = factor();

  while(lookahead == '*' || lookahead == '/')
    {
      op = lookahead;
      match(op);
      f = createBinaryExpr(op, f, factor());

    }
  
  return f;

}


/* Expression constructors
 *
 * expr -> term { ( + | - ) term } 
 *
 */


Expr * expr()
{
  Expr *t;
  int op;

  t = term();

  while(lookahead == '+' || lookahead == '-')
    {
      op = lookahead;
      match(lookahead);
      t = createBinaryExpr(op, t, term());
    }

  return t;
}

/*
 * cmp -> expr cmp_op expr
 */

Expr * boolCmp()
{
  Expr * c;
  int op;
  
  c = expr();

  switch(lookahead)
    {
    case '=':
    case '<':
    case '>':
    case NE_:
    case LE_: 
    case GE_: 
    case OR_: 
      op = lookahead;
      match(op);
      c = createBinaryExpr(op, c, expr());
      break;

    }

  return c;
  
}


/*
 * bool_factor -> NOT bool_factor | ( bool ) | cmp
 */
Expr * boolFactor()
{
  Expr * f;
  
  switch(lookahead)
    {
    case NOT_:
      match(NOT_);
      f = createUnaryExpr(NOT_, boolFactor());
      break;

    case '(':
      match('(');
      f = bool();

      if(lookahead != ')')
	SyntaxError();

      match(')');
      break;

    default:
      f = boolCmp();
      
    }


  return f; 
  
}

/* bool_term -> bool_factor {AND bool_factor}
 *
 */

Expr * boolTerm()
{
  Expr  * t;

  int op;
  
  t = boolFactor();

  while(lookahead == AND_)
    {
      op = lookahead;
      match(AND_);
      t = createBinaryExpr(op, t, boolFactor());
    }

  return t;

}


/* Bool constructor
 *
 * bool -> bool_term { OR bool_term }
 *
 */

Expr * bool()
{
  Expr * b;
  int op;

  b = boolTerm();

  while(lookahead == OR_)
    {
      op = lookahead;
      match(OR_);
      b = createBinaryExpr(op, b, boolTerm());
    }

  return b;
}

/* Statement constructors */

/*
 * assign -> IDENT ASSIGN expr
 */

Stmt * createAssignStmt()
{
  Stmt *s = malloc(sizeof(Stmt));

  s->s.a.lval = symLookup(lval.s);

  match(IDENT_);

  if(lookahead != ASSIGN_)
    SyntaxError();
    
  s->type = lookahead;
  match(ASSIGN_);

  s->s.a.rval = expr();

  return s;

}

/*
 * block -> stmt { stmt }
 */

Stmt * createBlockStmt()
{
  Stmt * b;

  switch(lookahead)
    {
    case OD_:
    case ELSIF_:
    case ELSE_:
    case FI_:
      return NULL;
    }


  b = stmt();
  b->list = createBlockStmt();

  return b;

}



/*
 * while -> WHILE bool DO block OD
 */

Stmt* createWhileStmt()
{
  Stmt * w = malloc(sizeof(Stmt));

  w->list = NULL;
  w->type = WHILE_;
  match(WHILE_);

  w->s.w.cond = bool();

  if(lookahead != DO_)
    SyntaxError();

  match(DO_);

  w->s.w.body = createBlockStmt();

  if(lookahead != OD_)
    SyntaxError();

  match(OD_);

    
  

  return w;
}

/*
 *  if_stmt -> IF bool THEN block { ELSIF bool THEN block } [ELSE block] FI

typedef struct If_Stmt {

  Expr *cond;
  struct Stmt *then;
  struct Stmt *ifels;
  struct Stmt *els;

} If_Stmt;

 */

Stmt * createIfStmt()
{
  Stmt * i = (Stmt*)malloc(sizeof(Stmt));
  Stmt * tmp;

  i->s.i.cond = NULL;
  i->s.i.then = NULL;
  i->s.i.ifels = NULL;
  i->s.i.els = NULL;
  i->list = NULL;


  i->type = IF_;
  match(IF_);

  i->s.i.cond = bool();

  if(lookahead != THEN_)
    SyntaxError();

  match(THEN_);

  i->s.i.then = createBlockStmt();

  tmp = i;

  while( lookahead == ELSIF_ )
    {
      match(ELSIF_);
      
      tmp->s.i.ifels = malloc(sizeof(Stmt));
      tmp = tmp->s.i.ifels;

      tmp->type = IF_;
      tmp->s.i.cond = bool();

      if(lookahead != THEN_)
	SyntaxError();

      match(THEN_);

      tmp->s.i.then = createBlockStmt();
      
    }
  
  
  if(lookahead == FI_)
    {
      match(FI_);
      return i;
    }

  if(lookahead != ELSE_)
    SyntaxError();

  
  match(ELSE_);

  i->s.i.els = createBlockStmt();


  if(lookahead != FI_)
    SyntaxError();

  match(FI_);



  return i;
}

/*
 * Action -> HOME | PENUP | PENDOWN | FORWARD expr |
 *           RIGHT expr | LEFT expr | PUSHSTATE | POPSTATE
 */

Stmt * createActionStmt(int t)
{
  Stmt * s = (Stmt*)malloc(sizeof(Stmt));

  s->type = t;

  match(t);

  switch(s->type)
    {
    case FORWARD_:
    case RIGHT_: 
    case LEFT_: 
      s->s.e = expr();
      break;
    default:
      s->s.e = NULL;
    }

  return s;
}

/*
 * stmt -> ASSIGN | WHILE | IF | ACTION
 */

Stmt* stmt()
{
  Stmt *s;

  switch(lookahead)
    {
    case IDENT_:
      return createAssignStmt();

    case WHILE_: 
      return createWhileStmt();

    case IF_:  
      return createIfStmt();
      break;

    case HOME_: 
    case PENUP_: 
    case PENDOWN_:
    case FORWARD_:
    case RIGHT_: 
    case LEFT_:  
    case PUSHSTATE_:
    case POPSTATE_: 
      return createActionStmt(lookahead);


    default:
      SyntaxError();

    }


  return s;
}


/* End constructors */


/* Execution functions, executes statements */

/*
 * evalExpr evalutes Expr e, returns value as float
 */

float evalExpr(Expr * e)
{

  switch(e->op)
    {
    case '+':
      return evalExpr(e->e.b.left) + evalExpr(e->e.b.right);
    case '-':
      return evalExpr(e->e.b.left) - evalExpr(e->e.b.right);
    case '*':
      return evalExpr(e->e.b.left) * evalExpr(e->e.b.right);
    case '/': 
      return evalExpr(e->e.b.left) / evalExpr(e->e.b.right);
    case NEG: 
      return -evalExpr(e->e.u);
    case '=':
      return evalExpr(e->e.b.left) == evalExpr(e->e.b.right) ? 1:0;
    case NE_:
      return evalExpr(e->e.b.left) != evalExpr(e->e.b.right) ? 1:0;
    case '<':
      return evalExpr(e->e.b.left) < evalExpr(e->e.b.right) ? 1:0;
    case '>':
      return evalExpr(e->e.b.left) > evalExpr(e->e.b.right) ? 1:0;
    case LE_:
      return evalExpr(e->e.b.left) <= evalExpr(e->e.b.right) ? 1:0;
    case GE_:     
      return evalExpr(e->e.b.left) >= evalExpr(e->e.b.right) ? 1:0;

    case NOT_:
      return (float)(((int)evalExpr(e->e.u) + 1) % 2 );
    case OR_:
      return evalExpr(e->e.b.left) || evalExpr(e->e.b.right);
    case AND_:
      return evalExpr(e->e.b.left) && evalExpr(e->e.b.right);
	    
    case REAL_:
      return e->e.real;
    case IDENT_:
      return e->e.sym->val;

    default:
      fprintf(stderr, "Expression %d will not evaluate at line %d\n", 
	      e->op, lineno);
      SyntaxError();
    }

  return 0.0;
}

void executeStmt(Stmt * s);

/* helper function to execute a block of code */

void executeBlock(Stmt * s)
{
  while(s)
    {
      executeStmt(s);
      s = s->list;
    }
}

/* Entry point for executing top level statements*/

void executeStmt(Stmt * s)
{

  Stmt * tmp;

  switch(s->type)
    {
    case ASSIGN_: 

      s->s.a.lval->val = evalExpr(s->s.a.rval);
      break;

    case WHILE_:  

      while(evalExpr(s->s.w.cond))
	{
	  tmp = s->s.w.body;
	  executeBlock(tmp);
	}
  
      break;

    case IF_:  

      if(evalExpr(s->s.w.cond))
	{
	  executeBlock(s->s.i.then);
	  
	}
      else
	{
	  if(s->s.i.ifels)
	    executeStmt(s->s.i.ifels);

	  if(s->s.i.els)
	    executeStmt(s->s.i.els);
	}

      break;
 
    case HOME_:  
      turtleHome();
      break;

    case PENUP_:  
      turtlePenUp(1);
      break;
    case PENDOWN_:  
      turtlePenUp(0);
      break;

    case FORWARD_:  
      turtleForward(evalExpr(s->s.e));
      break;

    case RIGHT_:  
      turtleRotate(-evalExpr(s->s.e));
      break;

    case LEFT_:  
      turtleRotate(evalExpr(s->s.e));
      break;

    case PUSHSTATE_:  
      turtlePushState();
      break;

    case POPSTATE_: 
      turtlePopState();
      break;

    default: 
      fprintf(stderr, "unknown-statement, exitting on line %d\n", lineno);
      exit(2);
    }
}


/* End execution functions */


/* Destructors, each one does what the function name says */

void destroyExpr(Expr * e);

void destroyBinaryExpr(Expr * e)
{
  destroyExpr(e->e.b.right);
  destroyExpr(e->e.b.left);

  free(e);
}

void destroyUnaryExpr(Expr * e)
{
  destroyExpr(e->e.u);

  free(e);
}

void destroyExpr(Expr * e)
{
  switch(e->op)
    {
    case '+':
    case '-':
    case '*':
    case '/': 
    case '=':
    case NE_:
    case '<':
    case '>':
    case LE_:
    case GE_:     
    case OR_:
    case AND_:
      destroyBinaryExpr(e);
      return;

    case NEG: 
    case NOT_:
      destroyUnaryExpr(e);
      return;
	    
    case REAL_:
    case IDENT_:
      free(e);
      return;

    default:
      fprintf(stderr, "Expression %d cannot be destroyed at line %d\n", 
	      e->op, lineno);
      exit(2);
    }

  return;
}

void destroyStmt(Stmt * s);

void destroyBlock(Stmt * b)
{
  if(b)
    {
      destroyBlock(b->list);
      free(b);
    }
} 

void destroyAssign(Stmt * a)
{
  destroyExpr(a->s.a.rval);

  free(a);
}

void destroyWhile(Stmt * a)
{
  destroyExpr(a->s.w.cond);
  destroyBlock(a->s.w.body);

  free(a);

}

void destroyIf(Stmt * i)
{
  if(i->s.i.cond)
    destroyExpr(i->s.i.cond);

  if(i->s.i.then)
    destroyBlock(i->s.i.then);

  if(i->s.i.ifels)
    destroyIf(i->s.i.ifels);

  if(i->s.i.els)
    destroyBlock(i->s.i.els);

  free(i);

  return;

}

void destroyAction(Stmt * a)
{
  if(a->s.e)
    destroyExpr(a->s.e);

  free(a);

  return;

}

void destroyStmt(Stmt * s)
{

  switch(s->type)
    {
    case ASSIGN_: 
      destroyAssign(s);
      return;

    case WHILE_:  
      destroyWhile(s);
      return;

    case IF_:  
      destroyIf(s);
      return;
 
    case HOME_:  
    case PENUP_:  
    case PENDOWN_:  
    case PUSHSTATE_:  
    case POPSTATE_: 
    case FORWARD_:  
    case RIGHT_:  
    case LEFT_:  
      destroyAction(s);
      return;

    default: 
      fprintf(stderr, "unknown-statement, exitting on line %d\n", lineno);
      exit(2);
    }
}


/* End destructors */



/* Recursive descent starter functions */

/*
 * stmt_seq -> stmt { stmt }
 *
 * this function creates, executes, and destroys top
 * level statements one at a time
 *
 */

void stmt_seq()
{
  Stmt *s;

  do{
    s = stmt();
    executeStmt(s);
    destroyStmt(s);

  }while( lookahead);
}

/*
 * prgm -> stmt_seq $
 */

void prgm()
{
  turtleInit();
  stmt_seq();

  if (lookahead != 0) fprintf(stderr, "Extraneous input! %d on line %d",
			     lookahead,
			     lineno);


}

/* Entry point of interpretter */

int main(void)
{
  lookahead = nextToken(&lval);

  prgm();

  turtleDumpImage(stdout);

  return 0;

}
