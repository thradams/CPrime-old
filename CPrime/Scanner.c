#include "Scanner.h"
#include "Path.h"
#include "Macro.h"
#include "Parser.h"
#include <stdarg.h>

BasicScanner* Scanner_Top(Scanner* pScanner);

void Scanner_MatchDontExpand(Scanner* pScanner);

PPTokenType TokenToPPToken(Tokens token)
{
  PPTokenType result = PPTokenType_Other;

  switch (token)
  {
      case TK_AUTO:
      case    TK__AUTO:
      case TK_BREAK:
      case TK_CASE:
      case     TK_char:
      case TK_CONST:
      case TK_CONTINUE:
      case TK_DEFAULT:
      case TK__DEFAULT:
      case TK__DEFVAL:
      case TK_DO:
      case TK_DOUBLE:
      case TK_ELSE:
      case TK_ENUM:
      case TK_EXTERN:
      case TK_FLOAT:
      case TK_FOR:
      case TK_GOTO:
      case TK_IF:
      case TK_INT:
      case TK_LONG:
          ////////////////
          //Microsoft - specific
      case TK__INT8:
      case TK__INT16:
      case TK__INT32:
      case TK__INT64:
      case TK__WCHAR_T:
          ////////////////
      case TK_REGISTER:
      case TK_RETURN:
      case TK_SHORT:
      case TK_SIGNED:
      case TK_SIZEOF:
      case TK_STATIC:
      case TK_STRUCT:
      case TK_SWITCH:
      case TK_TYPEDEF:
      case  TK_UNION:
      case TK_UNSIGNED:
      case TK_VOID:
      case TK_VOLATILE:
      case TK_WHILE:
      case TK__THREAD_LOCAL:
      case TK__BOOL:
      case TK__COMPLEX:
      case TK__ATOMIC:
      case TK_RESTRICT:
      case TK__STATIC_ASSERT:
      case TK_INLINE:
      case TK__INLINE://ms
      case TK__FORCEINLINE: //ms
      case TK__NORETURN:
      case TK__ALIGNAS:
      case TK__GENERIC:
      case  TK__IMAGINARY:
      case TK__ALINGOF:
    case TK_IDENTIFIER:
      result = PPTokenType_Identifier;
      break;

    case TK_LINE_COMMENT:
    case TK_COMMENT:
    case TK_SPACES:
      result = PPTokenType_Spaces;
      break;

    case TK_HEX_INTEGER:
    case TK_DECIMAL_INTEGER:
    case TK_FLOAT_NUMBER:
      result = PPTokenType_Number;
      break;

    case TK_CHAR_LITERAL:
      result = PPTokenType_CharConstant;
      break;

    case TK_STRING_LITERAL:
      result = PPTokenType_StringLiteral;
      break;

    case TK_ARROW:
    case TK_PLUSPLUS:
    case TK_MINUSMINUS:
    case TK_LESSLESS:
    case TK_GREATERGREATER:
    case TK_LESSEQUAL:
    case TK_GREATEREQUAL:
    case TK_EQUALEQUAL:
    case TK_NOTEQUAL:
    case TK_ANDAND:
    case TK_OROR:
    case TK_MULTIEQUAL:
    case TK_DIVEQUAL:
    case TK_PERCENT_EQUAL:
    case TK_PLUSEQUAL:
    case TK_MINUS_EQUAL:
    case TK_ANDEQUAL:
    case TK_CARETEQUAL:
    case TK_OREQUAL:
    case TK_NUMBERNUMBER:
    case TK_LESSCOLON:
    case TK_COLONGREATER:
    case TK_LESSPERCENT:
    case TK_PERCENTGREATER:
    case TK_PERCENTCOLON:
    case TK_DOTDOTDOT:
    case TK_GREATERGREATEREQUAL:
    case TK_LESSLESSEQUAL:
    case TK_PERCENTCOLONPERCENTCOLON:

    case     TK_EXCLAMATION_MARK:// = '!';
    case    TK_QUOTATION_MARK:// = '\"';
    case    TK_NUMBER_SIGN:// = '#';

    case    TK_DOLLAR_SIGN:// = '$';
    case     TK_PERCENT_SIGN:// = '%';
    case    TK_AMPERSAND:// = '&';
    case     TK_APOSTROPHE:// = '\'';
    case    TK_LEFT_PARENTHESIS:// = '(';
    case    TK_RIGHT_PARENTHESIS:// = ')';
    case    TK_ASTERISK:// = '*';
    case    TK_PLUS_SIGN:// = '+';
    case    TK_COMMA:// = ':';
    case    TK_HYPHEN_MINUS:// = '-';
    case    TK_HYPHEN_MINUS_NEG:// = '-'; //nao retorna no basic string mas eh usado para saber se eh - unario
    case    TK_FULL_STOP:// = '.';
    case    TK_SOLIDUS:// = '/';

    case    TK_COLON:// = ':';
    case    TK_SEMICOLON:// = ';';
    case    TK_LESS_THAN_SIGN:// = '<';
    case    TK_EQUALS_SIGN:// = '=';
    case    TK_GREATER_THAN_SIGN:// = '>';
    case    TK_QUESTION_MARK:// = '\?';
    case    TK_COMMERCIAL_AT:// = '@';
        
    case     TK_LEFT_SQUARE_BRACKET:// = '[';
    case    REVERSE_SOLIDUS:// = '\\';
    case     TK_RIGHT_SQUARE_BRACKET:// = ']';
    case    TK_CIRCUMFLEX_ACCENT:// = '^';
    case    TK_LOW_LINE:// = '_';
    case    TK_GRAVE_ACCENT:// = '`';

    case    TK_LEFT_CURLY_BRACKET:// = '{';
    case    TK_VERTICAL_LINE:// = '|';
    case    TK_RIGHT_CURLY_BRACKET:// = '}';
    case    TK_TILDE: // ~

      result = PPTokenType_Punctuator;
      break;
    default:
        ASSERT(false);
        result = PPTokenType_Punctuator;
        break;
  }

  return result;
}

TFile* TFile_Create() _default
{
    TFile *p = (TFile*) malloc(sizeof * p);
    if (p != NULL) {
        String_Init(&p->FullPath);
        String_Init(&p->IncludePath);
        p->FileIndex = 0;
        p->PragmaOnce = false;
        p->bDirectInclude = false;
        p->bSystemLikeInclude = false;
    }
    return p;
}

void TFile_Destroy(TFile* p) _default
{
    String_Destroy(&p->FullPath);
    String_Destroy(&p->IncludePath);
}

void TFile_Delete(TFile* p) _default
{
    if (p != NULL) {
        TFile_Destroy(p);
        free(p);
    }
}

void TFile_DeleteVoid(void* p)
{
  TFile_Delete((TFile*)p);
}

void TFileMap_Destroy(TFileMap* p)
{
  Map_Destroy(p, TFile_DeleteVoid);
}

Result TFileMap_Set(TFileMap* map, const char* key, TFile* pFile)
{
  // tem que ser case insensitive!
  ASSERT(IsFullPath(key));
  // converter
  // Ajusta o file index de acordo com a entrada dele no mapa
  pFile->FileIndex = map->Size;
  Result result = Map_Set(map, key, pFile);
  String_Set(&pFile->FullPath, key);
  return result;
}

TFile* TFileMap_Find(TFileMap* map, const char* key)
{
  // tem que ser case insensitive!
  return (TFile*)Map_Find2(map, key);
}

Result TFileMap_DeleteItem(TFileMap* map, const char* key)
{
  return Map_DeleteItem(map, key, TFile_DeleteVoid);
}

typedef enum
{
  NONE, // inclui
  I1,   // inclui
  I0,
  E0,
  E1, // inclui
} State;

bool IsIncludeState(State e)
{
  return e == NONE || e == I1 || e == E1;
}

State StateTop(Scanner* pScanner)
{
  if (pScanner->StackIfDef.size == 0)
    return NONE;

  return (State)ArrayInt_Top(&pScanner->StackIfDef);
}

void StatePush(Scanner* pScanner, State s)
{
  ArrayInt_Push(&pScanner->StackIfDef, s);
}

void StatePop(Scanner* pScanner)
{
  ArrayInt_Pop(&pScanner->StackIfDef);
}

void Scanner_GetError(Scanner* pScanner, StrBuilder* str)
{
  StrBuilder_Append(str, pScanner->DebugString.c_str);
  StrBuilder_Append(str, "\n");

  ForEachBasicScanner(p, pScanner->stack)
  {
    StrBuilder_AppendFmt(str, "%s(%d)\n",
        p->stream.NameOrFullPath, 
        p->stream.currentLine);        
  }
}


void Scanner_GetFilePositionString(Scanner* pScanner, StrBuilder* sb)
{
    if (Scanner_Top(pScanner) != NULL)
    {
        StrBuilder_Set(sb,
            Scanner_Top(pScanner)->stream.NameOrFullPath);
    }
    
    if (Scanner_Top(pScanner))
    {
        StrBuilder_AppendFmt(sb, "(%d): ", Scanner_Top(pScanner)->stream.currentLine);
    }
    else
    {
        StrBuilder_Append(sb, "(1): ");
    }
}

void Scanner_SetError(Scanner* pScanner, const char* fmt, ...)
{
  if (!pScanner->bError)
  {
    pScanner->bError = true;

    if (Scanner_Top(pScanner))
    {
        StrBuilder_AppendFmt(&pScanner->ErrorString, "%s(%d) :",
            Scanner_Top(pScanner)->stream.NameOrFullPath,
            Scanner_Top(pScanner)->stream.currentLine);      
    }
    else
    {
        StrBuilder_Append(&pScanner->ErrorString, "(0) :");
    }
    
    
    va_list args;
    va_start(args, fmt);
    StrBuilder_AppendFmtV(&pScanner->ErrorString, fmt, args);
    va_end(args);
  }
}

void Scanner_PrintDebug(Scanner* pScanner)
{
  printf("\ndebug---\n");

  ForEachBasicScanner(p, pScanner->stack)
  {
    printf("stream %s\n", p->stream.NameOrFullPath);
    printf("line, col = %d %d\n", p->stream.currentLine, p->stream.currentCol);
  }

  printf("---\n");
}

static Result AddStandardMacro(Scanner* pScanner, const char* name,
                               const char* value)
{
  Macro* pDefine1 = Macro_Create();
  String_Set(&pDefine1->Name, name);
  // TODO tipo do token
  TokenArray_Push(&pDefine1->TokenSequence,
                  PPToken_Create(value, PPTokenType_Other));
  pDefine1->FileIndex = 0;
  MacroMap_SetAt(&pScanner->Defines2, name, pDefine1);
  return RESULT_OK;
}
static void Scanner_PushToken(Scanner* pScanner, Tokens token,
                              const char* lexeme, bool bActive);

static Result Scanner_InitCore(Scanner* pScanner)
{
  List_Init(&pScanner->AcumulatedTokens);

  // TFileMap_init
  // pScanner->IncludeDir
  Map_Init(&pScanner->FilesIncluded, 100);
  MacroMap_Init(&pScanner->Defines2);
  StrBuilder_Init(&pScanner->DebugString, 100);
  // StrBuilder_Init(&pScanner->PreprocessorAndCommentsString, 100);
  StrBuilder_Init(&pScanner->ErrorString, 100);

  pScanner->bError = false;
  ArrayInt_Init(&pScanner->StackIfDef);

  BasicScannerStack_Init(&pScanner->stack);

  StrArray_Init(&pScanner->IncludeDir);

  // Indica que foi feita uma leitura especulativa
  // pScanner->bHasLookAhead = false;
  //  pScanner->pLookAheadPreviousScanner = NULL;

  // Valor lido na leitura especulativa
  // ScannerItem_Init(&pScanner->LookAhead);
  //__FILE__ __LINE__ __DATE__ __STDC__  __STD_HOSTED__  __TIME__
  //__STD_VERSION__
  //
  AddStandardMacro(pScanner, "__LINE__", "0");
  AddStandardMacro(pScanner, "__FILE__", "\"__FILE__\"");
  AddStandardMacro(pScanner, "__DATE__", "\"__DATE__\"");
  AddStandardMacro(pScanner, "__TIME__", "\"__TIME__\"");
  AddStandardMacro(pScanner, "__STDC__", "1");
  AddStandardMacro(pScanner, "__COUNTER__", "0");
  // AddStandardMacro(pScanner, "__STD_HOSTED__", "1");

  Scanner_PushToken(pScanner, TK_BOF, "", true);

  return RESULT_OK;
}

Result Scanner_InitString(Scanner* pScanner, const char* name,
                          const char* text)
{
  Scanner_InitCore(pScanner);

  BasicScanner* pNewScanner;
  Result result =
    BasicScanner_Create(&pNewScanner, name, text, BasicScannerType_Macro);
  BasicScannerStack_Push(&pScanner->stack, pNewScanner);
  return result;
}

Result PushExpandedMacro(Scanner* pScanner,
    
                         const char* callString, 
    const char* defineContent)
{
  if (pScanner->bError)
  {
    return RESULT_FAIL;
  }

  BasicScanner* pNewScanner;
  Result result = BasicScanner_Create(&pNewScanner, callString, /*defineName*/
                                      defineContent, BasicScannerType_Macro);

  if (result == RESULT_OK)
  {
    pNewScanner->bMacroExpanded = true;
    BasicScanner_Match(pNewScanner); // inicia
    BasicScannerStack_Push(&pScanner->stack, pNewScanner);
  }

  return result;
}

bool Scanner_GetFullPath(Scanner* pScanner, const char* fileName,
                         bool bQuotedForm, String* fullPathOut)
{
  if (pScanner->bError)
  {
    return false;
  }

  bool bFullPathFound = false;

  // https://msdn.microsoft.com/en-us/library/36k2cdd4.aspx
  /*
  bQuotedForm
  The preprocessor searches for include files in this order:
  1) In the same directory as the file that contains the #include statement.
  2) In the directories of the currently opened include files, in the reverse
  order in which they were opened. The search begins in the directory of the
  parent include file and continues upward through the directories of any
  grandparent include files. 3) Along the path that's specified by each /I
  compiler option. 4) Along the paths that are specified by the INCLUDE
  environment variable.
  */
  if (bQuotedForm)
  {
    // String s = STRING_INIT;
    // GetFullPath(fileName, &s);
    // String_Destroy(&s);
    if (IsFullPath(fileName))
    {
      // Se ja vier com fullpath?? este caso esta cobrindo
      // mas deve ter uma maneira melhor de verificar se eh um fullpath ja
      bFullPathFound = true;
      String_Set(fullPathOut, fileName);
    }
    else
    {
      if (pScanner->stack != NULL)
      {
        // tenta nos diretorios ja abertos
        StrBuilder path = STRBUILDER_INIT;

        // for (int i = (int)pScanner->stack.size - 1; i >= 0; i--)
        ForEachBasicScanner(p, pScanner->stack)
        {
          // BasicScanner* p = (BasicScanner*)pScanner->stack.pItems[i];
          StrBuilder_Set(&path, p->stream.FullDir2);
          StrBuilder_Append(&path, fileName);
          bool bFileExists = FileExists(path.c_str);

          if (bFileExists)
          {
            GetFullPath(path.c_str, fullPathOut);
            // String_Set(fullPathOut, StrBuilder_Release(&path));
            bFullPathFound = true;
            break;
          }
        }

        StrBuilder_Destroy(&path);
      }
      else
      {
        // nao abriu nenhum, faz o full path do nome do arquivo
        String fullPath;
        String_InitWith(&fullPath, NULL);
        GetFullPath(fileName, &fullPath);
        bool bFileExists = FileExists(fullPath);

        if (bFileExists)
        {
          String_Swap(&fullPath, fullPathOut);
          bFullPathFound = true;
        }

        String_Destroy(&fullPath);
      }
    }
  }

  /*
  Angle-bracket form
  The preprocessor searches for include files in this order:
  1) Along the path that's specified by each /I compiler option.
  2) When compiling occurs on the command line, along the paths that are
  specified by the INCLUDE environment variable.
  */
  if (!bFullPathFound)
  {
    StrBuilder path = STRBUILDER_INIT;

    // StrBuilder_Init(&path, 200);
    for (int i = 0; i < pScanner->IncludeDir.size; i++)
    {
      const char* pItem = pScanner->IncludeDir.pItems[i];
      StrBuilder_Set(&path, pItem);
      
      //barra para o outro lado funciona
      //windows e linux
      StrBuilder_Append(&path, "/");

      StrBuilder_Append(&path, fileName);
      bool bFileExists = FileExists(path.c_str);

      if (bFileExists)
      {
        String_Set(fullPathOut, StrBuilder_Release(&path));
        bFullPathFound = true;
        break;
      }
    }

    StrBuilder_Destroy(&path);
  }

  return bFullPathFound;
}

void Scanner_IncludeFile(Scanner* pScanner, const char* includeFileName,
                         FileIncludeType fileIncludeType, bool bSkipBof)
{
  if (pScanner->bError)
  {
    return;
  }

  bool bDirectInclude = false;

  String fullPath = STRING_INIT;
  String_InitWith(&fullPath, "");
  // Faz a procura nos diretorios como se tivesse adicinando o include
  // seguindo as mesmas regras. Caso o include seja possivel ele retorna o path
  // completo  este path completo que eh usado para efeitos do pragma once.
  bool bHasFullPath = false;

  switch (fileIncludeType)
  {
    case FileIncludeTypeQuoted:
    case FileIncludeTypeIncludes:
      bHasFullPath = Scanner_GetFullPath(pScanner, includeFileName,
                                         fileIncludeType == FileIncludeTypeQuoted,
                                         &fullPath);
      break;

    case FileIncludeTypeFullPath:
      String_Set(&fullPath, includeFileName);
      bHasFullPath = true;
      break;
  };

  if (bHasFullPath)
  {
    TFile* pFile = TFileMap_Find(&pScanner->FilesIncluded, fullPath);

    if (pFile != NULL && pFile->PragmaOnce)
    {
      // foi marcado como pragma once.. nao faz nada
      // tenho q enviar um comando
      Scanner_PushToken(pScanner, TK_FILE_EOF, "pragma once file", true);
    }
    else
    {
      if (pFile == NULL)
      {
        pFile = TFile_Create();
        pFile->bDirectInclude = bDirectInclude;
        pFile->bSystemLikeInclude =
          (fileIncludeType == FileIncludeTypeIncludes);
        String_Set(&pFile->IncludePath, includeFileName);
        TFileMap_Set(&pScanner->FilesIncluded, fullPath, pFile); // pfile Moved
      }

      BasicScanner* pNewScanner = NULL;
      Result result = BasicScanner_CreateFile(fullPath, &pNewScanner);

      if (result == RESULT_OK)
      {
        if (pFile)
        {
          pNewScanner->FileIndex = pFile->FileIndex;

          if (bSkipBof)
          {
            BasicScanner_Match(pNewScanner);
          }

          BasicScannerStack_Push(&pScanner->stack, pNewScanner);
        }
        else
        {
          Scanner_SetError(pScanner, "mem");
        }
      }
      else
      {
        Scanner_SetError(pScanner, "Cannot open source file: '%s': No such file or directory", fullPath);        
      }
    }
  }
  else
  {
    Scanner_SetError(pScanner, "Cannot open include file: '%s': No such file or directory", includeFileName);    
  }

  String_Destroy(&fullPath);
}

const char* Scanner_GetStreamName(Scanner* pScanner)
{
  const char* streamName = NULL;

  BasicScanner* p = Scanner_Top(pScanner);
  streamName = p ? p->stream.NameOrFullPath : NULL;

  return streamName;
}

Result Scanner_Init(Scanner* pScanner)
{
  return Scanner_InitCore(pScanner);
}

void Scanner_Destroy(Scanner* pScanner) _default
{
    BasicScannerStack_Destroy(&pScanner->stack);
    MacroMap_Destroy(&pScanner->Defines2);
    ArrayInt_Destroy(&pScanner->StackIfDef);
    TFileMap_Destroy(&pScanner->FilesIncluded);
    StrArray_Destroy(&pScanner->IncludeDir);
    StrBuilder_Destroy(&pScanner->DebugString);
    StrBuilder_Destroy(&pScanner->ErrorString);
}

/*int Scanner_GetCurrentLine(Scanner* pScanner)
{
    ASSERT(!pScanner->bHasLookAhead);

    if (pScanner->bError)
    {
        return -1;
    }


    int currentLine = -1;
    int fileIndex = -1;


    ForEachBasicScanner(pBasicScanner, pScanner->stack)
    {
        fileIndex = pBasicScanner->FileIndex;

        if (fileIndex >= 0) //macro eh -1
        {
            currentLine = pBasicScanner->stream.currentLine;
            break;
        }
    }

    return currentLine;
}
*/

int Scanner_GetFileIndex(Scanner* pScanner)
{
  if (pScanner->bError)
  {
    return -1;
  }

  int fileIndex = -1;

  ForEachBasicScanner(pBasicScanner, pScanner->stack)
  {
    fileIndex = pBasicScanner->FileIndex;

    if (fileIndex >= 0)
    {
      break;
    }
  }

  // ASSERT(fileIndex >= 0);
  return fileIndex;
}

/*const char* Scanner_LexemeAt(Scanner* pScanner)
{
    if (pScanner->bHasLookAhead)
    {
        return pScanner->LookAhead.lexeme.c_str;
    }
    else
    {
        BasicScanner* pBasicScanner = pScanner->stack;

        return pBasicScanner ?
            pBasicScanner->currentItem.lexeme.c_str :
            "";
    }
}*/

BasicScanner* Scanner_Top(Scanner* pScanner)
{
  return pScanner->stack;
}

// int Scanner_Line(Scanner* pScanner)
//{
//  ASSERT(!pScanner->bHasLookAhead);
// return Scanner_Top(pScanner)->stream.currentLine;
//}

// int Scanner_Col(Scanner* pScanner)
//{
//  ASSERT(!pScanner->bHasLookAhead);
//    return Scanner_Top(pScanner)->stream.currentCol;
//}

void IgnorePreProcessorv2(BasicScanner* pBasicScanner, StrBuilder* strBuilder)
{

  while (pBasicScanner->currentItem.token != TK_EOF &&
         pBasicScanner->currentItem.token != TK_FILE_EOF)
  {
    if (pBasicScanner->currentItem.token == TK_BREAKLINE)
    {
      // StrBuilder_Append(strBuilder, pTop->currentItem.lexeme.c_str);
      BasicScanner_Match(pBasicScanner);
      break;
    }
    StrBuilder_Append(strBuilder, pBasicScanner->currentItem.lexeme.c_str);
    BasicScanner_Match(pBasicScanner);
  }
}

void GetDefineString(Scanner* pScanner, StrBuilder* strBuilder)
{
  for (;;)
  {
    Tokens token = Scanner_Top(pScanner)->currentItem.token;

    if (token == TK_EOF)
    {
      break;
    }

    if (token == TK_BREAKLINE)
    {
      // deixa o break line
      // BasicScanner_Match(Scanner_Top(pScanner));
      break;
    }

    if (token == TK_COMMENT || token == TK_LINE_COMMENT)
    {
      // transforma em espaços
      StrBuilder_Append(strBuilder, " ");
    }
    else
    {
      StrBuilder_Append(strBuilder, BasicScanner_Lexeme(Scanner_Top(pScanner)));
    }

    BasicScanner_Match(Scanner_Top(pScanner));
  }
}

Macro* Scanner_FindPreprocessorItem2(Scanner* pScanner, const char* key)
{
  Macro* pMacro = MacroMap_Find(&pScanner->Defines2, key);
  return pMacro;
}

bool Scanner_IsLexeme(Scanner* pScanner, const char* psz)
{
  return BasicScanner_IsLexeme(Scanner_Top(pScanner), psz);
}

int PreprocessorExpression(Parser* parser)
{
  // Faz o parser da expressão
  TExpression* pExpression = NULL;
  ConstantExpression(parser, &pExpression);
  //..a partir da arvore da expressão
  // calcula o valor
  // TODO pegar error
  int r;
  if (!EvaluateConstantExpression(pExpression, &r))
  {
    Scanner_SetError(&parser->Scanner, "error evaluating expression");
  }

  TExpression_Delete(pExpression);
  return r;
}

int EvalExpression(const char* s, Scanner* pScanner)
{
  MacroMap* pDefines = &pScanner->Defines2;
  // printf("%s = ", s);
  // TODO avaliador de expressoes para pre processador
  // https://gcc.gnu.org/onlinedocs/gcc-3.0.2/cpp_4.html#SEC38
  Parser parser;
  Parser_InitString(&parser, "eval expression", s);
  parser.bPreprocessorEvalFlag = true;

  if (pDefines)
  {
    // usa o mapa de macros para o pre-processador
    MacroMap_Swap(&parser.Scanner.Defines2, pDefines);
  }

  //    Scanner_Match(&parser.Scanner);
  int iRes = PreprocessorExpression(&parser);

  // printf(" %d\n", iRes);
  if (pDefines)
  {
    MacroMap_Swap(&parser.Scanner.Defines2, pDefines);
  }

  if (parser.bError)
  {
    Scanner_SetError(pScanner, parser.ErrorMessage.c_str);
  }

  if (parser.Scanner.bError)
  {
    Scanner_SetError(pScanner, parser.Scanner.ErrorString.c_str);
  }

  Parser_Destroy(&parser);
  return iRes;
}

static void GetMacroArguments(Scanner* pScanner, BasicScanner* pBasicScanner,
                              Macro* pMacro, TokenArray* ppTokenArray,
                              StrBuilder* strBuilder)
{
  // StrBuilder_Append(strBuilderResult, Scanner_LexemeAt(pScanner));
  // TODO aqui nao pode ser o current
  const char* lexeme = pBasicScanner->currentItem.lexeme.c_str;
  Tokens token = pBasicScanner->currentItem.token;

  // verificar se tem parametros
  int nArgsExpected = pMacro->FormalArguments.Size; // pMacro->bIsFunction;
  int nArgsFound = 0;

  // fazer uma lista com os parametros

  if (token == TK_LEFT_PARENTHESIS)
  {
    // Adiciona o nome da macro
    PPToken* ppTokenName = PPToken_Create(pMacro->Name, PPTokenType_Identifier);
    TokenArray_Push(ppTokenArray, ppTokenName);

    // Match do (
    PPToken* ppToken = PPToken_Create(lexeme, TokenToPPToken(token));
    TokenArray_Push(ppTokenArray, ppToken);

    StrBuilder_Append(strBuilder, lexeme);
    BasicScanner_Match(pBasicScanner);

    token = pBasicScanner->currentItem.token;
    lexeme = pBasicScanner->currentItem.lexeme.c_str;

    // comeca com 1
    nArgsFound = 1;
    int iInsideParentesis = 1;

    for (;;)
    {
      if (token == TK_LEFT_PARENTHESIS)
      {

        PPToken* ppToken = PPToken_Create(lexeme, TokenToPPToken(token));

        TokenArray_Push(ppTokenArray, ppToken);

        StrBuilder_Append(strBuilder, lexeme);
        BasicScanner_Match(pBasicScanner);

        token = pBasicScanner->currentItem.token;
        lexeme = pBasicScanner->currentItem.lexeme.c_str;

        iInsideParentesis++;
      }
      else if (token == TK_RIGHT_PARENTHESIS)
      {
        if (iInsideParentesis == 1)
        {
          PPToken* ppToken = PPToken_Create(lexeme, TokenToPPToken(token));
          TokenArray_Push(ppTokenArray, ppToken);

          StrBuilder_Append(strBuilder, lexeme);
          BasicScanner_Match(pBasicScanner);

          token = pBasicScanner->currentItem.token;
          lexeme = pBasicScanner->currentItem.lexeme.c_str;

          break;
        }

        iInsideParentesis--;

        PPToken* ppToken = PPToken_Create(lexeme, TokenToPPToken(token));
        TokenArray_Push(ppTokenArray, ppToken);

        StrBuilder_Append(strBuilder, lexeme);
        BasicScanner_Match(pBasicScanner);

        token = pBasicScanner->currentItem.token;
        lexeme = pBasicScanner->currentItem.lexeme.c_str;
      }
      else if (token == TK_COMMA)
      {
        if (iInsideParentesis == 1)
        {
          nArgsFound++;
        }
        else
        {
          // continuar...
        }

        // StrBuilder_Append(strBuilderResult, Scanner_LexemeAt(pScanner));
        PPToken* ppToken = PPToken_Create(lexeme, TokenToPPToken(token));
        TokenArray_Push(ppTokenArray, ppToken);

        StrBuilder_Append(strBuilder, lexeme);
        BasicScanner_Match(pBasicScanner);
        token = pBasicScanner->currentItem.token;
        lexeme = pBasicScanner->currentItem.lexeme.c_str;
      }
      else
      {
        // StrBuilder_Append(strBuilderResult, Scanner_LexemeAt(pScanner));
        PPToken* ppToken = PPToken_Create(lexeme, TokenToPPToken(token));
        TokenArray_Push(ppTokenArray, ppToken);

        StrBuilder_Append(strBuilder, lexeme);
        BasicScanner_Match(pBasicScanner);
        token = pBasicScanner->currentItem.token;
        lexeme = pBasicScanner->currentItem.lexeme.c_str;
      }
    }
  }

  if (nArgsExpected != nArgsFound)
  {
    if (nArgsFound == 0 && nArgsExpected > 0)
    {
      // erro
    }
    else
    {
      if (nArgsExpected > nArgsFound)
      {
        // Scanner_SetError(pScanner, "Illegal macro call. Too few arguments
        // error");
      }
      else
      {
        // Scanner_SetError(pScanner, "Illegal macro call. Too many arguments
        // error.");
      }
      ASSERT(false);
      // JObj_PrintDebug(pMacro);
      // Scanner_PrintDebug(pScanner);
    }
  }

  // tODO se nao for macro tem que pegar todo
  // o match feito e devolver.
  // nome da macro e espacos..

  // return false;
}

Tokens FindPreToken(const char* lexeme)
{
  Tokens token = TK_NONE;
  if (strcmp(lexeme, "include") == 0)
  {
    token = TK_PRE_INCLUDE;
  }
  else if (strcmp(lexeme, "pragma") == 0)
  {
    token = TK_PRE_PRAGMA;
  }
  else if (strcmp(lexeme, "if") == 0)
  {
    token = TK_PRE_IF;
  }
  else if (strcmp(lexeme, "elif") == 0)
  {
    token = TK_PRE_ELIF;
  }
  else if (strcmp(lexeme, "ifndef") == 0)
  {
    token = TK_PRE_IFNDEF;
  }
  else if (strcmp(lexeme, "ifdef") == 0)
  {
    token = TK_PRE_IFDEF;
  }
  else if (strcmp(lexeme, "endif") == 0)
  {
    token = TK_PRE_ENDIF;
  }
  else if (strcmp(lexeme, "else") == 0)
  {
    token = TK_PRE_ELSE;
  }
  else if (strcmp(lexeme, "error") == 0)
  {
    token = TK_PRE_ERROR;
  }
  else if (strcmp(lexeme, "line") == 0)
  {
    token = TK_PRE_LINE;
  }
  else if (strcmp(lexeme, "undef") == 0)
  {
    token = TK_PRE_UNDEF;
  }
  else if (strcmp(lexeme, "define") == 0)
  {
    token = TK_PRE_DEFINE;
  }
  return token;
}

void GetPPTokens(BasicScanner* pBasicScanner, TokenArray* pptokens,
                 StrBuilder* strBuilder)
{
  Tokens token = pBasicScanner->currentItem.token;
  const char* lexeme = pBasicScanner->currentItem.lexeme.c_str;

  // Corpo da macro
  while (token != TK_BREAKLINE && token != TK_EOF && token != TK_FILE_EOF)
  {
    StrBuilder_Append(strBuilder, lexeme);

    if (token != TK_BACKSLASHBREAKLINE)
    {
      // TODO comentarios entram como espaco
      PPToken* ppToken = PPToken_Create(lexeme, TokenToPPToken(token));
      TokenArray_Push(pptokens, ppToken);
    }
    BasicScanner_Match(pBasicScanner);
    token = pBasicScanner->currentItem.token;
    lexeme = pBasicScanner->currentItem.lexeme.c_str;
  }

  // Remove os espaços do fim
  while (pptokens->Size > 0 &&
         pptokens->pItems[pptokens->Size - 1]->Token == PPTokenType_Spaces)
  {
    TokenArray_Pop(pptokens);
  }
}

static void Scanner_MatchAllPreprocessorSpaces(BasicScanner* pBasicScanner,
    StrBuilder* strBuilder)
{
  Tokens token = pBasicScanner->currentItem.token;
  while (token == TK_SPACES || token == TK_BACKSLASHBREAKLINE ||
         token == TK_COMMENT)
  {
    StrBuilder_Append(strBuilder, pBasicScanner->currentItem.lexeme.c_str);

    BasicScanner_Match(pBasicScanner);
    token = pBasicScanner->currentItem.token;
  }
}

void ParsePreDefinev2(Scanner* pScanner, StrBuilder* strBuilder)
{
  BasicScanner* pBasicScanner = Scanner_Top(pScanner);

  // objetivo eh montar a macro e colocar no mapa
  Macro* pNewMacro = Macro_Create();

  Tokens token = pBasicScanner->currentItem.token;
  const char* lexeme = pBasicScanner->currentItem.lexeme.c_str;

  String_Set(&pNewMacro->Name, lexeme);
  StrBuilder_Append(strBuilder, lexeme);

  // Match nome da macro
  BasicScanner_Match(pBasicScanner);

  token = pBasicScanner->currentItem.token;
  lexeme = pBasicScanner->currentItem.lexeme.c_str;

  // Se vier ( é macro com parâmetros
  if (token == TK_LEFT_PARENTHESIS)
  {
    pNewMacro->bIsFunction = true;

    StrBuilder_Append(strBuilder, lexeme);

    // Match (
    BasicScanner_Match(pBasicScanner);

    for (;;)
    {
      Scanner_MatchAllPreprocessorSpaces(pBasicScanner, strBuilder);

      token = pBasicScanner->currentItem.token;
      lexeme = pBasicScanner->currentItem.lexeme.c_str;

      if (token == TK_RIGHT_PARENTHESIS)
      {
        // Match )
        StrBuilder_Append(strBuilder, lexeme);
        BasicScanner_Match(pBasicScanner);
        break;
      }

      if (token == TK_BREAKLINE || token == TK_EOF)
      {
        // oopss
        break;
      }

      token = pBasicScanner->currentItem.token;
      lexeme = pBasicScanner->currentItem.lexeme.c_str;

      PPToken* ppToken = PPToken_Create(lexeme, TokenToPPToken(token));
      TokenArray_Push(&pNewMacro->FormalArguments, ppToken);

      StrBuilder_Append(strBuilder, lexeme);
      BasicScanner_Match(pBasicScanner);

      Scanner_MatchAllPreprocessorSpaces(pBasicScanner, strBuilder);

      token = pBasicScanner->currentItem.token;
      lexeme = pBasicScanner->currentItem.lexeme.c_str;

      if (token == TK_COMMA)
      {
        // Match ,
        StrBuilder_Append(strBuilder, lexeme);
        BasicScanner_Match(pBasicScanner);
        // tem mais
      }
    }
  }
  else
  {
    Scanner_MatchAllPreprocessorSpaces(pBasicScanner, strBuilder);
  }

  Scanner_MatchAllPreprocessorSpaces(pBasicScanner, strBuilder);

  GetPPTokens(pBasicScanner, &pNewMacro->TokenSequence, strBuilder);

  MacroMap_SetAt(&pScanner->Defines2, pNewMacro->Name, pNewMacro);

  // breakline ficou...
}

int EvalPre(Scanner* pScanner, StrBuilder* sb)
{
  if (pScanner->bError)
  {
    return 0;
  }

  // pega todos os tokens ate o final da linha expande e
  // avalia
  // usado no #if #elif etc.
  BasicScanner* pBasicScanner = Scanner_Top(pScanner);

  TokenArray pptokens = TOKENARRAY_INIT;
  GetPPTokens(pBasicScanner, &pptokens, sb);
  StrBuilder strBuilder = STRBUILDER_INIT;
  ExpandMacroToText(&pptokens, &pScanner->Defines2, false, true, true, NULL, 
                    &strBuilder);

  int iRes = EvalExpression(strBuilder.c_str, pScanner);

  StrBuilder_Destroy(&strBuilder);
  TokenArray_Destroy(&pptokens);
  return iRes;
}

static void Scanner_PushToken(Scanner* pScanner, Tokens token,
                              const char* lexeme, bool bActive)
{
  if (pScanner->bError)
  {
    return;
  }

  ScannerItem* pNew = ScannerItem_Create();
  StrBuilder_Set(&pNew->lexeme, lexeme);
  pNew->token = token;
  pNew->bActive = bActive; //;
  List_Add(&pScanner->AcumulatedTokens, pNew);
}

// Atencao
// Esta funcao eh complicada.
//
// Ela faz uma parte da expansao da macro que pode virar um "tetris"
// aonde o colapso de uma expansao vira outra expansao
// a unica garantia sao os testes.
//
void Scanner_BuyIdentifierThatCanExpandAndCollapse(Scanner* pScanner)
{
  State state = StateTop(pScanner);
  BasicScanner* pBasicScanner = Scanner_Top(pScanner);
  ASSERT(pBasicScanner != NULL);

  Tokens token = pBasicScanner->currentItem.token;
  const char* lexeme = pBasicScanner->currentItem.lexeme.c_str;
  ASSERT(token == TK_IDENTIFIER);

  if (!IsIncludeState(state))
  {
    ScannerItem* pNew = ScannerItem_Create();
    StrBuilder_Swap(&pNew->lexeme, &pBasicScanner->currentItem.lexeme);
    pNew->token = pBasicScanner->currentItem.token;
    pNew->bActive = false;
    List_Add(&pScanner->AcumulatedTokens, pNew);

    // Match do identificador
    BasicScanner_Match(pBasicScanner);
    return;
  }

  Macro* pMacro2 = Scanner_FindPreprocessorItem2(pScanner, lexeme);
  if (pMacro2 == NULL)
  {
    // nao eh macro
    ScannerItem* pNew = ScannerItem_Create();
    StrBuilder_Swap(&pNew->lexeme, &pBasicScanner->currentItem.lexeme);
    pNew->token = pBasicScanner->currentItem.token;
    pNew->bActive = true;
    List_Add(&pScanner->AcumulatedTokens, pNew);

    // Match do identificador
    BasicScanner_Match(pBasicScanner);
    return;
  }

  if (pBasicScanner->bMacroExpanded &&
      strcmp(pMacro2->Name, pBasicScanner->stream.NameOrFullPath) == 0)
  {
    // ja estou expandindo esta mesma macro
    // nao eh macro
    ScannerItem* pNew = ScannerItem_Create();
    StrBuilder_Swap(&pNew->lexeme, &pBasicScanner->currentItem.lexeme);
    pNew->token = pBasicScanner->currentItem.token;
    pNew->bActive = true;
    List_Add(&pScanner->AcumulatedTokens, pNew);

    // Match do identificador
    BasicScanner_Match(pBasicScanner);
    return;
  }

  Macro* pFirstMacro = pMacro2;

  // Match do identificador do nome da macro funcao
  BasicScanner_Match(pBasicScanner);

  bool bExitLoop = false;

  do
  {
    if (!pMacro2->bIsFunction)
    {
      TokenArray ppTokenArray = TOKENARRAY_INIT;
      // o nome eh a propria expansao
      PPToken* ppTokenName =
        PPToken_Create(pMacro2->Name, TokenToPPToken(TK_IDENTIFIER));
      TokenArray_Push(&ppTokenArray, ppTokenName);

      StrBuilder strExpanded = STRBUILDER_INIT;

      ExpandMacroToText(&ppTokenArray, &pScanner->Defines2, false, false, false,NULL,
                        &strExpanded);

      // se expandir para identificador e ele for uma macro do tipo funcao
      // pode ser tetris
      Macro* pMacro3 = NULL;

      if (strExpanded.size > 0)
      {
        pMacro3 = Scanner_FindPreprocessorItem2(pScanner, strExpanded.c_str);
      }

      if (pMacro3)
      {
        if (pMacro3->bIsFunction)
        {
          // Expandiu para uma identificador que é macro funcao
          pMacro2 = pMacro3;
        }
        else
        {
          // ok expandiu

          PushExpandedMacro(pScanner, pMacro2->Name, strExpanded.c_str);

          Scanner_PushToken(pScanner, TK_MACRO_CALL, pMacro2->Name, true);
          bExitLoop = true;

        }
      }
      else
      {
        // ok expandiu

        PushExpandedMacro(pScanner, pMacro2->Name, strExpanded.c_str);

        Scanner_PushToken(pScanner, TK_MACRO_CALL, pMacro2->Name, true);
        bExitLoop = true;
      }

      TokenArray_Destroy(&ppTokenArray);
      StrBuilder_Destroy(&strExpanded);
    }
    else
    {
      //é uma função

      // Procurar pelo (

      TScannerItemList LocalAcumulatedTokens = LIST_INIT;
      token = pBasicScanner->currentItem.token;
      lexeme = pBasicScanner->currentItem.lexeme.c_str;
      while (token == TK_SPACES || token == TK_COMMENT)
      {
        // StrBuilder_Append(strBuilder, lexeme);

        /////////////
        ScannerItem* pNew = ScannerItem_Create();
        StrBuilder_Set(&pNew->lexeme, lexeme);
        pNew->token = token;
        pNew->bActive = true;
        List_Add(&LocalAcumulatedTokens, pNew);
        ////////////

        BasicScanner_Match(pBasicScanner);
        token = pBasicScanner->currentItem.token;
        lexeme = pBasicScanner->currentItem.lexeme.c_str;
      }

      if (token == TK_LEFT_PARENTHESIS)
      {
          StrBuilder strCallString = STRBUILDER_INIT;
        StrBuilder strExpanded = STRBUILDER_INIT;
        TokenArray ppTokenArray = TOKENARRAY_INIT;

        StrBuilder_Set(&strCallString, pFirstMacro->Name);
        // eh uma chamada da macro funcao
        // coletar argumentos e expandir
        GetMacroArguments(pScanner,
                          pBasicScanner,
                          pMacro2,
                          &ppTokenArray,
                          &strCallString);

        ExpandMacroToText(&ppTokenArray,
                          &pScanner->Defines2,
                          false,
                          false,
                          false,
                          NULL,
                          &strExpanded);

        /////////////////////////////////
        // se expandir para identificador e ele for uma macro do tipo funcao
        // pode ser tetris
        Macro* pMacro3 = NULL;
        if (strExpanded.size > 0)
        {
            pMacro3 = Scanner_FindPreprocessorItem2(pScanner, strExpanded.c_str);
        }

        if (pMacro3)
        {
          if (pMacro3->bIsFunction)
          {
            // Expandiu para uma identificador que é macro funcao
            pMacro2 = pMacro3;
          }
          else
          {
            // ok expandiu

            PushExpandedMacro(pScanner, pMacro2->Name, strExpanded.c_str);

            Scanner_PushToken(pScanner, TK_MACRO_CALL, pMacro2->Name, true);
          }
        }
        else
        {
          // ok expandiu

          PushExpandedMacro(pScanner, pMacro2->Name, strExpanded.c_str);

          Scanner_PushToken(pScanner, TK_MACRO_CALL, strCallString.c_str, true);
          bExitLoop = true;
        }
        ///////////////////////
        TokenArray_Destroy(&ppTokenArray);
        StrBuilder_Destroy(&strExpanded);
        StrBuilder_Destroy(&strCallString);
      }
      else
      {
        // macro call
        // B
        // endcall
        // espacos
        if (pFirstMacro != pMacro2)
        {
          // nao era uma chamada da macro funcao
          ScannerItem* pNew = ScannerItem_Create();
          StrBuilder_Append(&pNew->lexeme, pFirstMacro->Name);
          pNew->token = TK_MACRO_CALL;
          pNew->bActive = true;
          List_Add(&pScanner->AcumulatedTokens, pNew);
        }

        ScannerItem* pNew0 = ScannerItem_Create();
        StrBuilder_Append(&pNew0->lexeme, pMacro2->Name);
        pNew0->token = TK_IDENTIFIER;
        pNew0->bActive = true;
        List_Add(&pScanner->AcumulatedTokens, pNew0);

        if (pFirstMacro != pMacro2)
        {
          ScannerItem* pNew2 = ScannerItem_Create();
          pNew2->token = TK_MACRO_EOF;
          pNew2->bActive = true;
          List_Add(&pScanner->AcumulatedTokens, pNew2);
        }

        if (LocalAcumulatedTokens.pHead != NULL)
        {
          List_Add(&pScanner->AcumulatedTokens, LocalAcumulatedTokens.pHead);
          LocalAcumulatedTokens.pHead = NULL;
          LocalAcumulatedTokens.pTail = NULL;
        }
        // tODO espacos

        // a macro eh uma funcao mas isso nao eh a chamada da macro
        // pq nao foi encontrado (
        bExitLoop = true;
      }


      TScannerItemList_Destroy(&LocalAcumulatedTokens);
    }
    if (bExitLoop)
      break;
  }
  while (!bExitLoop);
}

void Scanner_BuyTokens(Scanner* pScanner)
{
  // Sempre compra uma carta nova do monte do baralho.
  // se a carta servir ele coloca na mesa.
  // se comprar uma carta macro expande e coloca em cima
  // do baralho novas cartas sou coloca na mesa
  //(sim eh complicado)

  if (pScanner->bError)
  {
    return;
  }

  BasicScanner* pBasicScanner = Scanner_Top(pScanner);
  if (pBasicScanner == NULL)
  {
    // acabaram todos os montes de tokens (cartas do baralho)
    return;
  }

  Tokens token = pBasicScanner->currentItem.token;
  const char* lexeme = pBasicScanner->currentItem.lexeme.c_str;

  if (token == TK_BOF)
  {
    BasicScanner_Match(pBasicScanner);
    token = pBasicScanner->currentItem.token;
    lexeme = pBasicScanner->currentItem.lexeme.c_str;
  }

  while (token == TK_EOF)
  {
    // ok remove este baralho vazio
    BasicScannerStack_Pop(&pScanner->stack);

    // proximo baralho
    pBasicScanner = Scanner_Top(pScanner);

    if (pBasicScanner != NULL)
    {
      // vai removendo enquanto sao baralhos vazios
      token = pBasicScanner->currentItem.token;
      lexeme = pBasicScanner->currentItem.lexeme.c_str;
    }
    else
    {
      break;
    }
  }

  if (token == TK_FILE_EOF)
  {
    if (pScanner->stack->pPrevious == NULL)
    {
      // se eh o unico arquivo TK_FILE_EOF vira eof
      token = TK_EOF;
    }
  }

  if (token == TK_EOF)
  {
    // não sobrou nenhum baralho nao tem o que comprar
    ScannerItem* pNew = ScannerItem_Create();
    pNew->token = TK_EOF;
    pNew->bActive = true;
    List_Add(&pScanner->AcumulatedTokens, pNew);

    return;
  }

  StrBuilder strBuilder = STRBUILDER_INIT;

  State state = StateTop(pScanner);
  bool bActive0 = IsIncludeState(state);

  if (token == TK_PREPROCESSOR)
  {
    // Match #
    StrBuilder_Append(&strBuilder, pBasicScanner->currentItem.lexeme.c_str);
    BasicScanner_Match(pBasicScanner);

    // Match ' '
    Scanner_MatchAllPreprocessorSpaces(pBasicScanner, &strBuilder);

    lexeme = pBasicScanner->currentItem.lexeme.c_str;
    token = pBasicScanner->currentItem.token;

    Tokens preToken = FindPreToken(lexeme);

    if (preToken == TK_PRE_INCLUDE)
    {
      // Match include
      StrBuilder_Append(&strBuilder, lexeme);
      BasicScanner_Match(pBasicScanner);

      lexeme = pBasicScanner->currentItem.lexeme.c_str;
      token = pBasicScanner->currentItem.token;

      if (IsIncludeState(state))
      {
        // Match espacos
        Scanner_MatchAllPreprocessorSpaces(pBasicScanner, &strBuilder);

        lexeme = pBasicScanner->currentItem.lexeme.c_str;
        token = pBasicScanner->currentItem.token;

        if (token == TK_STRING_LITERAL)
        {
          String fileName;
          String_InitWith(&fileName, lexeme + 1);

          StrBuilder_Append(&strBuilder, lexeme);
          BasicScanner_Match(pBasicScanner);

          fileName[strlen(fileName) - 1] = 0;

          // tem que ser antes de colocar o outro na pilha
          IgnorePreProcessorv2(pBasicScanner, &strBuilder);

          Scanner_PushToken(pScanner, TK_PRE_INCLUDE, strBuilder.c_str, true);
          Scanner_IncludeFile(pScanner, fileName, FileIncludeTypeQuoted, true);
          String_Destroy(&fileName);
          // break;
        }
        else if (token == TK_LESS_THAN_SIGN)
        {
          StrBuilder_Append(&strBuilder, lexeme);

          BasicScanner_Match(pBasicScanner);
          lexeme = pBasicScanner->currentItem.lexeme.c_str;
          token = pBasicScanner->currentItem.token;
          StrBuilder path = STRBUILDER_INIT;

          // StrBuilder_Init(&path, 200);
          for (;;)
          {
            StrBuilder_Append(&strBuilder, lexeme);

            if (token == TK_GREATER_THAN_SIGN)
            {
              BasicScanner_Match(pBasicScanner);
              lexeme = pBasicScanner->currentItem.lexeme.c_str;
              token = pBasicScanner->currentItem.token;
              break;
            }

            if (token == TK_BREAKLINE)
            {
              // oopps
              break;
            }

            StrBuilder_Append(&path, lexeme);
            BasicScanner_Match(pBasicScanner);
            lexeme = pBasicScanner->currentItem.lexeme.c_str;
            token = pBasicScanner->currentItem.token;
          }

          IgnorePreProcessorv2(pBasicScanner, &strBuilder);

          Scanner_PushToken(pScanner, TK_PRE_INCLUDE, strBuilder.c_str, true);
          Scanner_IncludeFile(pScanner, path.c_str, FileIncludeTypeIncludes,
                              true);
          StrBuilder_Destroy(&path);
        }

      }
      else
      {
        // TODO active
        Scanner_PushToken(pScanner, TK_SPACES, strBuilder.c_str, false);
      }
    }
    else if (preToken == TK_PRE_PRAGMA)
    {
      // Match pragma
      StrBuilder_Append(&strBuilder, lexeme);
      BasicScanner_Match(pBasicScanner);

      if (IsIncludeState(state))
      {
        Scanner_MatchAllPreprocessorSpaces(pBasicScanner, &strBuilder);

        if (BasicScanner_IsLexeme(pBasicScanner, "once"))
        {
          const char* fullPath = Scanner_Top(pScanner)->stream.NameOrFullPath;
          TFile* pFile = TFileMap_Find(&pScanner->FilesIncluded, fullPath);

          if (pFile == NULL)
          {
            pFile = TFile_Create();
            pFile->PragmaOnce = true;
            TFileMap_Set(&pScanner->FilesIncluded, fullPath, pFile);
          }
          else
          {
            pFile->PragmaOnce = true;
          }
        }
        else if (BasicScanner_IsLexeme(Scanner_Top(pScanner), "dir"))
        {
          StrBuilder_Append(&strBuilder, lexeme);

          BasicScanner_Match(pBasicScanner);
          Scanner_MatchAllPreprocessorSpaces(pBasicScanner, &strBuilder);
          lexeme = pBasicScanner->currentItem.lexeme.c_str;
          String fileName;
          String_InitWith(&fileName, lexeme + 1);
          Scanner_Match(pScanner);
          fileName[strlen(fileName) - 1] = 0;
          StrArray_Push(&pScanner->IncludeDir, fileName);
          String_Destroy(&fileName);
        }
        IgnorePreProcessorv2(pBasicScanner, &strBuilder);
        Scanner_PushToken(pScanner, TK_PRE_PRAGMA, strBuilder.c_str, true);
      }
      else
      {
        IgnorePreProcessorv2(pBasicScanner, &strBuilder);
        Scanner_PushToken(pScanner, preToken, strBuilder.c_str, false);
      }
    }
    else if (preToken == TK_PRE_IF || preToken == TK_PRE_IFDEF ||
             preToken == TK_PRE_IFNDEF)
    {
      StrBuilder_Append(&strBuilder, pBasicScanner->currentItem.lexeme.c_str);

      BasicScanner_Match(pBasicScanner);
      Scanner_MatchAllPreprocessorSpaces(pBasicScanner, &strBuilder);

      lexeme = pBasicScanner->currentItem.lexeme.c_str;

      switch (state)
      {
        case NONE:
        case I1:
        case E1:
        {
          int iRes = 0;

          if (preToken == TK_PRE_IF)
          {
            iRes = EvalPre(pScanner, &strBuilder);
          }
          else
          {
            bool bFound = Scanner_FindPreprocessorItem2(pScanner, lexeme) != NULL;

            if (preToken == TK_PRE_IFDEF)
            {
              iRes = bFound ? 1 : 0;
            }
            else // if (preToken == TK_PRE_IFNDEF)
            {
              iRes = !bFound ? 1 : 0;
            }
          }

          if (iRes != 0)
          {
            StatePush(pScanner, I1);
          }
          else
          {
            StatePush(pScanner, I0);
          }
        }
        break;

        case I0:
          StatePush(pScanner, I0);
          break;

        case E0:
          StatePush(pScanner, E0);
          break;
      }

      state = StateTop(pScanner);
      bool bActive = IsIncludeState(state);

      IgnorePreProcessorv2(pBasicScanner, &strBuilder);
      Scanner_PushToken(pScanner, preToken, strBuilder.c_str, bActive);
    }
    else if (preToken == TK_PRE_ELIF)
    {
      // Match elif
      StrBuilder_Append(&strBuilder, pBasicScanner->currentItem.lexeme.c_str);

      BasicScanner_Match(pBasicScanner);
      Scanner_MatchAllPreprocessorSpaces(pBasicScanner, &strBuilder);

      switch (state)
      {
        case NONE:
        case I1:
          pScanner->StackIfDef.pItems[pScanner->StackIfDef.size - 1] = E0;
          break;

        case I0:
        {
          int iRes = EvalPre(pScanner, &strBuilder);

          if (pScanner->StackIfDef.size >= 2)
          {
            if ((pScanner->StackIfDef.pItems[pScanner->StackIfDef.size - 2] ==
                 I1 ||
                 pScanner->StackIfDef.pItems[pScanner->StackIfDef.size - 2] ==
                 E1))
            {
              if (iRes)
              {
                pScanner->StackIfDef.pItems[pScanner->StackIfDef.size - 1] = I1;
              }
            }
          }
          else
          {
            if (iRes)
            {
              pScanner->StackIfDef.pItems[pScanner->StackIfDef.size - 1] = I1;
            }
          }
        }
        break;

        case E0:
          break;

        case E1:
          ASSERT(0);
          break;
      }

      state = StateTop(pScanner);
      bool bActive = IsIncludeState(state);

      IgnorePreProcessorv2(pBasicScanner, &strBuilder);
      Scanner_PushToken(pScanner, TK_PRE_ELIF, strBuilder.c_str, bActive);
    }
    else if (preToken == TK_PRE_ENDIF)
    {
      // Match elif
      StrBuilder_Append(&strBuilder, pBasicScanner->currentItem.lexeme.c_str);

      BasicScanner_Match(pBasicScanner);
      Scanner_MatchAllPreprocessorSpaces(pBasicScanner, &strBuilder);

      IgnorePreProcessorv2(pBasicScanner, &strBuilder);
      StatePop(pScanner);

      state = StateTop(pScanner);
      bool bActive = IsIncludeState(state);

      Scanner_PushToken(pScanner, TK_PRE_ENDIF, strBuilder.c_str, bActive);
    }
    else if (preToken == TK_PRE_ELSE)
    {

      // Match else
      StrBuilder_Append(&strBuilder, pBasicScanner->currentItem.lexeme.c_str);
      BasicScanner_Match(pBasicScanner);

      switch (state)
      {
        case NONE:
          ASSERT(0);
          break;

        case I1:
          pScanner->StackIfDef.pItems[pScanner->StackIfDef.size - 1] = E0;
          break;

        case I0:
          if (pScanner->StackIfDef.size >= 2)
          {
            if ((pScanner->StackIfDef.pItems[pScanner->StackIfDef.size - 2] ==
                 I1 ||
                 pScanner->StackIfDef.pItems[pScanner->StackIfDef.size - 2] ==
                 E1))
            {
              pScanner->StackIfDef.pItems[pScanner->StackIfDef.size - 1] = E1;
            }
          }
          else
          {
            pScanner->StackIfDef.pItems[pScanner->StackIfDef.size - 1] = E1;
          }

          break;

        case E0:
          break;

        case E1:
          ASSERT(false);
          break;
      }

      state = StateTop(pScanner);
      bool bActive = IsIncludeState(state);

      IgnorePreProcessorv2(pBasicScanner, &strBuilder);
      Scanner_PushToken(pScanner, TK_PRE_ELSE, strBuilder.c_str, bActive);
    }
    else if (preToken == TK_PRE_ERROR)
    {
      // Match error
      StrBuilder_Append(&strBuilder, lexeme);
      BasicScanner_Match(pBasicScanner);

      if (IsIncludeState(state))
      {
        StrBuilder str = STRBUILDER_INIT;
        StrBuilder_Append(&str, ": #error : ");
        GetDefineString(pScanner, &str);
        Scanner_SetError(pScanner, str.c_str);
        StrBuilder_Destroy(&str);

        IgnorePreProcessorv2(pBasicScanner, &strBuilder);
        Scanner_PushToken(pScanner, TK_PRE_ERROR, strBuilder.c_str, true);
      }
      else
      {
        Scanner_PushToken(pScanner, preToken, strBuilder.c_str, false);
      }
    }
    else if (preToken == TK_PRE_LINE)
    {
      if (IsIncludeState(state))
      {
        // Match line
        StrBuilder_Append(&strBuilder, pBasicScanner->currentItem.lexeme.c_str);
        BasicScanner_Match(pBasicScanner);

        IgnorePreProcessorv2(pBasicScanner, &strBuilder);
        Scanner_PushToken(pScanner, TK_PRE_LINE, strBuilder.c_str, true);
      }
      else
      {
        Scanner_PushToken(pScanner, preToken, strBuilder.c_str, false);
      }
    }
    else if (preToken == TK_PRE_UNDEF)
    {
      if (IsIncludeState(state))
      {
        // Match undef
        StrBuilder_Append(&strBuilder, pBasicScanner->currentItem.lexeme.c_str);
        BasicScanner_Match(pBasicScanner);

        lexeme = BasicScanner_Lexeme(Scanner_Top(pScanner));

        MacroMap_RemoveKey(&pScanner->Defines2, lexeme);

        IgnorePreProcessorv2(pBasicScanner, &strBuilder);
        Scanner_PushToken(pScanner, TK_PRE_UNDEF, strBuilder.c_str, true);
      }
      else
      {
        Scanner_PushToken(pScanner, preToken, strBuilder.c_str, false);
      }
    }
    else if (preToken == TK_PRE_DEFINE)
    {
      // Match define
      StrBuilder_Append(&strBuilder, lexeme);
      BasicScanner_Match(pBasicScanner);

      // Match all ' '
      Scanner_MatchAllPreprocessorSpaces(pBasicScanner, &strBuilder);
      bool bActive = IsIncludeState(state);
      if (bActive)
      {
        ParsePreDefinev2(pScanner, &strBuilder);
      }

      IgnorePreProcessorv2(pBasicScanner, &strBuilder);
      Scanner_PushToken(pScanner, TK_PRE_DEFINE, strBuilder.c_str, bActive);
    }

    // break;
  } //#
  else if (token == TK_IDENTIFIER)
  {
    // codigo complicado tetris
    Scanner_BuyIdentifierThatCanExpandAndCollapse(pScanner);
  }
  else
  {
    ScannerItem* pNew = ScannerItem_Create();
    StrBuilder_Swap(&pNew->lexeme, &pBasicScanner->currentItem.lexeme);
    pNew->token = pBasicScanner->currentItem.token;
    pNew->bActive = bActive0;
    List_Add(&pScanner->AcumulatedTokens, pNew);

    BasicScanner_Match(pBasicScanner);
  }
  StrBuilder_Destroy(&strBuilder);
  //}//for

  // state = StateTop(pScanner);
  // pTopScanner->currentItem.bActive = IsIncludeState(state);
}

void PrintPreprocessedToFileCore(FILE* fp, Scanner* scanner)
{
  while (Scanner_TokenAt(scanner, 0) != TK_EOF)
  {
    Tokens token = Scanner_TokenAt(scanner, 0);
    const char* lexeme = Scanner_LexemeAt(scanner, 0);
    if (Scanner_IsActiveAt(scanner, 0))
    {
      switch (token)
      {
        // Tokens para linhas do pre processador
        case TK_PRE_INCLUDE:
        case TK_PRE_PRAGMA:
        case TK_PRE_IF:
        case TK_PRE_ELIF:
        case TK_PRE_IFNDEF:
        case TK_PRE_IFDEF:
        case TK_PRE_ENDIF:
        case TK_PRE_ELSE:
        case TK_PRE_ERROR:
        case TK_PRE_LINE:
        case TK_PRE_UNDEF:
        case TK_PRE_DEFINE:
          fprintf(fp, "\n");
          break;

        // fim tokens preprocessador
        case TK_LINE_COMMENT:
        case TK_COMMENT:
          fprintf(fp, " ");
          break;

        case TK_BOF:
          break;

        case TK_MACRO_CALL:
        case TK_MACRO_EOF:
        case TK_FILE_EOF:
          break;

        default:
          fprintf(fp, "%s", lexeme);
          break;
      }
    }

    Scanner_Match(scanner);
  }
}

void PrintPreprocessedToFile(const char* fileIn, const char* configFileName)
{
  String fullFileNamePath = STRING_INIT;
  GetFullPath(fileIn, &fullFileNamePath);

  Scanner scanner;
  Scanner_Init(&scanner);

  Scanner_IncludeFile(&scanner, fullFileNamePath, FileIncludeTypeFullPath,
                      false);

  if (configFileName != NULL)
  {
    String configFullPath = STRING_INIT;
    GetFullPath(configFileName, &configFullPath);

    Scanner_IncludeFile(&scanner, configFullPath, FileIncludeTypeFullPath,
                        true);
    Scanner_Match(&scanner);

    String_Destroy(&configFullPath);
  }

  ///
  char drive[CPRIME_MAX_DRIVE];
  char dir[CPRIME_MAX_DIR];
  char fname[CPRIME_MAX_FNAME];
  char ext[CPRIME_MAX_EXT];
  SplitPath(fullFileNamePath, drive, dir, fname, ext); // C4996

  char fileNameOut[CPRIME_MAX_DRIVE + CPRIME_MAX_DIR + CPRIME_MAX_FNAME + CPRIME_MAX_EXT + 1];
  strcat(fname, "_pre");
  MakePath(fileNameOut, drive, dir, fname, ".c");

  FILE* fp = fopen(fileNameOut, "w");
  PrintPreprocessedToFileCore(fp, &scanner);

  fclose(fp);
  Scanner_Destroy(&scanner);
  String_Destroy(&fullFileNamePath);
}

void PrintPreprocessedToConsole(const char* fileIn,
                                const char* configFileName)
{
  String fullFileNamePath = STRING_INIT;
  GetFullPath(fileIn, &fullFileNamePath);

  Scanner scanner;
  Scanner_Init(&scanner);

  Scanner_IncludeFile(&scanner, fullFileNamePath, FileIncludeTypeFullPath,
                      false);

  if (configFileName != NULL)
  {
    String configFullPath = STRING_INIT;
    GetFullPath(configFileName, &configFullPath);

    Scanner_IncludeFile(&scanner, configFullPath, FileIncludeTypeFullPath,
                        true);
    Scanner_Match(&scanner);

    String_Destroy(&configFullPath);
  }

  PrintPreprocessedToFileCore(stdout, &scanner);

  Scanner_Destroy(&scanner);
  String_Destroy(&fullFileNamePath);
}

int Scanner_GetNumberOfScannerItems(Scanner* pScanner)
{
  int nCount = 1; // contando com o "normal"
  ForEachListItem(ScannerItem, pItem, &pScanner->AcumulatedTokens)
  {
    nCount++;
  }
  return nCount;
}

ScannerItem* Scanner_ScannerItemAt(Scanner* pScanner, int index)
{

  // item0 item1 ..itemN
  //^
  // posicao atual

  ScannerItem* pScannerItem = NULL;

  if (!pScanner->bError)
  {
    // conta o numero de itens empilhados
    int nCount = 0;
    ForEachListItem(ScannerItem, pItem, &pScanner->AcumulatedTokens)
    {
      nCount++;
    }

    // precisa comprar tokens?
    if (index >= nCount)
    {
      for (int i = nCount; i <= index; i++)
      {
        // comprar mais tokens
        Scanner_BuyTokens(pScanner);
      }
      pScannerItem = pScanner->AcumulatedTokens.pTail;
    }
    else
    {
      // nao precisa comprar eh so pegar
      int n = 0;
      ForEachListItem(ScannerItem, pItem, &pScanner->AcumulatedTokens)
      {
        if (n == index)
        {
          pScannerItem = pItem;
          break;
        }
        n++;
      }
    }
  }

  
  return pScannerItem;
}

void Scanner_PrintItems(Scanner* pScanner)
{
  printf("-----\n");
  int n = Scanner_GetNumberOfScannerItems(pScanner);
  for (int i = 0; i < n; i++)
  {
    ScannerItem* pItem = Scanner_ScannerItemAt(pScanner, i);
    printf("%d : %s %s\n", i, pItem->lexeme.c_str, TokenToString(pItem->token));
  }
  printf("-----\n");
}

int Scanner_FileIndexAt(Scanner* pScanner, int index)
{
  ScannerItem* pScannerItem = Scanner_ScannerItemAt(pScanner, index);
  if (pScannerItem)
  {
    return pScannerItem->FileIndex;
  }
  return 0;
}

int Scanner_LineAt(Scanner* pScanner, int index)
{
  ScannerItem* pScannerItem = Scanner_ScannerItemAt(pScanner, index);
  if (pScannerItem)
  {
    return pScannerItem->Line;
  }
  return 0;
}

bool Scanner_IsActiveAt(Scanner* pScanner, int index)
{
  ScannerItem* pScannerItem = Scanner_ScannerItemAt(pScanner, index);
  if (pScannerItem)
  {
    return pScannerItem->bActive;
  }
  return false;
}

Tokens Scanner_TokenAt(Scanner* pScanner, int index)
{
  ScannerItem* pScannerItem = Scanner_ScannerItemAt(pScanner, index);
  if (pScannerItem)
  {
    return pScannerItem->token;
  }

  return TK_EOF;
}

const char* Scanner_LexemeAt(Scanner* pScanner, int index)
{
  ScannerItem* pScannerItem = Scanner_ScannerItemAt(pScanner, index);
  if (pScannerItem)
  {
    return pScannerItem->lexeme.c_str;
  }
  return "";
}

#define List_PopFront(ITEM, pList)                                             \
  \
do {                                                                           \
    ITEM *p = (pList)->pHead;                                                  \
    (pList)->pHead = (pList)->pHead->pNext;                                    \
    ITEM##_Destroy(p);                                                         \
  \
}                                                                         \
  while (0)

void Scanner_MatchDontExpand(Scanner* pScanner)
{
  if (!pScanner->bError)
  {
    if (pScanner->AcumulatedTokens.pHead != NULL)
    {
      List_PopFront(ScannerItem, &pScanner->AcumulatedTokens);
    }
    else
    {
      BasicScanner* pTopScanner = Scanner_Top(pScanner);
      if (pTopScanner == NULL)
      {
        return;
      }

      BasicScanner_Match(pTopScanner);

      Tokens token = pTopScanner->currentItem.token;

      while (token == TK_EOF && pScanner->stack->pPrevious != NULL)
      {
        ASSERT(pScanner->AcumulatedTokens.pHead == NULL);
        BasicScannerStack_PopIfNotLast(&pScanner->stack);
        pTopScanner = Scanner_Top(pScanner);
        token = pTopScanner->currentItem.token;
      }
    }
  }
}
void Scanner_Match(Scanner* pScanner)
{
  if (pScanner->AcumulatedTokens.pHead != NULL)
  {
    List_PopFront(ScannerItem, &pScanner->AcumulatedTokens);
    if (pScanner->AcumulatedTokens.pHead == NULL)
    {
      Scanner_BuyTokens(pScanner);
    }
  }
}

bool Scanner_MatchToken(Scanner* pScanner, Tokens token, bool bActive)
{
  if (pScanner->bError)
  {
    return false;
  }

  bool b = Scanner_TokenAt(pScanner, 0) == token;
  Scanner_Match(pScanner);
  return b;
}

void TScannerItemList_Destroy(TScannerItemList* p)
{
    List_Destroy(ScannerItem, (p));
}