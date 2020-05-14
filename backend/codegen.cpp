#include "backend/codegen.h"

#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

namespace cs160::backend {

// Code generator methods


std::vector<std::string> CodeGen::generateCode(const Program & program) {
  // The main entry point of the code generator. This method should
  // clean up the data structures, visit given AST, and return the
  // generated list of instructions.

  program.Visit(this);
  return this->output;
}

void CodeGen::VisitIntegerExpr(const IntegerExpr& exp) {
  std::string emit;
  emit = " movl $" + std::to_string(exp.value()) + ", %eax";
  this->output.push_back(emit);
}

void CodeGen::VisitVariableExpr(const VariableExpr& exp) {
  std::string emit;
  //offset
  int info = this->myTable->lookUp(exp.name());
  emit = " movl " + std::to_string(info) + "(%ebp), %eax";
  this->output.push_back(emit);
}

void CodeGen::VisitAddExpr(const AddExpr& exp) {
  int tmp = this->myTable->currScope->curr_var_num;
  std::string tmpSymbol = "_temp" + std::to_string(this->myTable->currScope->curr_var_num);
  this->myTable->currScope->insert(tmpSymbol, 1);
  this->output.push_back(" sub $4, %esp");
  //left
  exp.lhs().Visit(this); 
  //offset
  int info = this->myTable->lookUp(tmpSymbol);
  std::string emit = " movl %eax, " + std::to_string(info) + "(%ebp)";
  this->output.push_back(emit);
  this->myTable->currScope->curr_var_num = tmp + 1;
  //right
  exp.rhs().Visit(this); 
  //offset
  info = this->myTable->lookUp(tmpSymbol);
  emit = " movl " + std::to_string(info) + "(%ebp), %edx";
  this->output.push_back(emit);

  this->output.push_back(" add %edx, %eax");
  this->output.push_back(" add $4, %esp");
  this->myTable->currScope->curr_var_num --;
}


void CodeGen::VisitSubtractExpr(const SubtractExpr& exp) {
  int tmp = this->myTable->currScope->curr_var_num;
  std::string tmpSymbol = "_temp" + std::to_string(this->myTable->currScope->curr_var_num);
  this->myTable->currScope->insert(tmpSymbol, 1);
  this->output.push_back(" sub $4, %esp");
  //left
  exp.lhs().Visit(this); 
  //offset
  int info = this->myTable->lookUp(tmpSymbol);
  std::string emit = " movl %eax, " + std::to_string(info) + "(%ebp)";
  this->output.push_back(emit);
  this->myTable->currScope->curr_var_num = tmp + 1;
  //right
  exp.rhs().Visit(this); 
  //offset
  info = this->myTable->lookUp(tmpSymbol);
  emit = " movl " + std::to_string(info) + "(%ebp), %edx";
  this->output.push_back(emit);

  this->output.push_back(" sub %eax, %edx");
  this->output.push_back(" movl %edx, %eax");
  this->output.push_back(" add $4, %esp");
  this->myTable->currScope->curr_var_num --;
}

void CodeGen::VisitMultiplyExpr(const MultiplyExpr& exp) {
  int tmp = this->myTable->currScope->curr_var_num;
  std::string tmpSymbol = "_temp" + std::to_string(this->myTable->currScope->curr_var_num);
  this->myTable->currScope->insert(tmpSymbol, 1);
  this->output.push_back(" sub $4, %esp");
  //left
  exp.lhs().Visit(this); 
  //offset
  int info = this->myTable->lookUp(tmpSymbol);
  std::string emit = " movl %eax, " + std::to_string(info) + "(%ebp)";
  this->output.push_back(emit);
  this->myTable->currScope->curr_var_num = tmp + 1;
  //right
  exp.rhs().Visit(this); 
  //offset
  info = this->myTable->lookUp(tmpSymbol);
  emit = " movl " + std::to_string(info) + "(%ebp), %edx";
  this->output.push_back(emit);

  this->output.push_back(" imul %edx, %eax");
  this->output.push_back(" add $4, %esp");
  this->myTable->currScope->curr_var_num --;
}

void CodeGen::VisitLessThanExpr(const LessThanExpr& exp) {
  int tmp = this->myTable->currScope->curr_var_num;
  std::string tmpSymbol = "_temp" + std::to_string(this->myTable->currScope->curr_var_num);
  this->myTable->currScope->insert(tmpSymbol, 1);
  this->output.push_back(" sub $4, %esp");
  //left
  exp.lhs().Visit(this); 
  //offset
  int info = this->myTable->lookUp(tmpSymbol);
  std::string emit = " movl %eax, " + std::to_string(info) + "(%ebp)";
  this->output.push_back(emit);
  this->myTable->currScope->curr_var_num = tmp + 1;
  //right
  exp.rhs().Visit(this); 
  //offset
  info = this->myTable->lookUp(tmpSymbol);
  emit = " movl " + std::to_string(info) + "(%ebp), %edx";
  this->output.push_back(emit);

  this->output.push_back(" cmp %eax, %edx");
  this->output.push_back(" setl %al");
  this->output.push_back(" movzbl %al, %eax");
  this->output.push_back(" add $4, %esp");
  this->myTable->currScope->curr_var_num --;
}

void CodeGen::VisitLessThanEqualToExpr(const LessThanEqualToExpr& exp) {
  std::string tmpSymbol = "_temp" + std::to_string(this->myTable->currScope->curr_var_num);
  this->myTable->currScope->insert(tmpSymbol, 1);
  this->output.push_back(" sub $4, %esp");
  //left
  exp.lhs().Visit(this); 
  //offset
  int info = this->myTable->lookUp(tmpSymbol);
  std::string emit = " movl %eax, " + std::to_string(info) + "(%ebp)";
  this->output.push_back(emit);
  //this->myTable->currScope->curr_var_num ++;
  //right
  exp.rhs().Visit(this); 
  //offset
  info = this->myTable->lookUp(tmpSymbol);
  emit = " movl " + std::to_string(info) + "(%ebp), %edx";
  this->output.push_back(emit);

  this->output.push_back(" cmp %eax, %edx");
  this->output.push_back(" setle %al");
  this->output.push_back(" movzbl %al, %eax");
  this->output.push_back(" add $4, %esp");
  this->myTable->currScope->curr_var_num --;
}

void CodeGen::VisitEqualToExpr(const EqualToExpr& exp) {
  std::string tmpSymbol = "_temp" + std::to_string(this->myTable->currScope->curr_var_num);
  this->myTable->currScope->insert(tmpSymbol, 1);
  this->output.push_back(" sub $4, %esp");
  //left
  exp.lhs().Visit(this); 
  //offset
  int info = this->myTable->lookUp(tmpSymbol);
  std::string emit = " movl %eax, " + std::to_string(info) + "(%ebp)";
  this->output.push_back(emit);
  //this->myTable->currScope->curr_var_num ++;
  //right
  exp.rhs().Visit(this); 
  //offset
  info = this->myTable->lookUp(tmpSymbol);
  emit = " movl " + std::to_string(info) + "(%ebp), %edx";
  this->output.push_back(emit);

  this->output.push_back(" cmp %eax, %edx");
  this->output.push_back(" sete %al");
  this->output.push_back(" movzbl %al, %eax");
  this->output.push_back(" add $4, %esp");
  this->myTable->currScope->curr_var_num --;
}

void CodeGen::VisitLogicalAndExpr(const LogicalAndExpr& exp) {
  int tmp = this->myTable->currScope->curr_var_num;
  std::string tmpSymbol = "_temp" + std::to_string(this->myTable->currScope->curr_var_num);
  this->myTable->currScope->insert(tmpSymbol, 1);
  this->output.push_back(" sub $4, %esp");
  //left
  exp.lhs().Visit(this); 
  //offset
  int info = this->myTable->lookUp(tmpSymbol);
  std::string emit = " movl %eax, " + std::to_string(info) + "(%ebp)";
  this->output.push_back(emit);
  this->myTable->currScope->curr_var_num = tmp + 1;
  //right
  exp.rhs().Visit(this); 
  //offset
  info = this->myTable->lookUp(tmpSymbol);
  emit = " movl " + std::to_string(info) + "(%ebp), %edx";
  this->output.push_back(emit);

  this->output.push_back(" andl %edx, %eax");
  this->output.push_back(" add $4, %esp");
  this->myTable->currScope->curr_var_num --;
}

void CodeGen::VisitLogicalOrExpr(const LogicalOrExpr& exp) {
  int tmp = this->myTable->currScope->curr_var_num;
  std::string tmpSymbol = "_temp" + std::to_string(this->myTable->currScope->curr_var_num);
  this->myTable->currScope->insert(tmpSymbol, 1);
  this->output.push_back(" sub $4, %esp");
  //left
  exp.lhs().Visit(this); 
  //offset
  int info = this->myTable->lookUp(tmpSymbol);
  std::string emit = " movl %eax, " + std::to_string(info) + "(%ebp)";
  this->output.push_back(emit);
  this->myTable->currScope->curr_var_num = tmp + 1;
  //right
  exp.rhs().Visit(this); 
  //offset
  info = this->myTable->lookUp(tmpSymbol);
  emit = " movl " + std::to_string(info) + "(%ebp), %edx";
  this->output.push_back(emit);

  this->output.push_back(" orl %edx, %eax");
  this->output.push_back(" add $4, %esp");
  this->myTable->currScope->curr_var_num --;
}

void CodeGen::VisitLogicalNotExpr(const LogicalNotExpr& exp) {
  exp.operand().Visit(this);
  this->output.push_back(" cmp $0, %eax");
  this->output.push_back(" sete %al");
  this->output.push_back(" movzbl %al, %eax");
}

void CodeGen::VisitIntTypeExpr(const IntType& exp) {
  std::string emit;
  int info = this->myTable->lookUp(exp.value());
  emit = " movl $0, " + std::to_string(info) + "(%ebp)"; 
  this->output.push_back(emit);
}

void CodeGen::VisitBlockExpr(const BlockExpr& exp) {
  std::string emit;
  int info = (exp.decls().size())*4;
  emit = " sub $" + std::to_string(info) + ", %esp";
  this->output.push_back(emit);

  int i = 0;
  int j = 0;

  while (i < (info/4)){
    exp.decls().at(i)->Visit(this);
    i ++;
  }
  int num_stmts = exp.stmts().size();
  while (j < num_stmts){
    exp.stmts().at(j)->Visit(this);
    j ++;
  }
  this->myTable->currScope->total_var_num = (info/4);
}

void CodeGen::VisitDeclarationExpr(const Declaration& exp) {
  std::string emit;
  this->myTable->currScope->insert(exp.id().name(), 1);
  this->myTable->currScope->decls ++;
  int info = this->myTable->lookUp(exp.id().name());
  emit = " movl $0, " + std::to_string(info) + "(%ebp)";
  this->output.push_back(emit); 
}

void CodeGen::VisitAssignmentExpr(const Assignment& assignment) {
  std::string emit;
  int info = this->myTable->lookUp(assignment.lhs().name());
  assignment.rhs().Visit(this);
  emit = " movl %eax, " + std::to_string(info) + "(%ebp)";
  this->output.push_back(emit);
}

void CodeGen::VisitConditionalExpr(const Conditional& conditional) {
  //guard
  conditional.guard().Visit(this);
  this->output.push_back(" cmp $0, %eax");
  std::string if_false = "IF_FALSE_" + std::to_string(this->cond_num);
  std::string if_end = "IF_END_" + std::to_string(this->cond_num);
  this->output.push_back(" je " + if_false);
  this->cond_num ++;

  //if true
  std::string emit;
  this->myTable->createScope();
  conditional.true_branch().Visit(this);
  int info = (this->myTable->currScope->total_var_num) * 4;
  emit = " add $" + std::to_string(info) + ", %esp";
  this->output.push_back(emit);
  this->myTable->endScope();
  this->output.push_back(" jmp " + if_end);
  this->output.push_back(if_false + ":");

  //if false
  this->myTable->createScope();
  conditional.false_branch().Visit(this);
  info = (this->myTable->currScope->total_var_num) * 4;
  emit = " add $" + std::to_string(info) + ", %esp";
  this->output.push_back(emit);
  this->myTable->endScope();
  this->output.push_back(if_end + ":");

}

void CodeGen::VisitLoopExpr(const Loop& loop) {
  std::string while_start = "WHILE_START_"+std::to_string(this->cond_num);
  this->output.push_back(while_start + ":");
  int tmp_cond_num = this->cond_num;
  this->cond_num ++;

  //guard
  loop.guard().Visit(this);
  this->output.push_back(" cmp $0, %eax");
  std::string while_end = "WHILE_END_" + std::to_string(tmp_cond_num);
  this->output.push_back(" je " + while_end);

  //body
  std::string emit;
  this->myTable->createScope();
  loop.body().Visit(this);
  int info = (this->myTable->currScope->total_var_num) * 4;
  emit = " add $" + std::to_string(info) + ", %esp";
  this->output.push_back(emit);
  this->myTable->endScope();
  this->output.push_back(" jmp " + while_start);

  //exit
  this->output.push_back(while_end + ":");
}

void CodeGen::VisitFunctionCallExpr(const FunctionCall& call) {
  std::string emit;
  std::string name = call.callee_name();
  int tmp_num = call.arguments().size();
  int i = tmp_num - 1;
  while (i >= 0){
    call.arguments().at(i)->Visit(this);
    this->output.push_back(" push %eax");
    i --;
  }
  this->output.push_back(" call " + name);
  int info = tmp_num * 4;
  emit = " add $" + std::to_string(info) + ", %esp";
  this->output.push_back(emit);
}

void CodeGen::VisitFunctionDefExpr(const FunctionDef& def) {
  std::string emit;
  this->myTable->createScope();

  this->output.push_back(def.function_name() + ":");
  this->output.push_back(" push %ebp");
  this->output.push_back(" movl %esp, %ebp");

  int i = 0;
  while(i < def.parameters().size()){
    std::string tmpSymbol = def.parameters().at(i).second->name();
    this->myTable->currScope->insert(tmpSymbol, 2);
    i++;
  }

  this->myTable->createScope();
  def.function_body().Visit(this);
  int tmp_var_num = this->myTable->currScope->total_var_num;
  def.retval().Visit(this);

  tmp_var_num = this->myTable->currScope->total_var_num;
  int info = tmp_var_num * 4;
  emit = " add $" + std::to_string(info) + ", %esp";
  this->output.push_back(emit);
  this->myTable->currScope->curr_var_num -= tmp_var_num;
  this->myTable->currScope->decls -= tmp_var_num;
  this->myTable->endScope();

  this->output.push_back(" movl %ebp, %esp");
  this->output.push_back(" pop %ebp");
  this->output.push_back(" ret");
}

void CodeGen::VisitProgramExpr(const Program& program) {
  std::string emit;
  int i = 0;
  while (i < program.function_defs().size()){
    program.function_defs().at(i)->Visit(this);
    i++;
  }

  this->myTable->createScope();
  
  this->output.push_back(" .globl Entry");
  this->output.push_back(" .type Entry, @function");
  this->output.push_back("Entry:");
  this->output.push_back(" push %ebp");
  this->output.push_back(" movl %esp, %ebp");

  program.statements().Visit(this);
  program.arithmetic_exp().Visit(this);

  int info = (this->myTable->currScope->total_var_num) * 4;
  emit = " add $" + std::to_string(info) + ", %esp";
  this->output.push_back(emit);

  this->output.push_back(" movl %ebp, %esp");
  this->output.push_back(" pop %ebp");
  this->output.push_back(" ret");
  this->myTable->endScope();

}

}  // namespace cs160::backend
