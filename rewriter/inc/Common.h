#ifndef COMMON_H
#define COMMON_H

#ifndef STD_LIB
#define STD_LIB
#include <string>
#include <vector>
#include <cstdio>
#include <memory>
#include <string>
#include <sstream>
#include <cstring>
#include <list>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ftw.h>
#endif //STD_LIB

#ifndef CLANG_API
#define CLANG_API
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ParentMap.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#endif //CLANG_API

#ifndef USING_NAMESPACE
#define USING_NAMESPACE
using namespace clang;
using namespace std;
#endif //USING_NAMESPACE

#endif //COMMON_H
