/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <dirent.h>
#include <algorithm>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
   // TODO...
   //cout << _dofileStack.size() << endl;
   if(_dofileStack.size() > 1024)
   {
      while (_dofileStack.size() != 0)
      {
         _dofileStack.pop();
      }
      //cerr << "Error!!" << endl;
      return false;
   }
   _dofileStack.push(_dofile);
   _dofile = new ifstream(dof.c_str());
   //_dofileStack.push(_dofile);
   if(!_dofile -> is_open())
   {
      if (_dofileStack.size() != 0)
      {
         _dofile = _dofileStack.top();
         _dofileStack.pop();
      }
      //delete _dofile;
      //cerr << "Unable to open dofile!!" << endl;
      return false;
   }
   else
   {
      return true;
   }
   //return true;
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
   assert(_dofile != 0);
   // TODO...
    if (_dofile ->is_open())
   {
      _dofile -> close();
      //delete _dofile;
   }
   delete _dofile;
   _dofile = NULL;
   if (_dofileStack.size() != 0)
   {
      _dofile = _dofileStack.top();
      _dofileStack.pop();
   }
   /*else
   {
      delete _dofile;
      _dofile = NULL;
   }*/
   
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);
   else
      newCmd = readCmd(cin);

   // execute the command
   if (newCmd) {
      string option;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
   // TODO...
   for (CmdMap::const_iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it)
   {
      it -> second -> help();
   }
   cout << endl;
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
   assert(_tempCmdStored == false);
   assert(!_history.empty());
   string str = _history.back();

   // TODO...
   assert(str[0] != 0 && str[0] != ' ');
   string cmd;
   size_t token_pos = myStrGetTok(str, cmd);
   if (getCmd(cmd) == 0)
   {
      cerr << "Illegal command!! (" << cmd << ")" << endl;
      //return NULL;
   }
   else
   {
      if (token_pos < str.size() - 1)
      {
         size_t trailing_pos = str.find_first_not_of(' ', token_pos);
         option = str.substr(trailing_pos);
      }
      else
      {
         option = "";
      }
      //_tabPressCount = 0;
   }
   
   return getCmd(cmd);
   //return NULL;
}

// Remove this function for TODO...
//
// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS
//    --- 1.1 ---
//    [Before] Null cmd
//    cmd> $
//    --- 1.2 ---
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. LIST ALL PARTIALLY MATCHED COMMANDS
//    --- 2.1 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    --- 2.2 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$llo                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$llo                // and then re-print the partial command
//
// 3. LIST THE SINGLY MATCHED COMMAND
//    ==> In either of the following cases, print out cmd + ' '
//    ==> and reset _tabPressCount to 0
//    --- 3.1 ---
//    [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $               // auto completed with a space inserted
//    --- 3.2 ---
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$ahah
//    [After Tab]
//    cmd> heLp $ahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//    --- 3.3 ---
//    [Before] fully matched (cursor right behind cmd)
//    cmd> hElP$sdf
//    [After Tab]
//    cmd> hElP $sdf            // a space character is inserted
//
// 4. NO MATCH IN FITST WORD
//    --- 4.1 ---
//    [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
//    --- 5.1 ---
//    [Before] Already matched on first tab pressing
//    cmd> help asd$gh
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$gh
//
// 6. FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
//    ==> Note: command usage has been printed under first tab press
//    ==> Check the word the cursor is at; get the prefix before the cursor
//    ==> So, this is to list the file names under current directory that
//        match the prefix
//    ==> List all the matched file names alphabetically by:
//           cout << setw(16) << left << fileName;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location
//    Considering the following cases in which prefix is empty:
//    --- 6.1.1 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and they do not have a common prefix,
//    cmd> help $sdfgh
//    [After] print all the file names
//    .               ..              Homework_3.docx Homework_3.pdf  Makefile
//    MustExist.txt   MustRemove.txt  bin             dofiles         include
//    lib             mydb            ref             src             testdb
//    cmd> help $sdfgh
//    --- 6.1.2 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and all of them have a common prefix,
//    cmd> help $orld
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    // e.g. in hw3/ref
//    cmd> help mydb-$orld
//    ==> DO NOT print the matched files
//    ==> If "tab" is pressed again, see 6.2
//    --- 6.1.3 ---
//    [Before] if prefix is empty, and only one file in the current directory
//    cmd> help $ydb
//    [After] print out the single file name followed by a ' '
//    // e.g. in hw3/bin
//    cmd> help mydb $
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.2 ---
//    [Before] with a prefix and with mutiple matched files
//    cmd> help M$Donald
//    [After]
//    Makefile        MustExist.txt   MustRemove.txt
//    cmd> help M$Donald
//    --- 6.3 ---
//    [Before] with a prefix and with mutiple matched files,
//             and these matched files have a common prefix
//    cmd> help Mu$k
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help Must$k
//    --- 6.4 ---
//    [Before] with a prefix and with a singly matched file
//    cmd> help MustE$aa
//    [After] insert the remaining of the matched file name followed by a ' '
//    cmd> help MustExist.txt $aa
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.5 ---
//    [Before] with a prefix and NO matched file
//    cmd> help Ye$kk
//    [After] beep and stay in the same location
//    cmd> help Ye$kk
//
//    [Note] The counting of tab press is reset after "newline" is entered.
//
// 7. FIRST WORD NO MATCH
//    --- 7.1 ---
//    [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location
void
CmdParser::listCmd(const string& str)
{
   // TODO...
   //cout << endl << _tabPressCount << endl;
   string str_before_cursor = str.substr(0, size_t(_readBufPtr - _readBuf));
   if (str_before_cursor.find_first_not_of(' ', 0) == str_before_cursor.npos)
   {
      int i = 0;
      char* temp_ptr = _readBufPtr;
      //_history.push_back(str);
      moveBufPtr(_readBufEnd);
      cout << endl;
      for (CmdMap::iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it)
      {
         ++i;
         cout << setw(12) << left << it -> first + it -> second -> getOptCmd();
         if (i % 5 == 0 && i != _cmdMap.size()) cout << endl;
      }
      cout << endl;
      printPrompt();
      cout << _readBuf;
      //_readBufPtr = _readBuf + str.size();
      moveBufPtr(temp_ptr);
      //_history.pop_back();
      return ;
   }
   else
   {
      str_before_cursor = str_before_cursor.substr(str_before_cursor.find_first_not_of(' ', 0));
      //size_t sub_str_len = str_before_cursor.size();
      //cout << str_before_cursor << endl;
      if (str_before_cursor.find_first_of(' ') != str_before_cursor.npos) // at least one space char
      {
         string cmd;
         myStrGetTok(str_before_cursor, cmd);
         //cout << cmd << endl;
         if (getCmd(cmd) == 0)  // no matches
         {
            mybeep();
            return ;
         }
         else // matches
         {
            //_tabPressCount = 0;
            if (_tabPressCount ==1)
            {
               char* temp_ptr = _readBufPtr;
               moveBufPtr(_readBufEnd);
               cout << endl;
               getCmd(cmd) -> usage(cout);
               //++_tabPressCount;
               printPrompt();
               cout << _readBuf;
               _readBufPtr = _readBufEnd;
               moveBufPtr(temp_ptr);
            }
            else// if (_tabPressCount == 2)  // second press
            {
               if (str_before_cursor.find_last_of(' ') == str_before_cursor.size() - 1) // space before cursor
               {
                  vector<string> files;
                  string dir = ".";
                  DIR *dp;
                  dirent *dirp;
                  if ((dp = opendir(dir.c_str())) == NULL) 
                  {
                     cerr << "Error(" << errno << "): failed to open " << dir << "!!\n";
                     return ;
                  }
                  else
                  {
                     while ((dirp = readdir(dp)) != NULL)
                     {
                        files.push_back(string(dirp->d_name));
                     }
                     sort(files.begin(), files.end());
                     closedir(dp);
                  }
                  if (files.size() == 1) // only one file
                  {
                     string match = files.back() + ' ';
                     for (char* i = _readBufEnd; i >= _readBufPtr; --i)
                     {
                        *(i + match.size()) = *i;
                     }
                     char* temp_ptr = _readBufPtr + match.size();
                     _readBufEnd += match.size();
                     int j = 0;
                     for (char* i = _readBufPtr; i < _readBufPtr + match.size(); ++i)
                     {
                       *i = match[j++];
                     }
                     cout <<  _readBufPtr;
                     _readBufPtr = _readBufEnd;
                     moveBufPtr(temp_ptr);
                     //++_tabPressCount;
                     return ;
                  }
                  // more than one file
                  string prefix = "";
                  int check_prefix = files.size() - 1;
                  while (check_prefix == files.size() - 1)
                  {
                     check_prefix = 0;
                     for (int i = 0; i < files.size() - 1; ++i)
                     {
                        if(files[i][prefix.size()] == files[i + 1][prefix.size()] && files[i][prefix.size()] != NULL && files[i + 1][prefix.size()] != NULL)
                        {
                           ++check_prefix;
                        }
                        else
                        {
                           break;
                        }
                     }
                     if (check_prefix == files.size() - 1)
                     {
                        prefix += files[0][prefix.size()];
                     }
                  }
                  if (prefix.size() == 0) // no common prefix
                  {
                     char* temp_ptr = _readBufPtr;
                     moveBufPtr(_readBufEnd);
                     cout << endl;
                     for (int i = 0; i < files.size(); ++i)
                     {
                        cout << setw(16) << left << files[i];
                        if (i % 5 == 4 && i != files.size() - 1) cout << endl;
                     }
                     cout << endl;
                     printPrompt();
                     cout << _readBuf;
                     moveBufPtr(temp_ptr);
                     return ;
                  }
                  else // common prefix
                  {
                     for (char* i = _readBufEnd; i >= _readBufPtr; --i)
                     {
                        *(i + prefix.size()) = *i;
                     }
                     char* temp_ptr = _readBufPtr + prefix.size();
                     _readBufEnd += prefix.size();
                     int j = 0;
                     for (char* i = _readBufPtr; i < _readBufPtr + prefix.size(); ++i)
                     {
                       *i = prefix[j++];
                     }
                     cout <<  _readBufPtr;
                     _readBufPtr = _readBufEnd;
                     moveBufPtr(temp_ptr);
                     mybeep();
                     return ;
                  }
               }
               else // space is not before cursor
               {
                  string prefix = str_before_cursor.substr(str_before_cursor.find_last_of(' ') + 1);
                  //cout << endl << prefix;
                  vector<string> files;
                  listDir(files, prefix, ".");
                  if (files.size() == 0)
                  {
                     mybeep();
                     return ;
                  }
                  else if (files.size() == 1) // only one match file
                  {
                     string match = files.back().substr(prefix.size()) + ' ';
                     for (char* i = _readBufEnd; i >= _readBufPtr; --i)
                     {
                        *(i + match.size()) = *i;
                     }
                     char* temp_ptr = _readBufPtr + match.size();
                     _readBufEnd += match.size();
                     int j = 0;
                     for (char* i = _readBufPtr; i < _readBufPtr + match.size(); ++i)
                     {
                       *i = match[j++];
                     }
                     cout <<  _readBufPtr;
                     _readBufPtr = _readBufEnd;
                     moveBufPtr(temp_ptr);
                     return ;
                  }
                  else // more than one match file
                  {
                     string file_prefix = "";
                     int check_prefix = files.size() - 1;
                     while (check_prefix == files.size() - 1)
                     {
                        check_prefix = 0;
                        for (int i = 0; i < files.size() - 1; ++i)
                        {
                           if(files[i][prefix.size() + file_prefix.size()] == files[i + 1][prefix.size() + file_prefix.size()] && files[i][prefix.size() + file_prefix.size()] != NULL && files[i + 1][prefix.size() + file_prefix.size()] != NULL)
                           {
                              ++check_prefix;
                           }
                           else
                           {
                              break;
                           }
                        }
                        if (check_prefix == files.size() - 1)
                        {
                           file_prefix += files[0][file_prefix.size() + prefix.size()];
                        }
                     }
                     if (file_prefix.size() == 0) // file no common prefix
                     {
                        char* temp_ptr = _readBufPtr;
                        moveBufPtr(_readBufEnd);
                        cout << endl;
                        for (int i = 0; i < files.size(); ++i)
                        {
                           cout << setw(16) << left << files[i];
                           if (i % 5 == 4 && i != files.size() - 1) cout << endl;
                        }
                        cout << endl;
                        printPrompt();
                        cout << _readBuf;
                        moveBufPtr(temp_ptr);
                        return ;
                     }
                     else // file common prefix
                     {
                        for (char* i = _readBufEnd; i >= _readBufPtr; --i)
                        {
                           *(i + file_prefix.size()) = *i;
                        }
                        char* temp_ptr = _readBufPtr + prefix.size();
                        _readBufEnd += prefix.size();
                        int j = 0;
                        for (char* i = _readBufPtr; i < _readBufPtr + prefix.size(); ++i)
                        {
                           *i = file_prefix[j++];
                        }
                        cout <<  _readBufPtr;
                        _readBufPtr = _readBufEnd;
                        moveBufPtr(temp_ptr);
                        mybeep();
                        return ;
                     }
                  }
               }
            }
            /*else
            {
               mybeep();
               return ;
            }*/
         }
      }
      else  // no space char
      {
         //int possible_match = 0;
         vector<string> possible_cmd;
         for (CmdMap::iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it)
         {
            //cout << it -> first + it -> second -> getOptCmd() << endl;
            //cout <<myStrNCmp(it -> first + it -> second -> getOptCmd(), str_before_cursor, str_before_cursor.size()) << endl;
            if (myStrNCmp(it -> first + it -> second -> getOptCmd(), str_before_cursor, str_before_cursor.size()) == 0)
            {
               //cout << it -> first + it -> second -> getOptCmd() << endl;
               possible_cmd.push_back(it -> first + it -> second -> getOptCmd());
            }
         }
         //cout << str_before_cursor.size();
         if (possible_cmd.size() == 0) // no matches
         {
            mybeep();
            return ;
         }
         else if (possible_cmd.size() == 1)  // one match
         {
            string match = possible_cmd.back().substr(str_before_cursor.size()) + ' ';
            //cout << endl << _readBufPtr << endl;
            for (char* i = _readBufEnd; i >= _readBufPtr; --i)
            {
               *(i + match.size()) = *i;
            }
            //cout << endl << _readBufPtr;
            char* temp_ptr = _readBufPtr + match.size();
            _readBufEnd += match.size();
            int j = 0;
            for (char* i = _readBufPtr; i < _readBufPtr + match.size(); ++i)
            {
               *i = match[j++];
            }
            cout <<  _readBufPtr;
            _readBufPtr = _readBufEnd;
            moveBufPtr(temp_ptr);
            _tabPressCount = 0;
            return ;
         }
         else
         {
            char* temp_ptr = _readBufPtr;
            moveBufPtr(_readBufEnd);
            cout << endl;
            for (int i = 0; i < possible_cmd.size(); ++i)
            {
               cout << setw(12) << left << possible_cmd[i];
               if (i % 5 == 4 && i != possible_cmd.size() - 1) cout << endl;
            }
            cout << endl;
            printPrompt();
            cout << _readBuf;
            moveBufPtr(temp_ptr);
         }
         possible_cmd.clear();
      }
   }
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{
   CmdExec* e = 0;
   // TODO...
   for (CmdMap::iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it)
   {
      if (myStrNCmp(it -> first + it -> second -> getOptCmd(), cmd, it -> first.size()) == 0)
      {
         e = it -> second;
         break;
      }
   }
   /*it = _cmdMap.find(cmd);
   if (it != _cmdMap.end())
   {
      e = it -> second;
   }*/
   return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// return false if option contains an token
bool
CmdExec::lexNoOption(const string& option) const
{
   string err;
   myStrGetTok(option, err);
   if (err.size()) {
      errorOption(CMD_OPT_EXTRA, err);
      return false;
   }
   return true;
}

// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

