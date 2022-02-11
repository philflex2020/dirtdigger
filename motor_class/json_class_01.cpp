// work with words and json
// this demo reads a json file and builds a dict of words in the file
// we really want to decode words
// build a dictionary
// output a compressed representation
//
#include <iostream>
#include <fstream>
#include <string>
#include <cjson/cJSON.h>

// g++ -o jc_01 json_class_01.cpp

using namespace std;

int g_index = 0;

class Word {
public:
  Word() {
    word = "";
    next =  NULL;
    instring = 0;
    esc = 0;
    used = 0;
    idx = ++g_index;
  };
  ~Word(){};
  int addChar(char c);
  char getChar(int i){ return word[i];};
  Word *getNext(){ return next;};
  Word *setNext(Word * w){ next=w;};
  Word *addWord();
  string getWord(){ return word;};
  string showWord();
  int length(){ return word.length();};
  int incUsed() {return ++used;};
  int getUsed() {return used;};
  void setIdx(int i) {idx = i;};
  int getIdx() {return idx;};


private:
  int used;
  int idx;
  int instring;
  int esc;
  string word;
  Word *next;
};

// This guy processes a whole file somehow
//  input {"name":"john"  , "age": 32}
// output  {00,01:02:03}
// with a dict of 00:name 01:john 02 age 03 32 
class Parser {
public:
  Parser();
  ~Parser();
  int numWords;
  int nxtWord;
  Word *words;
  int addChar(char c);
  int addChar1(char c);
  int addChar2(char c);
  string getString();
  string getWords();
  string decode();
  void clearOutput();
  int isCmd(char c);
  string output;
  int inQuote;
  string tmpWord;
};

Parser::Parser() {
  words = NULL;
  output = "";
  numWords = 0;
  nxtWord = 0;
  inQuote = 0;
  tmpWord = "";
}

Parser::~Parser() {
  // delete   words = NULL;
  numWords = 0;
  nxtWord = 0;
  // delete all words
}
// c can be
// '"'
// ','
//'{', '}'
//':' ,
// '1-9a-zA-z'

char ckey = '~';

int findWordIdx(string w);
string getWordIdx(int idx);

Word *findWord(string w);
void addWord(Word * w);
int newWord(string ws);

int isNum(char c)
{
  int rc = 0;
  if (c >= '0' && c <= '9') rc = 1;
  return rc;
}

string Parser::decode()
{
  char c;
  string num = "";
  string res = "";
  string ws = output;
  int indx;
  cout << "decode string ["<<ws<<"]"<<endl;
  
  for (int i = 0; i < ws.length(); i++) {
    c = ws[i];
    if (c == ckey) {
      i++;
      for (;(i<ws.length() && isNum(ws[i])) ; i++) {
	num += ws[i];
      }
      indx = atoi(num.c_str());
      
      tmpWord = getWordIdx(indx);
      cout << " indx " << indx << " tmp ["<< tmpWord<<"]" << endl;
      if(tmpWord.length() > 0) {
	if(res.length() > 0) res += " ";
	res += tmpWord;
	
      } else {
	res += num;
      }
      if (ws[i] == ckey) i--;
      num="";
    } else {
      res += c;
    }
  }
  return res;
}

// simple addChar just looks for spaces
int Parser::addChar(char c) {
  //  input {"name":"john"  , "age": 32}
  char ctmp[64];
  int indx;
  Word *w;
  if (
      (c == ' ')
      ||(c == '\n')
      ||(c == 0)
      )
    {
      if(tmpWord.length() > 0) {
	cout << " adding word ["<< tmpWord<<"]" << endl;
	indx = findWordIdx(tmpWord);
	cout << " got indx " << indx << endl;
	
	if (indx<0) {
	  newWord(tmpWord);
	  indx = findWordIdx(tmpWord);
	  cout << " after newword got indx " << indx << endl;
	  
	}
	
	tmpWord = "";
	snprintf(ctmp,sizeof(ctmp),"%c%x", ckey,indx);
	output +=ctmp;
	return 0;
      } else {
	output +=c;
      }
    } else {
    tmpWord += c;
  }
  return 0;
}

int Parser::isCmd(char c)
{
  int rc = 0;
  if     (c ==',') rc = 1;
  else if(c =='"') rc = 1;
  else if(c =='{') rc = 1;
  else if(c =='}') rc = 1;
  else if(c =='[') rc = 1;
  else if(c ==']') rc = 1;
  else if(c ==':') rc = 1;
  return rc;
}

// simple addChar just looks for commands ",',{,},[,],:
int Parser::addChar1(char c) {
  //  input {"name":"john"  , "age": 32}
  char ctmp[64];
  int indx;
  Word *w;
  if ( isCmd(c) ) {
    	output +=c;
	return 0;
  }
    
  if (
      (c == ' ')
      ||(c == '\n')
      ||(c == 0)
      )
    {
      if(tmpWord.length() > 0) {
	cout << " adding word ["<< tmpWord<<"]" << endl;
	indx = findWordIdx(tmpWord);
	cout << " got indx " << indx << endl;
	
	if (indx<0) {
	  newWord(tmpWord);
	  indx = findWordIdx(tmpWord);
	  cout << " after newword got indx " << indx << endl;
	  
	}
	
	tmpWord = "";
	snprintf(ctmp,sizeof(ctmp),"%c%x", ckey,indx);
	output +=ctmp;
	return 0;
      } else {
	output +=c;
      }
    } else {
    tmpWord += c;
  }
  return 0;
}


int Parser::addChar2(char c) {
  //  input {"name":"john"  , "age": 32}
  char ctmp[64];
  int indx;
  Word *w;
  if (inQuote == 1 ) {
    if (c == '"') {
      inQuote = 0;
      nxtWord = 1;
      indx = findWordIdx(tmpWord);
      if (indx<0) {
	newWord(tmpWord);
	indx = findWordIdx(tmpWord);
      }
      
      tmpWord = "";
      snprintf(ctmp,sizeof(ctmp),"%x", indx);
      output +=c;
      return 0;
    } else {
      output +=c;
    }
  }
  if (inQuote == 0 ) {
    if((c == '}') 
       || (c == '{')
       || (c == ',') 
       ){
      nxtWord = 0;

      indx = findWordIdx(tmpWord);
      if (indx<0) {
	newWord(tmpWord);
	indx = findWordIdx(tmpWord);

      }
      tmpWord = "";

      snprintf(ctmp,sizeof(ctmp),"%x", indx);
      output += ctmp;
      output += c;
      return 0;
    }
    
    if (c == '"') {
      tmpWord = "";
      output += c;
      inQuote = 1;
      return 0;
    }
  }
  return 0;
}



string Parser::getString() {
  return output;
}

void Parser::clearOutput() {
  output="";
}

string Parser::getWords() {
  string dst="";
  char sidx[32];
  Word *next = words;
  while (next) {
    Word *widx= next->getNext();
    snprintf(sidx,sizeof(sidx),"%x:%x:"
	     , next->getIdx()
	     , widx->length()
	     );
    dst += sidx;

    next= next->getNext();
  }
  return dst;
}


Word * g_words = NULL;

// adds chars but ignores leading spaces unless we are in a string
// a word terminates with either an out of string  ' or } 
int Word::addChar(char c)
{
  if(!instring) {
    if (c == ' ') return 0;
    if (c == '\"') {
      instring = 1;
      return 0;
    }
    if (c == '}') {
      return 4;
    }
    if (c == ']') {
      return 3;
    }
    if (c == ',') {
      return 2;
    }
    if (c == ':') {
      return 1;
    }
    if (c == ' ') {
      return 0;
    }
    word += c;
  } else {
    if (c == '\"') {
      instring = 0;
      return 0;
      
    }
    word += c;

  }
  return 0;
}

Word *findWord(string w)
{
  Word *last = g_words;;
  while(last) {
    if (last->getWord() == w) {
      return last;
    } else {
      last = last->getNext();
    }
  }
  return NULL;
    
}

string getWordIdx(int idx)
{
  Word *last = g_words;;
  while(last) {
    if (last->getIdx() == idx) {
      return last->getWord();
    } else {
      last = last->getNext();
    }
  }
  return "";
}

int findWordIdx(string w)
{
  Word *last = findWord(w);
  while(last) {
    if (last->getWord() == w) {
      return last->getIdx();
    } else {
      last = last->getNext();
    }
  }
  return -1;
    
}

void addWord(Word * w)
{
  string ws = w->getWord();
  Word *last = findWord(ws);
  if(last) {
    last->incUsed();
    return;
  }
  last = g_words;
  if ( last == NULL) {
    g_words =  w;
    return;
  }
  while(last->getNext()) last = last->getNext();
  last->setNext(w);
}



string Word::showWord()
{
  string res;
  char tmp [128];
  res = " word ["+ word +"]";
  snprintf(tmp, sizeof(tmp)," index : %d",idx);
  res += tmp;
  snprintf(tmp, sizeof(tmp)," used : %d",used);
  res += tmp;
  return res;
}


int newWord(string ws)
{
  char c;
  int rc;
  
  Word * w = findWord(ws);
  if(w) {
    w->incUsed();
    return 0;
  }
  w = new Word ();
  for (int i = 0; i < ws.length(); i++) {
    rc = w->addChar(ws[i]);
    if(rc > 0) break;
  }
  addWord(w);
  return rc;
}

string newWord()
{
  string word;
  cout << " Enter word" << endl;
  cin >> word;
  newWord(word);
  return word;
}

Parser * g_p = NULL;

string decodeParser()
{
  Parser * p;
  if (g_p == NULL)
    g_p = new Parser;
  p = g_p;
  string res = p->decode();
  return res;
}

string testParser(int id)
{
  string word;
  Parser * p;
  if (g_p == NULL)
    g_p = new Parser;
  p = g_p;

  p->clearOutput();
  
  cout << " Enter line" << endl;
  getline(cin,word);
  if(word.length() < 2) {
    getline(cin,word);
  }
  for (int i = 0; i < word.length(); i++) {
    if(id == 0) {
      p->addChar(word[i]);
    } else if(id == 1) {
      p->addChar1(word[i]);
    }
  }
  // terminate
  if(id == 0){
    p->addChar(0);
  }
  else if(id == 1){
    p->addChar1(0);
  }
  word = p->getString();

  //delete p;
  return word;
}



string showWords()
{
  string res="";
  Word *word = g_words;
  while (word) {
    res += word->showWord();
    res += "\n";
    word = word->getNext();
  }
  return res;
}

string setupWords()
{
  string res = "";
  newWord("one,");
  newWord("two:");
  newWord("\"three with space\"");
  newWord("one,");
  res = showWords();
  return res;
}


int done  = 0;

string  menuHelp()
{
  string res = " Hello Menu help:\n";
  res += " <?> show help\n";
  res +=" <q> quit\n";
  res += " <t> setup test list of words\n";
  res +=" <(w)ord> create new word\n";
  res += " <(s)how_words> show list of words\n";
  res += " <p1> test parser 1 \n";
  res += " <p> test parser \n";
  res += " <d> decode parser \n";
  res += " <dl> decode line \n";
  return res;
}

string menu( string input)
{
  string res;
  //int done = 0;

    
    cout << " You entered [" << input <<"]"<< endl;
    if (input == "?") res = menuHelp();
    else if (input == "q") {res = " quitting\n"; done = 1; }
    else if (input == "w") res = newWord();
    else if (input == "word") res = newWord();
    else if (input == "s") res = showWords();
    else if (input == "show_words") res = showWords();
    else if (input == "t") res = setupWords();
    else if (input == "p1") res = testParser(1);
    else if (input == "p") res = testParser(0);
    else if (input == "d") res = decodeParser();
    else res = " unknown command [" + input + "]\n";
    //}
    //cout << " quitting " << endl;
    return res;
}


int main() {
  string res;
  string input;
  while (!done) {
    cout << " Hello Menu <?= help>" << endl;
    cin >> input;
    res = menu(input);
    cout << res << endl;
  }
  return 0;
}
