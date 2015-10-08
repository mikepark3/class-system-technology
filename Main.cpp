#include <Common.h>

#define SIZE 256

// Structure decription
struct VarInfo
{
    string name;
    string type;
    string scope;
    unsigned int lineNumber;
    unsigned int colNumber;
    bool isSame = false;
};

bool VarInfoCmp(const VarInfo &a, const VarInfo &b)
{
    if(a.name.compare(b.name) < 0)
        return true;
    return false;
}

// Global variables
enum stateType
{
	LISTING1,
	LISTING2,
	SPIN,
	EXTERN,
	RENAME
};
int processState;
SourceLocation spinAcqLoc = SourceLocation(), spinRelLoc = SourceLocation();
vector<VarInfo> list1;
vector<VarInfo> list2;

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> 
{
    public:
	    // Constructor
        MyASTVisitor(Rewriter& R)
	        : reWriter(R), SM(R.getSourceMgr())
	    {
	    }

	    // Getter
        SourceManager& getSourceMgr()
        {
            return this->SM;
        }
	    // Non-override methods
        bool myVisitStmt(Stmt *s, ParentMap &PM) 
        {    
	    if(s == NULL)
	    {
	    	return true;
	    }
            if(isa<DeclRefExpr>(*s) && (processState == SPIN || processState == EXTERN || processState == RENAME))
            {
		DeclRefExpr *dre = cast<DeclRefExpr>(s);
		DeclarationNameInfo dni = dre->getNameInfo();
		string varName = dni.getName().getAsString();
		
		Stmt *chunk = s;
		while(PM.hasParent(chunk))	
		{
		//	cout<<chunk->getStmtClassName()<<endl;
			Stmt *parent = PM.getParent(chunk);
			if(isa<CompoundStmt>(parent))
			{
				break;
			}
			else
			{
				chunk = parent;
			}
		}
		//cout<<varName<<" chunk"<<endl;
		//cout<<"start("<<SM.getExpansionLineNumber(chunk->getLocStart())<<":"<<SM.getExpansionColumnNumber(chunk->getLocStart())<<") ~ "; 
		//cout<<"end("<<SM.getExpansionLineNumber(chunk->getLocEnd())<<":"<<SM.getExpansionColumnNumber(chunk->getLocEnd())<<")"<<endl; 

		// Case of rewriting
		for(vector<VarInfo>::iterator it=list1.begin(); it != list1.end(); ++it)
		{
			if(varName == it->name && it->isSame == true)
			{
				if(processState == SPIN)
				{
					//if나 return stmt에 걸리는 spinlock 포인트는 dummy변수 쓰는 것을 추천해줄것
					SourceLocation spinAcqLocPerDRE, spinRelLocPerDRE;

					spinAcqLocPerDRE = chunk->getLocStart();
					// spin acquire point needs to set line -1
					spinRelLocPerDRE = chunk->getLocEnd();
					string buffer = SM.getCharacterData(spinRelLocPerDRE);
					int offset = buffer.find_first_of('\n');
					spinRelLocPerDRE = spinRelLocPerDRE.getLocWithOffset(offset);
					// spin release point needs to set line +1, column = acquire column value

					/*
					string buffer = SM.getCharacterData(s->getLocStart());
					int offset = buffer.find_first_of(';')+1;
					spinAcqLocPerDRE = dre->getLocStart();	// Acquire point also need to offset until previous semi-colon 
					spinRelLocPerDRE = dre->getLocStart().getLocWithOffset(offset);
					*/

					// Case of first to visit DeclRefExpr
					if((spinAcqLoc == SourceLocation()) && (spinRelLoc == SourceLocation()))
					{
						spinAcqLoc = spinAcqLocPerDRE;
						spinRelLoc = spinRelLocPerDRE;
					}
					else if((spinAcqLoc != spinAcqLocPerDRE) && (spinRelLoc != spinRelLocPerDRE))
					{
						reWriter.InsertText(spinAcqLoc, "Spin_lock_Acquire();\n", true, true);
						reWriter.InsertText(spinRelLoc, "\nSpin_lock_Release();\n", true, true);
						spinAcqLoc = spinAcqLocPerDRE;
						spinRelLoc = spinRelLocPerDRE;
					}
					/*
					string fileName = SM.getFileEntryForID(SM.getMainFileID())->getName();
					cout<<fileName<<endl;
					cout<<"acquire loc("<<SM.getExpansionLineNumber(spinAcqLoc)<<":"<<SM.getExpansionColumnNumber(spinAcqLoc)<<")"<<endl; 
					cout<<"release loc("<<SM.getExpansionLineNumber(spinRelLoc)<<":"<<SM.getExpansionColumnNumber(spinRelLoc)<<")"<<endl; 
					*/
				}
				if(processState == EXTERN)
				{
				}
				if(processState == RENAME)
				{
					string fileName = SM.getFileEntryForID(SM.getMainFileID())->getName();
					fileName = fileName.substr(fileName.find_last_of('/')+1);
					fileName = fileName.substr(0, fileName.find_last_of('.'));
					reWriter.InsertText(dre->getLocEnd(), fileName+"_", true, true);
				}
				break;
			}
		}
	    }	
	    for(Stmt::child_iterator b = s->child_begin(), e = s->child_end(); b != e; ++b)
	    {
		myVisitStmt(*b, PM);
	    }
            return true;
        }
		
	    // Override methods
        bool VisitFunctionDecl(FunctionDecl *f) 
        {
            return true;
        }
        bool VisitStmt(Stmt *s) 
	{
            return true;
	}
        bool VisitDecl(Decl* dl)
        {
            if((isa<VarDecl>(*dl) || isa<ParmVarDecl>(*dl)) && (processState == LISTING1 || processState == LISTING2 || processState == SPIN || processState == EXTERN || processState == RENAME))
            {
                VarDecl *vd = cast<VarDecl>(dl);
		SourceLocation loc = dl->getLocEnd();
		unsigned int lineNumber = SM.getExpansionLineNumber(loc);
		unsigned int colNumber = SM.getExpansionColumnNumber(loc);

                VarInfo temp;
                temp.name = vd->getNameAsString();
                temp.type = vd->getType().getAsString();
		temp.lineNumber = lineNumber;
		temp.colNumber = colNumber;

		// Case of listing first file
                if(processState == LISTING1)
                {
                	list1.push_back(temp);
                }
		// Case of listing second file
                if(processState == LISTING2)
                {
                	list2.push_back(temp);
                }
		// Case of rewriting
		if(processState == SPIN || processState == EXTERN || processState == RENAME)
		{
			string varName = vd->getNameAsString();
			for(vector<VarInfo>::iterator it=list1.begin(); it != list1.end(); ++it)
			{
				if(varName == it->name && it->isSame == true)
				{
					if(processState == SPIN)
					{
					}
					if(processState == EXTERN)
					{
						reWriter.InsertText(vd->getTypeSpecStartLoc(), "extern ", true, true);
					}
					if(processState == RENAME)
					{
						string fileName = SM.getFileEntryForID(SM.getMainFileID())->getName();
						fileName = fileName.substr(fileName.find_last_of('/')+1);
						fileName = fileName.substr(0, fileName.find_last_of('.'));
						reWriter.InsertText(vd->getLocEnd(), fileName+"_", true, true);
					}
					break;
				}
			}
		}
	    }	
            return true;
        }

    private:
        Rewriter& reWriter;
        SourceManager& SM;
};


// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer 
{
    public:
	    // Constructor
        MyASTConsumer(Rewriter& R)
            : Visitor(R), reWriter(R)
        {
        }

        // Override the method that gets called for each parsed top-level
        // declaration.
        bool HandleTopLevelDecl(DeclGroupRef DR) override 
        {
            for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) 
            {
                // Traverse the declaration using our AST visitor.
		if((*b)->getBody() != NULL)
		{
			ParentMap PM((*b)->getBody());
			Visitor.myVisitStmt((*b)->getBody(), PM);
		}
                Visitor.TraverseDecl(*b);
		
		if(spinAcqLoc.isValid())
		{
			reWriter.InsertText(spinAcqLoc, "Spin_lock_Acquire();\n", true, true);
			reWriter.InsertText(spinRelLoc, "\nSpin_lock_Release();\n", true, true);
			spinAcqLoc = spinRelLoc = SourceLocation();
		}
                //(*b)->dump();
            }
            return true;
        }

    private:
        MyASTVisitor Visitor;
        Rewriter& reWriter;
};

void copyFile(const char *file)
{
    char backupFile[SIZE];
    strcpy(backupFile, file);
    strcat(backupFile, ".bak");

    ifstream src(file, ios::binary);
    ofstream dst(backupFile, ios::binary);

    dst<<src.rdbuf();
}

void parseASTPerFile(const char *file)
{
  // CompilerInstance will hold the instance of the Clang compiler for us,
  // managing the various objects needed to run the compiler.
  CompilerInstance TheCompInst;
  TheCompInst.createDiagnostics();

  LangOptions &lo = TheCompInst.getLangOpts();
  lo.CPlusPlus = 1;

  // Initialize target info with the default triple for our platform.
  auto TO = std::make_shared<TargetOptions>();
  TO->Triple = llvm::sys::getDefaultTargetTriple();
  TargetInfo *TI =
      TargetInfo::CreateTargetInfo(TheCompInst.getDiagnostics(), TO);
  TheCompInst.setTarget(TI);

  TheCompInst.createFileManager();
  FileManager &FileMgr = TheCompInst.getFileManager();
  TheCompInst.createSourceManager(FileMgr);
  SourceManager &SourceMgr = TheCompInst.getSourceManager();
  TheCompInst.createPreprocessor(TU_Module);
  TheCompInst.createASTContext();

  // A Rewriter helps us manage the code rewriting task.
  Rewriter TheRewriter;
  TheRewriter.setSourceMgr(SourceMgr, TheCompInst.getLangOpts());

  // Set the main file handled by the source manager to the input file.
  const FileEntry *FileIn = FileMgr.getFile(file);
  SourceMgr.setMainFileID(
      SourceMgr.createFileID(FileIn, SourceLocation(), SrcMgr::C_User));
  TheCompInst.getDiagnosticClient().BeginSourceFile(
      TheCompInst.getLangOpts(), &TheCompInst.getPreprocessor());

  // Create an AST consumer instance which is going to get called by
  // ParseAST.
  MyASTConsumer TheConsumer(TheRewriter);

  // Parse the file to AST, registering our consumer as the AST consumer.
  ParseAST(TheCompInst.getPreprocessor(), &TheConsumer,
           TheCompInst.getASTContext());

  // At this point the rewriter's buffer should be full with the rewritten
  // file contents.
  if(processState == SPIN || processState == EXTERN || processState == RENAME)
  {
    const RewriteBuffer *RewriteBuf =
        TheRewriter.getRewriteBufferFor(SourceMgr.getMainFileID());
    char backupFile[SIZE];
    strcpy(backupFile, file);
    strcat(backupFile, ".new");

    ofstream dst(backupFile, ios::binary);
    dst<<string(RewriteBuf->begin(), RewriteBuf->end());
  }
}

int ftw_list(const char* path, const struct stat*, int type)
{
	if(type == FTW_F)
	{
		parseASTPerFile(path);
	}
}

int main(int argc, const char **argv) 
{
    if(argc != 3)
    {
        cout<<"Usage: autoCodeRewriter <dir path1> <dir path2>\n"<<endl;
        return 1;
    }

    processState = LISTING1;
    cout<<"File '"<<argv[1]<<"' variables list"<<endl;
    sort(list1.begin(), list1.end(), VarInfoCmp);
    for(vector<VarInfo>::iterator it=list1.begin(); it != list1.end(); ++it)
    {
	cout<<it->name<<" (type : "<<it->type<<")"<<endl;
    }

    cout<<"=================================================================="<<endl<<endl;

    processState = LISTING2;
    parseASTPerFile(argv[2]);
    cout<<"File '"<<argv[2]<<"' variables list"<<endl;
    sort(list2.begin(), list2.end(), VarInfoCmp);
    for(vector<VarInfo>::iterator it=list2.begin(); it != list2.end(); ++it)
    {
	cout<<it->name<<" (type : "<<it->type<<")"<<endl;
    }

    int sameVarNames = 0;
    for(vector<VarInfo>::iterator it1=list1.begin(); it1 != list1.end(); ++it1)
    {
	    for(vector<VarInfo>::iterator it2=list2.begin(); it2 != list2.end(); ++it2)
	    {
		if(it1->name.compare(it2->name) == 0)
		{
			it1->isSame = it2->isSame = true;
			++sameVarNames;
			break;
		}
	    }
    }

    cout<<endl<<"Number of same variable's name between two files : "<<sameVarNames<<endl
    <<"Do you want to convert variables'name? [Y;Yes / N;No] ";
    char input;
    cin>>input;
    if(input == 'y' || input == 'Y')
    {
    	cout<<"Please select rewriter style. [S;Spin_lock / E;Extern / R;Rename] ";
    	cin>>input;
        switch(input)
  	{
		case 's':
		case 'S':
			/*
			copyFile(argv[1]);
			copyFile(argv[2]);
			cout<<"Backup the original sources automatically."<<endl;
			*/
			//process spin_lock
			processState = SPIN;
			parseASTPerFile(argv[1]);
			parseASTPerFile(argv[2]);
			cout<<"Spin_lock rewriter complete."<<endl;
			break;

		case 'e':
		case 'E':
			cout<<"Please select file to transform. [1;Flle1 / 2;File2] ";
			cin>>input;
			if(input == '1')
			{
				/*
				copyFile(argv[1]);
				cout<<"Backup the original source automatically."<<endl;
				*/
				//process extern
				processState = EXTERN;
				parseASTPerFile(argv[1]);
				cout<<"Extern rewriter complete."<<endl;
			}
			else if(input == '2')
			{
				/*
				copyFile(argv[2]);
				cout<<"Backup the original source automatically."<<endl;
				*/
				//process extern
				processState = EXTERN;
				parseASTPerFile(argv[2]);
				cout<<"Extern rewriter complete."<<endl;
			}
			else
			{
				cout<<"Rewriter cancelled."<<endl;
			}
			break;

		case 'r':
		case 'R':
			cout<<"Please select file to transform. [1;Flle1 / 2;File2 / 3;All files] ";
			cin>>input;
			if(input == '1')
			{
				/*
				copyFile(argv[1]);
				cout<<"Backup the original source automatically."<<endl;
				*/
				//process rename 
				processState = RENAME;
				parseASTPerFile(argv[1]);
				cout<<"Rewriter complete."<<endl;
			}
			else if(input == '2')
			{
				/*
				copyFile(argv[2]);
				cout<<"Backup the original source automatically."<<endl;
				*/
				//process rename 
				processState = RENAME;
				parseASTPerFile(argv[2]);
				cout<<"Rewriter complete."<<endl;
			}
			else if(input == '3')
			{
				/*
				copyFile(argv[2]);
				cout<<"Backup the original source automatically."<<endl;
				*/
				//process rename 
				processState = RENAME;
				parseASTPerFile(argv[1]);
				parseASTPerFile(argv[2]);
				cout<<"Rewriter complete."<<endl;
			}
			else
			{
				cout<<"Rewriter cancelled."<<endl;
			}
			break;

		default:
			cout<<"Rewriter cancelled."<<endl;
	}
    }
    return 0;
}
