/////////////////////////////////////////////////////////////////////
// Executive.h - Organizes and Directs Code Analysis               //
// ver 1.5                                                         //
//-----------------------------------------------------------------//
// Jim Fawcett (c) copyright 2016                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    HP EliteBook, Windows 10                           //
// Application: Project #2, CSE687 - Object Oriented Design, S2017 //
//  Author:     Manish Gupta, Syracuse University                  //
//                 (315) 412-8140, magupta@syr.edu 
// Sourcr:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////

#include "Executive.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <chrono>
#include <ctime>
#include "../Parser/Parser.h"
#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Dep_Anal/DepAnal.h"
#include "../TypeTable/TypeAnal.h"
#include "../StrongComponent/strongComp.h"
#include "../Display/Display.h"
using Rslt = Logging::StaticLogger<0>;  // use for application results
using Demo = Logging::StaticLogger<1>;  // use for demonstrations of processing
using Dbug = Logging::StaticLogger<2>;  // use for debug output


/////////////////////////////////////////////////////////////////////
// AnalFileMgr class
// - Derives from FileMgr to make application specific file handler
//   by overriding FileMgr::file(), FileMgr::dir(), and FileMgr::done()

using Path = std::string;
using File = std::string;
using Files = std::vector<File>;
using Pattern = std::string;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;


using namespace CodeAnalysis;
using namespace NoSQLDB;

//----< initialize application specific FileMgr >--------------------
/*
 * - Accepts CodeAnalysisExecutive's path and fileMap by reference
 */
AnalFileMgr::AnalFileMgr(const Path& path, FileMap& fileMap)
  : FileMgr(path), fileMap_(fileMap), numFiles_(0), numDirs_(0) {}

//----< override of FileMgr::file(...) to store found files >------

void AnalFileMgr::file(const File& f)
{
  File fqf = d_ + "\\" + f;
  Ext ext = FileSystem::Path::getExt(fqf);
  Pattern p = "*." + ext;
  fileMap_[p].push_back(fqf);
  ++numFiles_;
}
//----< override of FileMgr::dir(...) to save current dir >----------

void AnalFileMgr::dir(const Dir& d)
{
  d_ = d;
  ++numDirs_;
}
//----< override of FileMgr::done(), not currently used >------------

void AnalFileMgr::done()
{
}
//----< returns number of matched files from search >----------------

size_t AnalFileMgr::numFiles()
{
  return numFiles_;
}
//----< returns number of dirs searched >----------------------------

size_t AnalFileMgr::numDirs()
{
  return numDirs_;
}

/////////////////////////////////////////////////////////////////////
// CodeAnalysisExecutive class
// - 
using Path = std::string;
using Pattern = std::string;
using Patterns = std::vector<Pattern>;
using File = std::string;
using Files = std::vector<File>;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;
using ASTNodes = std::vector<ASTNode*>;
using FileToNodeCollection = std::vector<std::pair<File, ASTNode*>>;

//----< initialize parser, get access to repository >----------------

CodeAnalysisExecutive::CodeAnalysisExecutive()
{
  pParser_ = configure_.Build();
  if (pParser_ == nullptr)
  {
    throw std::exception("couldn't create parser");
  }
  pRepo_ = Repository::getInstance();
}
//----< cleanup >----------------------------------------------------

CodeAnalysisExecutive::~CodeAnalysisExecutive()
{
  /*
   *  Nothing to do:
   *  - pParser_ and pRepo_ point to objects that the configure_
   *    destructor will delete.
   *  - The AbstractSynTree object will cleanup its contents when
   *    it goes out of scope by deleting the root of the AST node
   *    tree.  
   *  - Each node deletes its children, so, again, nothing more 
   *    to do.
   *  - This is here to present these comments and to make this
   *    base destructor virtual.
   */
}
//----< usage message >----------------------------------------------

void showUsage()
{
  std::ostringstream out;
  out << "\n  Usage:";
  out << "\n  Command Line Arguments are:";
  out << "\n  - 1st: path to subdirectory containing files to analyze";
  out << "\n  - remaining non-option arguments are file patterns, e.g., *.h and/or *.cpp, etc.";
  out << "\n  - must have at least one file pattern to specify what to process";
  out << "\n  - option arguments have the format \"\"/x\" , where x is one of the options:";
  out << "\n    - m : display function metrics";
  out << "\n    - s : display file sizes";
  out << "\n    - a : display Abstract Syntax Tree";
  out << "\n    - r : set logger to display results";
  out << "\n    - d : set logger to display demo outputs";
  out << "\n    - b : set logger to display debug outputs";
  out << "\n    - f : write all logs to logfile.txt";
  out << "\n  A metrics summary is always shown, independent of any options used or not used";
  out << "\n\n";
  std::cout << out.str();
  //Rslt::write(out.str());
  //Rslt::flush();
}
//----< show command line arguments >--------------------------------

void CodeAnalysisExecutive::showCommandLineArguments(int argc, char* argv[])
{
  std::ostringstream out;
  out << "\n     Path: \"" << FileSystem::Path::getFullFileSpec(argv[1]) << "\"\n     Args: ";
  for (int i = 2; i < argc - 1; ++i)
    out << argv[i] << ", ";
  out << argv[argc - 1];
  Rslt::write(out.str());
  Rslt::flush();
}
//----< handle command line arguments >------------------------------
/*
* Arguments are:
* - path: possibly relative path to folder containing all analyzed code,
*   e.g., may be anywhere in the directory tree rooted at that path
* - patterns: one or more file patterns of the form *.h, *.cpp, and *.cs
* - options: /m (show metrics), /s (show file sizes), and /a (show AST)
*/
bool CodeAnalysisExecutive::ProcessCommandLine(int argc, char* argv[])
{
  if (argc < 2)
  {
    showUsage();
    return false;
  }
  try {
    path_ = FileSystem::Path::getFullFileSpec(argv[1]);
    if (!FileSystem::Directory::exists(path_))
    {
      std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
      return false;
    }
    for (int i = 2; i < argc; ++i)
    {
		if (argv[i][0] == '/')
			options_.push_back(argv[i][1]);
		else if ((argv[i][0] == '*'))
			patterns_.push_back(argv[i]);
		else
			ToXml_.push_back(argv[i]);
		
    }

    if (patterns_.size() == 0)
    {
      showUsage();
      return false;
    }
  }
  catch (std::exception& ex)
  {
    std::cout << "\n\n  command line argument parsing error:";
    std::cout << "\n  " << ex.what() << "\n\n";
    return false;
  }
  return true;
}
//----< returns path entered on command line >-------------------

std::string CodeAnalysisExecutive::getAnalysisPath()
{
  return path_;
}
//----< returns reference to FileMap >---------------------------
/*
 * Supports quickly finding all the files found with a give pattern
 */
FileMap& CodeAnalysisExecutive::getFileMap()
{
  return fileMap_;
}
//----< searches path for files matching specified patterns >----
/*
 * - Searches entire diretory tree rooted at path_, evaluated 
 *   from a command line argument.
 * - Saves found files in FileMap.
 */
void CodeAnalysisExecutive::getSourceFiles()
{
  AnalFileMgr fm(path_, fileMap_);
  for (auto patt : patterns_)
    fm.addPattern(patt);
  fm.search();
  numFiles_ = fm.numFiles();
  numDirs_ = fm.numDirs();
}
//----< helper: is text a substring of str? >--------------------

bool contains(const std::string& str, const std::string& text)
{
  if (str.find(text) < str.length())
    return true;
  return false;
}

//----< retrieve from fileMap all files matching *.h or *.cpp>-----------
std::vector<File>& CodeAnalysisExecutive::allFiles()
{
	allFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.h") || contains(item.first, "*.cpp"))
		{
			for (auto file : item.second)
				allFiles_.push_back(file);
		}
	}
	return allFiles_;
}


//----< retrieve from fileMap all files matching *.h >-----------

std::vector<File>& CodeAnalysisExecutive::cppHeaderFiles()
{
  cppHeaderFiles_.clear();
  for (auto item : fileMap_)
  {
    if (contains(item.first, "*.h"))
    {
      for (auto file : item.second)
        cppHeaderFiles_.push_back(file);
    }
  }
  return cppHeaderFiles_;
}
//----< retrieve from fileMap all files matching *.cpp >---------

std::vector<File>& CodeAnalysisExecutive::cppImplemFiles()
{
  cppImplemFiles_.clear();
  for (auto item : fileMap_)
  {
    if (contains(item.first, "*.cpp"))
    {
      for (auto file : item.second)
        cppImplemFiles_.push_back(file);
    }
  }
  return cppImplemFiles_;
}
//----< retrieve from fileMap all files matching *.cs >----------

std::vector<File>& CodeAnalysisExecutive::csharpFiles()
{
  csharpFiles_.clear();
  for (auto item : fileMap_)
  {
    if (contains(item.first, "*.cs"))
    {
      for (auto file : item.second)
        csharpFiles_.push_back(file);
    }
  }
  return csharpFiles_;
}
//----< report number of Source Lines Of Code (SLOCs) >----------

CodeAnalysisExecutive::Slocs CodeAnalysisExecutive::fileSLOCs(const File& file) 
{ 
  return slocMap_[file];
}
//----< report number of files processed >-----------------------

size_t CodeAnalysisExecutive::numFiles()
{
  return numFiles_;
}
//----< report number of directories searched >------------------

size_t CodeAnalysisExecutive::numDirs()
{
  return numDirs_;
}
//----< show processing activity >-------------------------------

void CodeAnalysisExecutive::showActivity(const File& file)
{
  std::function<std::string(std::string, size_t)> trunc = [](std::string in, size_t count)
  {
    return in.substr(0, count);
  };

  if (Rslt::running())
  {
    std::cout << std::left << "\r     Processing file: " << std::setw(80) << trunc(file, 80);
  }
}

void CodeAnalysisExecutive::clearActivity()
{
  if (Rslt::running())
  {
    std::cout << std::left << "\r                      " << std::setw(80) << std::string(80,' ');
  }
}
//----< parses code and saves results in AbstrSynTree >--------------
/*
* - Processes C++ header files first to build AST with nodes for
*   all public classes and structs.
* - Then processes C++ implementation files.  Each member function
*   is relocated to its class scope node, not the local scope.
* - Therefore, this ordering is important.
* - C# code has all member functions inline, so we don't need to
*   do any relocation of nodes in the AST.  Therefore, that analysis
*   can be done at any time.
* - If you bore down into the analysis code in ActionsAndRules.h you
*   will find some gymnastics to handle template syntax.  That can
*   get somewhat complicated, so there may be some latent bugs there.
*   I don't know of any at this time.
*/
void CodeAnalysisExecutive::setLanguage(const File& file)
{
  std::string ext = FileSystem::Path::getExt(file);
  if (ext == "h" || ext == "cpp")
    pRepo_->language() = Language::Cpp;
  else if (ext == "cs")
    pRepo_->language() = Language::CSharp;
}

void CodeAnalysisExecutive::processSourceCodecppHeader(bool showProc)
{
	for (auto file : cppHeaderFiles())
	{
		if (showProc)
			showActivity(file);
		pRepo_->package() = FileSystem::Path::getName(file);
		if (!configure_.Attach(file))
		{
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			Rslt::write(out.str());
			Rslt::flush();
			continue;
		}
		// parse file
		Rslt::flush();
		Demo::flush();
		Dbug::flush();
		if (!Rslt::running())
			Demo::write("\n\n  opening file \"" + pRepo_->package() + "\"");
		if (!Demo::running() && !Rslt::running())
			Dbug::write("\n\n  opening file \"" + pRepo_->package() + "\"");
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
		{
			pParser_->parse();
		}

		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
}
void CodeAnalysisExecutive::processSourceCodecppImplemFile(bool showProc)
{
	for (auto file : cppImplemFiles())
	{
		if (showProc)
			showActivity(file);
		pRepo_->package() = FileSystem::Path::getName(file);

		if (!configure_.Attach(file))
		{
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			Rslt::write(out.str());
			Rslt::flush();
			continue;
		}
		// parse file

		if (!Rslt::running())
			Demo::write("\n\n  opening file \"" + pRepo_->package() + "\"");
		if (!Demo::running() && !Rslt::running())
			Dbug::write("\n\n  opening file \"" + pRepo_->package() + "\"");
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();

		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
}
void CodeAnalysisExecutive::processSourceCodecsharp(bool showProc)
{ 
	for (auto file : csharpFiles())
	{
		if (showProc)
			showActivity(file);
		pRepo_->package() = FileSystem::Path::getName(file);

		if (!configure_.Attach(file))
		{
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			Rslt::write(out.str());
			continue;
		}
		// parse file

		if (!Rslt::running())
			Demo::write("\n\n  opening file \"" + pRepo_->package() + "\"");
		if (!Demo::running() && !Rslt::running())
			Dbug::write("\n\n  opening file \"" + pRepo_->package() + "\"");
		pRepo_->language() = Language::CSharp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();

		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
}
void CodeAnalysisExecutive::processSourceCode(bool showProc)
{
	processSourceCodecppHeader(showProc);
	processSourceCodecppImplemFile(showProc);
	processSourceCodecsharp(showProc);
 
  if (showProc)
    clearActivity();
  std::ostringstream out;
  out << std::left << "\r  " << std::setw(77) << " ";
  Rslt::write(out.str());
}
//----< evaluate complexities of each AST node >---------------------

void CodeAnalysisExecutive::complexityAnalysis()
{
  ASTNode* pGlobalScope = pRepo_->getGlobalScope();
  CodeAnalysis::complexityEval(pGlobalScope);
}
//----< comparison functor for sorting FileToNodeCollection >----
/*
* - supports stable sort on extension values
* - displayMetrics(...) uses to organize metrics display
*/
struct CompExts
{
  bool operator()(const std::pair<File, ASTNode*>& first, const std::pair<File, ASTNode*>& second)
  {
    return FileSystem::Path::getExt(first.first) > FileSystem::Path::getExt(second.first);
  }
};
//----< comparison functor for sorting FileToNodeCollection >----
/*
* - supports stable sort on name values
* - displayMetrics(...) uses these functions to organize metrics display
*/
static void removeExt(std::string& name)
{
  size_t extStartIndex = name.find_last_of('.');
  name = name.substr(0, extStartIndex);
}

struct CompNames
{
  bool operator()(const std::pair<File, ASTNode*>& first, const std::pair<File, ASTNode*>& second)
  {
    std::string fnm = FileSystem::Path::getName(first.first);
    removeExt(fnm);
    std::string snm = FileSystem::Path::getName(second.first);
    removeExt(snm);
    return fnm < snm;
  }
};
//----< display header line for displayMmetrics() >------------------

void CodeAnalysisExecutive::displayHeader()
{
  std::ostringstream out;
  out << std::right;
  out << "\n ";
  out << std::setw(25) << "file name";
  out << std::setw(12) << "type";
  out << std::setw(35) << "name";
  out << std::setw(8) << "line";
  out << std::setw(8) << "size";
  out << std::setw(8) << "cplx";
  out << std::right;
  out << "\n  ";
  out << std::setw(25) << "-----------------------";
  out << std::setw(12) << "----------";
  out << std::setw(35) << "---------------------------------";
  out << std::setw(8) << "------";
  out << std::setw(8) << "------";
  out << std::setw(8) << "------";
  Rslt::write(out.str());
}
//----< display single line for displayMetrics() >-------------------

void CodeAnalysisExecutive::displayMetricsLine(const File& file, ASTNode* pNode)
{
  std::function<std::string(std::string, size_t)> trunc = [](std::string in, size_t count)
  {
    return in.substr(0, count);
  };
  std::ostringstream out;
  out << std::right;
  out << "\n ";
  out << std::setw(25) << trunc(file, 23);
  out << std::setw(12) << pNode->type_;
  out << std::setw(35) << trunc(pNode->name_, 33);
  out << std::setw(8) << pNode->startLineCount_;
  out << std::setw(8) << pNode->endLineCount_ - pNode->startLineCount_ + 1;
  size_t debug1 = pNode->startLineCount_;
  size_t debug2 = pNode->endLineCount_;
  out << std::setw(8) << pNode->complexity_;
  Rslt::write(out.str());
}
//----< display lines containing public data declaration >-----------

std::string CodeAnalysisExecutive::showData(const Scanner::ITokCollection* pTc)
{
  std::string semiExpStr;
  for (size_t i=0; i<pTc->length(); ++i)
    semiExpStr += (*pTc)[i] + " ";
  return semiExpStr;
}

void CodeAnalysisExecutive::displayDataLines(ASTNode* pNode, bool isSummary)
{
  for (auto datum : pNode->decl_)
  {
    if (pNode->parentType_ == "namespace" || pNode->parentType_ == "class" || pNode->parentType_ == "struct")
    {
      if (pNode->type_ == "function" || pNode->parentType_ == "function")
        continue;
      if (datum.access_ == Access::publ && datum.declType_ == DeclType::dataDecl)
      {
        std::ostringstream out;
        out << std::right;
        out << "\n ";
        out << std::setw(25) << "public data:" << " ";
        if (isSummary)
        {
          out << datum.package_ << " : " << datum.line_ << " - "
            << pNode->type_ << " " << pNode->name_ << "\n " << std::setw(15) << " ";
        }
        out << showData(datum.pTc);
        Rslt::write(out.str());
      }
    }
  }
}
//----<  helper for displayMetrics() >-------------------------------
/*
* - Breaking this out as a separate function allows application to
*   display metrics for a subset of the Abstract Syntax Tree
*/
void CodeAnalysisExecutive::displayMetrics(ASTNode* root)
{
  flushLogger();
  std::ostringstream out;
  out << "Code Metrics - Start Line, Size (lines/code), and Complexity (number of scopes)";
  Utils::sTitle(out.str(), 3, 92, out, '=');
  out << "\n";
  Rslt::write(out.str());

  std::function<void(ASTNode* pNode)> co = [&](ASTNode* pNode) {
    if (
      pNode->type_ == "namespace" ||
      pNode->type_ == "function" ||
      pNode->type_ == "class" ||
      pNode->type_ == "interface" ||
      pNode->type_ == "struct" ||
      pNode->type_ == "lambda"
      )
      fileNodes_.push_back(std::pair<File, ASTNode*>(pNode->package_, pNode));
  };
  ASTWalkNoIndent(root, co);
  std::stable_sort(fileNodes_.begin(), fileNodes_.end(), CompExts());
  std::stable_sort(fileNodes_.begin(), fileNodes_.end(), CompNames());

  displayHeader();

  std::string prevFile;
  for (auto item : fileNodes_)
  {
    if (item.first != prevFile)
    {
      Rslt::write("\n");
      displayHeader();
    }
    displayMetricsLine(item.first, item.second);
    displayDataLines(item.second);
    prevFile = item.first;
  }
  Rslt::write("\n");
}
//----< display metrics results of code analysis >---------------

void CodeAnalysisExecutive::displayMetrics()
{
  ASTNode* pGlobalScope = pRepo_->getGlobalScope();
  displayMetrics(pGlobalScope);
}
//----< walk tree of element nodes >---------------------------------

template<typename element>
void TreeWalk(element* pItem, bool details = false)
{
  static std::string path;
  if (path != pItem->path_ && details == true)
  {
    path = pItem->path_;
    Rslt::write("\n" + path);
  }
  static size_t indentLevel = 0;
  std::ostringstream out;
  out << "\n  " << std::string(2 * indentLevel, ' ') << pItem->show();
  Rslt::write(out.str());
  auto iter = pItem->children_.begin();
  ++indentLevel;
  while (iter != pItem->children_.end())
  {
    TreeWalk(*iter);
    ++iter;
  }
  --indentLevel;
}
//----< display the AbstrSynTree build in processSourceCode() >------

void CodeAnalysisExecutive::displayAST()
{
  flushLogger();
  ASTNode* pGlobalScope = pRepo_->getGlobalScope();
  Utils::title("Abstract Syntax Tree");
  TreeWalk(pGlobalScope);
  Rslt::write("\n");
}
//----< show functions with metrics exceeding specified limits >-----

void CodeAnalysisExecutive::displayMetricSummary(size_t sMax, size_t cMax)
{
  flushLogger();
  std::ostringstream out;
  Utils::sTitle("Functions Exceeding Metric Limits and Public Data", 3, 92, out, '=');
  Rslt::write(out.str());
  displayHeader();

  if (fileNodes_.size() == 0)  // only build fileNodes_ if displayMetrics hasn't been called
  {
    std::function<void(ASTNode* pNode)> co = [&](ASTNode* pNode) {
      fileNodes_.push_back(std::pair<File, ASTNode*>(pNode->package_, pNode));
    };
    ASTNode* pGlobalNamespace = pRepo_->getGlobalScope();
    ASTWalkNoIndent(pGlobalNamespace, co);
    std::stable_sort(fileNodes_.begin(), fileNodes_.end(), CompExts());
    std::stable_sort(fileNodes_.begin(), fileNodes_.end(), CompNames());
  }
  for (auto item : fileNodes_)
  {
    if (item.second->type_ == "function")
    {
      size_t size = item.second->endLineCount_ - item.second->startLineCount_ + 1;
      size_t cmpl = item.second->complexity_;
      if (size > sMax || cmpl > cMax)
        displayMetricsLine(item.first, item.second);
    }
  }
  Rslt::write("\n");
  for (auto item : fileNodes_)
  {
    displayDataLines(item.second, true);
  }
  Rslt::write("\n");
}
//----< comparison functor for sorting SLOC display >----------------

struct compFiles
{
private:
  std::string ChangeFirstCharOfExt(const std::string& fileName) const
  {
    std::string temp = fileName;
    size_t pos = temp.find_last_of('.');
    if (pos < temp.size() - 1)
      if (temp[pos + 1] == 'h')
        temp[pos + 1] = 'a';
    return temp;
  }
public:
  bool operator()(const std::string& fileName1, const std::string& fileName2) const
  {
    return ChangeFirstCharOfExt(fileName1) < ChangeFirstCharOfExt(fileName2);
  }
};
//----< show sizes of all the files processed >----------------------

void CodeAnalysisExecutive::displaySlocs()
{
  flushLogger();
  Utils::sTitle("File Size - Source Lines of Code", 3, 92);
  size_t slocCount = 0;
  std::map<std::string, size_t, compFiles> fileColl;
  for (auto item : fileMap_)
  {
    for (auto file : item.second)
    {
      File fileName = FileSystem::Path::getName(file);
      fileColl[file] = slocMap_[fileName];
    }
  }
  for (auto fitem : fileColl)
  {
    std::ostringstream out;
    out << "\n  " << std::setw(8) << fitem.second << " : " << fitem.first;
    Rslt::write(out.str());
    slocCount += fitem.second;
  }
  std::ostringstream out;
  out << "\n\n      Total line count = " << slocCount << "\n";
  Rslt::write(out.str());
  Rslt::write("\n");
}
//----< display analysis info based on command line options >--------

void CodeAnalysisExecutive::dispatchOptionalDisplays()
{
  for (auto opt : options_)
  {
    switch (opt)
    {
    case 'm':
      displayMetrics();
      Rslt::start();
      break;
    case 'a':
      displayAST();
      Rslt::start();
      break;
    case 's':
      displaySlocs();
      Rslt::start();
      break;
    default:
      break;
    }
  }
}
//----< display analysis info based on command line options >--------

void CodeAnalysisExecutive::setDisplayModes()
{
  for (auto opt : options_)
  {
    switch (opt)
    {
    case 'r':
      Rslt::start();
      break;
    case 'd':
      Demo::start();
      break;
    case 'b':
      Dbug::start();
      break;
    case 'f':
      setLogFile("logFile.txt");
      break;
    default:
      if (opt != 'a' && opt != 'b' && opt != 'd' && opt != 'f' && opt != 'm' && opt != 'r' && opt != 's')
      {
        std::cout << "\n\n  unknown option " << opt << "\n\n";
      }
    }
  }
}
//----< helper functions for managing application's logging >--------

void CodeAnalysisExecutive::startLogger(std::ostream& out)
{
  Rslt::attach(&out);
  Demo::attach(&out);
  Dbug::attach(&out);

  // will start Demo and Dbug if  have options /d and /b

  setDisplayModes();
}

void CodeAnalysisExecutive::flushLogger()
{
  Rslt::flush();
  Demo::flush();
  Dbug::flush();
}

void CodeAnalysisExecutive::stopLogger()
{
  Rslt::flush();
  Demo::flush();
  Dbug::flush();
  Rslt::stop();
  Demo::stop();
  Dbug::stop();
}
//----< open file stream for logging >-------------------------------
/*
*  - must come after CodeAnalysisExecutive::processCommandLine()
*  - must come before starting any of the loggers
*/
void CodeAnalysisExecutive::setLogFile(const File& file)
{
  std::string path = getAnalysisPath();
  path += "\\" + file;
  pLogStrm_ = new std::ofstream(path);
  if (pLogStrm_->good())
  {
    Rslt::attach(pLogStrm_);
    Demo::attach(pLogStrm_);
    Dbug::attach(pLogStrm_);
  }
  else
    Rslt::write("\n  couldn't open logFile.txt for writing");
}


std::string CodeAnalysisExecutive::systemTime()
{ 
  time_t sysTime = time(&sysTime);
  char buffer[27];
  ctime_s(buffer, 27, &sysTime);
  buffer[24] = '\0';
  std::string temp(buffer);
  return temp;
}
std::vector<std::string> CodeAnalysisExecutive::getXmlFilePath()
{
	return ToXml_;
}
/////////////////////////////////////////////////////////////////
//Project 2 requirement 1
void CodeAnalysisExecutive::requirement1()
{
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "Submitted by: Manish Gupta\n";
	std::cout << "Requirement 1:\n";
	std::cout << " Used Visual Studio 2015 and its C++ Windows Console \n Projects, as provided in the ECS computer labs.";
	std::cout << "\n---------------------------------------------------------\n";
}
/////////////////////////////////////////////////////////////////
//Project 2 requirement 2
void CodeAnalysisExecutive::requirement2()
{
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "Requirement 2:\n";
	std::cout << " using the C++ standard library's streams for all I/O and \n new and delete for all heap-based memory management";
	std::cout << "\n---------------------------------------------------------\n";
}
/////////////////////////////////////////////////////////////////
//Project 2 requirement 3
void CodeAnalysisExecutive::requirement3()
{
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "Requirement 3:\n";
	std::cout << " C++ packages as described in the Purpose section: \n";
	std::cout << " Following packages were made in this project \n";
	std::cout << " 1. TypeTable. \n";
	std::cout << " 2. TypeAnalysis. \n";
	std::cout << " 3. DependencyAnalysis. \n";
	std::cout << " 4. Strong Components. \n";
	std::cout << " Following packages were modified/used from first project \n";
	std::cout << " 1. NoSqlDb \n";
	std::cout << " 2. Display. \n";
	std::cout << "\n---------------------------------------------------------\n";
}


//----< conduct code analysis >--------------------------------------

#ifdef TEST_EXECUTIVE
/////////////////////////////////////////////////////////////////
//Project 2 requirement 4
TypeTable requirement4()
{
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "Requirement 4:\n";
	TypeAnal ta;
	ta.doTypeAnal();
	TypeTable TT = ta.getTT();
	DisplayType dis_tt(TT);
	dis_tt.showTypeAnal();
	std::cout << "\n---------------------------------------------------------\n";
	return TT;
}
/////////////////////////////////////////////////////////////////
//Project 2 requirement 5
NoSqlDb<std::string>requirement5(TypeTable TT, Files files_)
{
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "Requirement 5:\n";
	DepAnal da;
	for (size_t t = 0; t < files_.size(); t++)
		da.doDepAnal(TT, files_[t]);
	NoSqlDb<std::string> depanal = da.getdepanal();
	NoSQLDB::Display<std::string> display(depanal);
	display.showDepAnal();
	std::cout << "\n---------------------------------------------------------\n";
	return depanal;
}
/////////////////////////////////////////////////////////////////
//Project 2 requirement 6
NoSqlDb<std::string>requirement6(NoSqlDb<std::string> depanal)
{
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "Requirement 6:\n";
	StrongComp sc = StrongComp(depanal);
	sc.strcomanal();
	NoSqlDb<std::string> SCanal = sc.getStrongComp();
	NoSQLDB::Display<std::string> SCdisplay(SCanal);
	SCdisplay.showSCAnal();
	std::cout << "\n---------------------------------------------------------\n";
	return SCanal;
}
/////////////////////////////////////////////////////////////////
//Project 2 requirement 7
void requirement7(NoSqlDb<std::string> depanal, NoSqlDb<std::string> SCanal, Files xmlPath)
{
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "Requirement 7:\n";
	std::string dep_pathname;
	std::string SC_pathname;
	if (xmlPath.size() == 0 || xmlPath.size()>2) {
		dep_pathname = "depanal.xml";
		SC_pathname = "SCanal.xml";
	}
	else if (xmlPath.size() == 1) {
		dep_pathname = xmlPath[0];
		SC_pathname = "SCanal.xml";
	}
	else {
		dep_pathname = xmlPath[0];
		SC_pathname = xmlPath[1];
	}
	Persist<std::string> savexml(depanal);
	savexml.depanal_saveToFile(dep_pathname);
	Persist<std::string> saveSCxml(SCanal);
	saveSCxml.SC_saveToFile(SC_pathname);
	std::cout << "Dependency analysis and strong component analysis\nsaved in xml files as given by command line\nor default folder\n";
	std::cout << "\n---------------------------------------------------------\n";
}
/////////////////////////////////////////////////////////////////
//Project 2 requirement 8
void requirement8()
{
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "Requirement 8:\n";
	std::cout << "Process this program using command line\n";
	std::cout << "Path to the directory tree containing files to analyze.\n";
	std::cout << "List of file patterns to match for selection of files to analyze.\n";
	std::cout<<	"Path of XML results file, supplying a default if no specification is provided.\n";
	std::cout << "\n---------------------------------------------------------\n";
	
}
/////////////////////////////////////////////////////////////////
//Project 2 requirement 9
void requirement9()
{
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "Requirement 9:\n";
	std::cout << "include an automated unit test suite that demonstrates\n all the requirements of this project.\n";
	std::cout << "The public data found in this projects are on unmodified files provided by professor\n";
	std::cout << "Few functions exceed the allowed metric but they are in unmodified files provided by professor\n";
	std::cout << "\n---------------------------------------------------------\n";
}
#include <fstream>
/////////////////////////////////////////////////////////////////
//Project 2 main function
int main(int argc, char* argv[])
{
  using namespace CodeAnalysis;
  CodeAnalysisExecutive exec;
  try {
    bool succeeded = exec.ProcessCommandLine(argc, argv);
    if (!succeeded)
       return 1;
    exec.getSourceFiles();  //get all files
    exec.processSourceCode(true); //process files
    exec.flushLogger();

	Files files_ = exec.allFiles();
	Files xmlPath = exec.getXmlFilePath(); //get command lines for xml path
	
    exec.flushLogger();
    Rslt::write("\n");
    std::ostringstream out;
    Rslt::write(out.str());
    Rslt::write("\n");
    exec.stopLogger();

	exec.requirement1();
	exec.requirement2();
	exec.requirement3();
	TypeTable TT = requirement4();
	NoSqlDb<std::string> depanal = requirement5(TT, files_);
	NoSqlDb<std::string> SCanal = requirement6(depanal);
	requirement7(depanal, SCanal, xmlPath);
	requirement8();
	requirement9();
  }
  catch (std::exception& except)
  {
    exec.flushLogger();
    std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
    exec.stopLogger();
    return 1;
  }
   return 0;
}
#endif // !TEST_EXECUTIVE