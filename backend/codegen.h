#pragma once

#include "frontend/ast.h"
#include "frontend/ast_visitor.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <optional>

#include <cstring>
#include <string>
#include <vector>
#include <iostream>

using namespace cs160::frontend;

namespace cs160::backend {

// This is meant to be a generic error, you can use whatever error message you want
struct CodeGenError : public std::runtime_error {
  explicit CodeGenError(const std::string & message) : runtime_error(message) {}
};

class SymbolTable { 
public:
  SymbolTable* currScope = NULL;
  SymbolTable* parent = NULL;
  int num_param = 1;
  int total_var_num = 0;
  int decls = 0;
  //int scopeId;
  int curr_var_num = 0; // num variables in current symbol table
  std::unordered_map<std::string, int> table;
  //default constructor
  SymbolTable(){
  //  this->scopeId = 0;
  }

  void insert(std::string symbol, int info){
    if(info == 1){
      this->curr_var_num ++;
      //insert <symbol, offset>
      this->table.insert(std::pair<std::string, int>(symbol, (this->curr_var_num)*(-4)));
    }
    else {
      this->num_param ++;
      //insert <symbol, offset>
      this->table.insert(std::pair<std::string, int>(symbol, (this->num_param)*(4)));
    }
 
  }

  void createScope() {
    //create a new table
    SymbolTable* currTable;
    
    if (this->currScope == NULL)
      currTable = new SymbolTable();

    else {
      currTable = new SymbolTable();
      currTable->curr_var_num = currScope->decls;
      currTable->decls = currScope->decls;
    }
    //set new table's parent to currScope
    currTable->parent = this->currScope;
    //set currScope to currTable
    this->currScope = currTable;
  }

  int lookUp(std::string symbol) {
    //start from current scope
    SymbolTable* search_area = currScope;
    std::unordered_map<std::string, int> search_table = search_area->table;
    //if not find, search the parent scope
    while (search_area != NULL && (search_table.find(symbol) == search_table.end())) {
      search_area = search_area->parent;
      search_table = search_area->table;
    }
    //if all scope have been searched but can not find the symbol
    if (search_area == NULL)
      return 0;
    else
      //return the offset of the found symbol
      return search_table.at(symbol);
  }

  void endScope(){
    currScope = currScope->parent;
  }

};

// The code generator is implemented as an AST visitor that will generate the relevant pieces of code as it traverses a node
class CodeGen final : public AstVisitor {
public:

  SymbolTable *myTable = new SymbolTable();
  std::vector<std::string> output;
  int cond_num = 0;
  // Entry point of the code generator. This function should visit given program and return generated code as a list of instructions and labels.
  std::vector<std::string> generateCode(const Program & program);

  // Visitor functions
  void VisitIntegerExpr(const IntegerExpr& exp) override;
  void VisitVariableExpr(const VariableExpr& exp) override;
  void VisitAddExpr(const AddExpr& exp) override;
  void VisitSubtractExpr(const SubtractExpr& exp) override;
  void VisitMultiplyExpr(const MultiplyExpr& exp) override;
  void VisitLessThanExpr(const LessThanExpr& exp) override;
  void VisitLessThanEqualToExpr(const LessThanEqualToExpr& exp) override;
  void VisitEqualToExpr(const EqualToExpr& exp) override;
  void VisitLogicalAndExpr(const LogicalAndExpr& exp) override;
  void VisitLogicalOrExpr(const LogicalOrExpr& exp) override;
  void VisitLogicalNotExpr(const LogicalNotExpr& exp) override;
  void VisitIntTypeExpr(const IntType& exp) override;
  void VisitBlockExpr(const BlockExpr& exp) override;
  void VisitDeclarationExpr(const Declaration& exp) override;
  void VisitAssignmentExpr(const Assignment& assignment) override;
  void VisitConditionalExpr(const Conditional& conditional) override;
  void VisitLoopExpr(const Loop& loop) override;
  void VisitFunctionCallExpr(const FunctionCall& call) override;
  void VisitFunctionDefExpr(const FunctionDef& def) override;
  void VisitProgramExpr(const Program& program) override;

  // You can add more public or private members here

};

}
