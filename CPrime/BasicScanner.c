
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "StrBuilder.h"
#include "Stream.h"
#include "Mem.h"
#include "BasicScanner.h"

wchar_t BasicScanner_MatchChar(struct BasicScanner* scanner);

const char* TokenToString(enum Tokens tk)
{
    switch (tk)
    {
        case TK_NONE:
            return "None";

        case TK_BOF:
            return "Bof";
        case TK_EOF:
            return "Eof";
        case TK_LINE_COMMENT:
            return "LINE_COMMENT";
        case TK_COMMENT:
            return "COMMENT";
        case TK_CLOSE_COMMENT:
            return "CLOSE_COMMENT";
        case TK_OPEN_COMMENT:
            return "OPEN_COMMENT";
        case TK_STRING_LITERAL:
            return "LITERALSTR";
        case TK_IDENTIFIER:
            return "IDENTIFIER";
        case TK_SPACES:
            return "SPACES";
        case TK_DECIMAL_INTEGER:
            return "TK_DECIMAL_INTEGER";
        case TK_HEX_INTEGER:
            return "TK_HEX_INTEGER";
        case TK_FLOAT_NUMBER:
            return "TK_FLOAT_NUMBER";
        case TK_BREAKLINE:
            return "BREAKLINE";
        case TK_BACKSLASHBREAKLINE:
            return "TK_BACKSLASHBREAKLINE";
        case TK_PREPROCESSOR:
            return "PREPROCESSOR";
        case CHARACTER_TABULATION:
            return "CHARACTER_TABULATION";
        case TK_EXCLAMATION_MARK:
            return "!";// = '!';
        case TK_QUOTATION_MARK:
            return "\"";//,// = '\"';
        case TK_NUMBER_SIGN:
            return "#";//,// = '#';
        case TK_DOLLAR_SIGN:
            return "$";//,// = '$';
        case TK_PERCENT_SIGN:
            return "%";//,// = '%';
        case TK_AMPERSAND:
            return "&";//,// = '&';
        case TK_APOSTROPHE:
            return "'";//,// = '\'';
        case TK_LEFT_PARENTHESIS:
            return "(";//,// = '(';
        case TK_RIGHT_PARENTHESIS:
            return ")";//,// = ')';
        case TK_ASTERISK:
            return "*";//,// = '*';
        case TK_PLUS_SIGN:
            return "+";//,// = '+';
        case TK_COMMA:
            return ",";//,// = ',';
        case TK_HYPHEN_MINUS:
            return "-";//,// = '-';
        case TK_FULL_STOP:
            return ".";//,// = '.';
        case TK_SOLIDUS:
            return "/";//,// = '/';
        case TK_COLON:
            return ":";//,// = ':';
        case TK_SEMICOLON:
            return ";";//,// = ';';
        case TK_LESS_THAN_SIGN:
            return "<";//,// = '<';
        case TK_EQUALS_SIGN:
            return "=";//,// = '=';
        case TK_GREATER_THAN_SIGN:
            return ">";//,// = '>';
        case TK_QUESTION_MARK:
            return "?";//,// = '\?';
        case TK_COMMERCIAL_AT:
            return "@";//,// = '@';
        case TK_LEFT_SQUARE_BRACKET:
            return "[";//,// = '[';
        case REVERSE_SOLIDUS:
            return "\\";//,// = '\\';
        case TK_RIGHT_SQUARE_BRACKET:
            return "]";//,// = ']';
        case TK_CIRCUMFLEX_ACCENT:
            return "^";// = '^';
        case TK_LOW_LINE:
            return "_";//,// = '_';
        case TK_GRAVE_ACCENT:
            return "`";//,// = '`';
        case TK_LEFT_CURLY_BRACKET:
            return "{";//,// = '{';
        case TK_VERTICAL_LINE:
            return "|";//,// = '|';
        case TK_RIGHT_CURLY_BRACKET:
            return "}";//,// = '}';

        case TK_TILDE:
            return "~";//,// = '~';
            break;
        case TK_AUTO:
            return "auto";

        case TK_BREAK:
            return "break";
        case TK_CASE:
            return "case";
        case TK_CHAR:
            return "char";
        case TK_CONST:
            return "const";
        case TK_CONTINUE:
            return "continue";
        case TK_DEFAULT:
            return "default";

        case TK_DO:
            return "do";
        case TK_DOUBLE:
            return "double";
        case TK_ELSE:
            return "else";
        case TK_ENUM:
            return "enum";
        case TK_EXTERN:
            return "extern";
        case TK_FLOAT:
            return "float";
        case TK_FOR:
            return "for";
        case TK_GOTO:
            return "goto";
        case TK_IF:
            return "if";
        case TK_INT:
            return "int";
        case TK_LONG:
            return "long";
        case TK__INT8:
            return "__int8";
        case TK__INT16:
            return "__int16";
        case TK__INT32:
            return "__int32";
        case TK__INT64:
            return "__int64";
        case TK__WCHAR_T:
            return "__wchar_t";

        case TK_REGISTER:
            return "register";
        case TK_RETURN:
            return "return";
        case TK_SHORT:
            return "short";
        case TK_SIGNED:
            return "signed";
        case TK_SIZEOF:
            return "sizeof";
        case TK_STATIC:
            return "static";
        case TK_STRUCT:
            return "struct";
        case TK_SWITCH:
            return "switch";
        case TK_TYPEDEF:
            return "typedef";
        case TK_UNION:
            return "union";

        case TK_UNSIGNED:
            return "unsigned";
        case TK_VOID:
            return "void";
        case TK__BOOL:
            return "_Bool";
            break;
        case TK_VOLATILE:
            return "volatile";
        case TK_WHILE:
            return "while";
        case TK_ARROW:
            return "->";

        case TK_PLUSPLUS:
            return "++";

        case TK_MINUSMINUS:
            return "--";

        case TK_EQUALEQUAL:
            return "==";
        case TK_NOTEQUAL:
            return "!=";

        case TK_LESSLESS:
            return "<<";

        case TK_GREATERGREATER:
            return ">>";


        case TK_LESSEQUAL:
            return "<=";
        case TK_GREATEREQUAL:
            return ">=";

        case TK_ANDAND:
            return "&&";
        case TK_OROR:
            return "||";
        case TK_MULTIEQUAL:
            return "*=";
        case TK_DIVEQUAL:
            return "/=";
        case TK_PERCENT_EQUAL:
            return "/%=";
        case TK_PLUSEQUAL:
            return "+=";
        case TK_MINUS_EQUAL:
            return "-=";
        case TK_ANDEQUAL:
            return "!=";
        case TK_CARETEQUAL:
            return "^=";
        case TK_OREQUAL:
            return "|=";
        case TK_NUMBERNUMBER:
            return "##";
        case TK_LESSCOLON:
            return "<:";
        case TK_COLONGREATER:
            return ":>";
        case TK_LESSPERCENT:
            return "<%";
        case TK_PERCENTGREATER:
            return "%>";
        case TK_PERCENTCOLON:
            return "%:";
        case TK_DOTDOTDOT:
            return "...";
        case TK_GREATERGREATEREQUAL:
            return ">>=";
        case TK_LESSLESSEQUAL:
            return "<<=";
        case TK_PERCENTCOLONPERCENTCOLON:
            return "/%:/%:";

        case TK_PRE_INCLUDE:
            return "TK_PRE_INCLUDE";

        case TK_PRE_DEFINE:
            return "TK_PRE_DEFINE";
            //
        case TK_MACRO_CALL:
            return "TK_MACRO_CALL";

        case TK_MACRO_EOF:
            return "TK_MACRO_EOF";

        case TK_FILE_EOF:
            return "TK_FILE_EOF";

        case TK_PRE_PRAGMA:
            return "TK_PRE_PRAGMA";

        default:
            //assert(false);
            break;
    }
    return "???";
}

struct ScannerItem* ScannerItem_Create(void) /*@default*/
{
    struct ScannerItem *p = (struct ScannerItem *) Malloc(sizeof * p);
    if (p != NULL)
    {
        ScannerItem_Init(p);
    }
    return p;
}

void ScannerItem_Delete(struct ScannerItem* pScannerItem) /*@default*/
{
    if (pScannerItem != NULL)
    {
        ScannerItem_Destroy(pScannerItem);
        Free((void*)pScannerItem);
    }
}

void ScannerItem_Init(struct ScannerItem* scannerItem) /*@default*/
{
    LocalStrBuilder_Init(&scannerItem->lexeme);
    scannerItem->token = TK_NONE;
    scannerItem->Line =  -1;
    scannerItem->FileIndex =  -1;
    scannerItem->bActive =  1;
    scannerItem->pNext = NULL;
}

void ScannerItem_Reset(struct ScannerItem* scannerItem)
{
    LocalStrBuilder_Clear(&scannerItem->lexeme);
    scannerItem->token = TK_ERROR;
}

void ScannerItem_Copy(struct ScannerItem* scannerItem,
                      struct ScannerItem* other)
{
    scannerItem->token = other->token;
    LocalStrBuilder_Set(&scannerItem->lexeme, other->lexeme.c_str);
}

void ScannerItem_Swap(struct ScannerItem* scannerItem,
                      struct ScannerItem* other)
{
    enum Tokens tk = other->token;
    other->token = scannerItem->token;
    scannerItem->token = tk;
    LocalStrBuilder_Swap(&scannerItem->lexeme, &other->lexeme);
}

void ScannerItem_Destroy(struct ScannerItem* scannerItem) /*@default*/
{
    LocalStrBuilder_Destroy(&scannerItem->lexeme);
}

void BasicScanner_InitCore(struct BasicScanner* pBasicScanner,
                           enum BasicScannerType Type)
{
    pBasicScanner->m_Token = TK_BOF;
    pBasicScanner->Type = Type;
    pBasicScanner->pPrevious = NULL;
    pBasicScanner->FileIndex = -1;
    pBasicScanner->bLineStart = true;
    pBasicScanner->bMacroExpanded = false;
    ScannerItem_Init(&pBasicScanner->currentItem);
    pBasicScanner->currentItem.token = TK_BOF;
}

bool BasicScanner_Init(struct BasicScanner* pBasicScanner,
                       const char* name,
                       const char* Text,
                       enum BasicScannerType type)
{
    BasicScanner_InitCore(pBasicScanner, type);
    bool b = Stream_Init(&pBasicScanner->stream, name, Text);
    return b ? true : false;
}

bool BasicScanner_InitFile(struct BasicScanner* pBasicScanner,
                           const char* fileName)
{
    BasicScanner_InitCore(pBasicScanner, BasicScannerType_File);

    bool b = Stream_InitFile(&pBasicScanner->stream, fileName);
    return b ? true : false;
}

bool BasicScanner_Create(struct BasicScanner** pp,
                         const char* name,
                         const char* Text,
                         enum BasicScannerType Type)
{
    bool result = false /*nomem*/;
    struct BasicScanner* p = (struct BasicScanner*)Malloc(sizeof(struct BasicScanner));
    if (p)
    {
        result = BasicScanner_Init(p, name, Text, Type);
        if (result == true)
        {
            *pp = p;
        }
        else
        {
            Free(p);
        }
    }
    return result;
}

bool BasicScanner_CreateFile(const char* fileName, struct BasicScanner** pp)
{
    bool result = false /*nomem*/;
    struct BasicScanner* p = (struct BasicScanner*)Malloc(sizeof(struct BasicScanner));
    if (p)
    {
        result = BasicScanner_InitFile(p, fileName);
        if (result == true)
        {
            *pp = p;
        }
        else
        {
            Free(p);
        }
    }
    return result;
}


void BasicScanner_Destroy(struct BasicScanner* pBasicScanner) /*@default*/
{
    Stream_Destroy(&pBasicScanner->stream);
    ScannerItem_Destroy(&pBasicScanner->currentItem);
}

void BasicScanner_Delete(struct BasicScanner* pBasicScanner) /*@default*/
{
    if (pBasicScanner != NULL)
    {
        BasicScanner_Destroy(pBasicScanner);
        Free((void*)pBasicScanner);
    }
}

struct TkPair
{
    const char* lexeme;
    enum Tokens token;
};

static struct TkPair singleoperators[] =
{
    //punctuator: one of

    {"[", TK_LEFT_SQUARE_BRACKET}, //0
{"]", TK_RIGHT_SQUARE_BRACKET},
{"(", TK_LEFT_PARENTHESIS},
{")", TK_RIGHT_PARENTHESIS},
{"{", TK_LEFT_CURLY_BRACKET},
{"}", TK_RIGHT_CURLY_BRACKET},
{".", TK_FULL_STOP},
{"&", TK_AMPERSAND},
{"*", TK_ASTERISK},
{"+", TK_PLUS_SIGN},
{"-", TK_HYPHEN_MINUS},
{"~", TK_TILDE},
{"!", TK_EXCLAMATION_MARK},
{"%", TK_PERCENT_SIGN},
{"<", TK_LESS_THAN_SIGN},
{">", TK_GREATER_THAN_SIGN},
{"^", TK_CIRCUMFLEX_ACCENT},
{"|", TK_VERTICAL_LINE},
{"?", TK_QUESTION_MARK},
{":", TK_COLON},
{";", TK_SEMICOLON},
{"=", TK_EQUALS_SIGN},
{",", TK_COMMA},
{"$", TK_DOLLAR_SIGN},
{"@", TK_COMMERCIAL_AT} //pode ser usado em macros pp-tokens
//  {"...", TK_DOTDOTDOT},//50
//  {"%:%:", TK_PERCENTCOLONPERCENTCOLON},
//  {"<<=", TK_LESSLESSEQUAL},
//{">>=", TK_GREATERGREATEREQUAL},
};

static struct TkPair doubleoperators[] =
{
    {"->", TK_ARROW},//25
{"++", TK_PLUSPLUS},
{"--", TK_MINUSMINUS},
{"<<", TK_LESSLESS},
{">>", TK_GREATERGREATER},
{"<=", TK_LESSEQUAL},
{">=", TK_GREATEREQUAL},
{"==", TK_EQUALEQUAL},
{"!=", TK_NOTEQUAL},
{"&&", TK_ANDAND},
{"||", TK_OROR},
{"*=", TK_MULTIEQUAL},
{"/=", TK_DIVEQUAL},
{"%=", TK_PERCENT_EQUAL},
{"+=", TK_PLUSEQUAL},
{"-=", TK_MINUS_EQUAL},
{"&=", TK_ANDEQUAL},
{"^=", TK_CARETEQUAL},
{"|=", TK_OREQUAL},
{"##", TK_NUMBERNUMBER},
{"<:", TK_LESSCOLON},
{":>", TK_COLONGREATER},
{"<%", TK_LESSPERCENT},
{"%>", TK_PERCENTGREATER},
{"%:", TK_PERCENTCOLON},

{"...", TK_DOTDOTDOT},//50
{"%:%:", TK_PERCENTCOLONPERCENTCOLON},
{"<<=", TK_LESSLESSEQUAL},
{">>=", TK_GREATERGREATEREQUAL}
};

static struct TkPair keywords[] =
{
    //keywords
    {"auto", TK_AUTO},
{"break", TK_BREAK},
{"case", TK_CASE},
{"char", TK_CHAR},
{"const", TK_CONST},
{"continue", TK_CONTINUE},
{"default", TK_DEFAULT},
{"do", TK_DO},
{"double", TK_DOUBLE},
{"else", TK_ELSE},
{"enum", TK_ENUM},
{"extern", TK_EXTERN},
{"float", TK_FLOAT},
{"for", TK_FOR},
{"goto", TK_GOTO},
{"if", TK_IF},
{"inline", TK_INLINE},
{"__inline", TK__INLINE},
{"__forceinline", TK__FORCEINLINE},
{"int", TK_INT},
{"long", TK_LONG},
//
{"__int8", TK__INT8},
{"__int16", TK__INT16},
{"__int32", TK__INT32},
{"__int64", TK__INT64},
{"__wchar_t", TK__WCHAR_T},
//
{"register", TK_REGISTER},
{"restrict", TK_RESTRICT},
{"return", TK_RETURN},
{"short", TK_SHORT},
{"signed", TK_SIGNED},
{"sizeof", TK_SIZEOF},
{"static", TK_STATIC},
{"struct", TK_STRUCT},
{"switch", TK_SWITCH},
{"typedef", TK_TYPEDEF},
{"union", TK_UNION},

{"unsigned", TK_UNSIGNED},
{"void", TK_VOID},
{"volatile", TK_VOLATILE},
{"while", TK_WHILE},
{"_Alignas", TK__ALIGNAS},
{"_Alignof", TK__ALINGOF},
{"_Atomic", TK__ATOMIC},

{"_Bool", TK__BOOL},
{"_Complex", TK__COMPLEX},
{"_Generic", TK__GENERIC},
{"_Imaginary", TK__IMAGINARY},
{"_Noreturn", TK__NORETURN},
{"_Static_assert", TK__STATIC_ASSERT},
{"_Thread_local", TK__THREAD_LOCAL},
//
{"__asm", TK__ASM} //visual studio

};
void BasicScanner_Next(struct BasicScanner* scanner);
void BasicScanner_Match(struct BasicScanner* scanner)
{
    BasicScanner_Next(scanner);
}

bool BasicScanner_MatchToken(struct BasicScanner* scanner, enum Tokens token)
{
    bool b = false;
    if (scanner->currentItem.token == token)
    {
        b = true;
        BasicScanner_Match(scanner);
    }
    return b;
}


void BasicScanner_Next(struct BasicScanner* scanner)
{
    if (scanner->Type == BasicScannerType_Token)
    {
        if (scanner->bLineStart)
        {
            ScannerItem_Reset(&scanner->currentItem);
            scanner->currentItem.token = scanner->m_Token;
            LocalStrBuilder_Set(&scanner->currentItem.lexeme,
                                scanner->stream.Text);
            scanner->bLineStart = false;
        }
        else
        {
            scanner->currentItem.token = TK_EOF;
        }
        return;
    }

    if (scanner->currentItem.token == TK_MACRO_EOF ||
        scanner->currentItem.token == TK_FILE_EOF)
    {
        scanner->currentItem.token = TK_EOF;
        return;
    }


    scanner->currentItem.Line = scanner->stream.Line;
    scanner->currentItem.FileIndex = scanner->FileIndex;

    //bool bLineStart = scanner->bLineStart;
    //scanner->bLineStart = false;
    wchar_t ch = '\0';
    ScannerItem_Reset(&scanner->currentItem);
    ch = scanner->stream.Character;
    wchar_t ch1 = Stream_LookAhead(&scanner->stream);
    if (ch == '.' && ch1 == '.')
    {
        BasicScanner_MatchChar(scanner);
        ch = BasicScanner_MatchChar(scanner);
        if (ch != '.')
        {
            scanner->currentItem.token = TK_ERROR;
        }
        BasicScanner_MatchChar(scanner);
        scanner->currentItem.token = TK_DOTDOTDOT;
        return;
    }
    //procura por puncturares com 2 caracteres
    for (int i = 0; i < sizeof(doubleoperators) / sizeof(doubleoperators[0]); i++)
    {
        if (doubleoperators[i].lexeme[0] == ch &&
            doubleoperators[i].lexeme[1] == ch1)
        {
            scanner->currentItem.token = doubleoperators[i].token;
            BasicScanner_MatchChar(scanner);
            BasicScanner_MatchChar(scanner);
            scanner->bLineStart = false;
            return;
        }
    }

    if (ch == '*' && ch1 == '/')
    {
        scanner->currentItem.token = TK_CLOSE_COMMENT;
        ch = BasicScanner_MatchChar(scanner);
        ch = BasicScanner_MatchChar(scanner);

        return;
    }

    //procura por puncturtorscom 1 caracteres
    for (int i = 0; i < sizeof(singleoperators) / sizeof(singleoperators[0]); i++)
    {
        if (singleoperators[i].lexeme[0] == ch)
        {
            scanner->currentItem.token = singleoperators[i].token;
            BasicScanner_MatchChar(scanner);
            scanner->bLineStart = false;
            return;
        }
    }
    //U'
    //u
    //L
    //Devido ao L' tem que vir antes do identificador
    //literal char
    if (ch == L'"' ||
        (ch == L'L' && ch1 == L'"'))
    {
        if (ch == 'L')
        {
            ch = BasicScanner_MatchChar(scanner); //L
        }
        scanner->currentItem.token = TK_STRING_LITERAL;
        ch = BasicScanner_MatchChar(scanner);
        for (;;)
        {
            if (ch == '\"')
            {
                ch = BasicScanner_MatchChar(scanner);
                break;
            }
            else if (ch == '\\')
            {
                //escape
                ch = BasicScanner_MatchChar(scanner);
                ch = BasicScanner_MatchChar(scanner);
            }
            else if (ch == '\0')
            {
                //oops
                scanner->currentItem.token = TK_EOF;
                break;
            }
            else
            {
                ch = BasicScanner_MatchChar(scanner);
            }
        }
        scanner->bLineStart = false;
        return;
    }
    //Devido ao L' tem que vir antes do identificador
    //literal
    if (ch == L'\'' ||
        (ch == L'L' && ch1 == L'\''))
    {
        if (ch == 'L')
        {
            ch = BasicScanner_MatchChar(scanner); //L
        }
        scanner->currentItem.token = TK_CHAR_LITERAL;
        ch = BasicScanner_MatchChar(scanner); //'
        if (ch == '\\')
        {
            //escape
            ch = BasicScanner_MatchChar(scanner); //
            ch = BasicScanner_MatchChar(scanner); //caractere
        }
        else
        {
            ch = BasicScanner_MatchChar(scanner);//caractere
        }
        ch = BasicScanner_MatchChar(scanner);//'
        scanner->bLineStart = false;
        return;
    }
    //Identificador
    if ((ch >= 'a' && ch <= 'z') ||
        (ch >= 'A' && ch <= 'Z') ||
        ch == '_')
    {
        scanner->currentItem.token = TK_IDENTIFIER;
        ch = BasicScanner_MatchChar(scanner);
        while ((ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z') ||
               (ch >= '0' && ch <= '9') ||
               ch == '_')
        {
            ch = BasicScanner_MatchChar(scanner);
        }
        //v� se � keywords e corrige o token
        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        {
            if (BasicScanner_IsLexeme(scanner, keywords[i].lexeme))
            {
                scanner->currentItem.token = keywords[i].token;
                //StrBuilder_Append(&scanner->currentItem.lexeme, keywords[i].lexeme);
                //Stream_Next(&scanner->stream);
                break;
            }
        }
        scanner->bLineStart = false;
        return;
    }

    //TODO binarios

    if (ch == '0' &&
        (
        (ch1 == 'x' || ch1 == 'X') || //hex 
        (ch1 >= '0' && ch1 <= '9')) //octal
        )
    {
        ch = BasicScanner_MatchChar(scanner);
        if (ch == 'x' || ch == 'X')
        {
            scanner->currentItem.token = TK_HEX_INTEGER;
        }
        else if (ch1 >= '0' && ch1 <= '9')
        {
            scanner->currentItem.token = TK_OCTAL_INTEGER;
        }
        else
        {
            assert(false);
        }
        ch = BasicScanner_MatchChar(scanner);


        while ((ch >= '0' && ch <= '9') ||
            (ch >= 'A' && ch <= 'F') ||
               (ch >= 'a' && ch <= 'f'))
        {
            ch = BasicScanner_MatchChar(scanner);
        }

        //integer suffix
        if (ch == 'u' || ch == 'U')
        {
            ch = BasicScanner_MatchChar(scanner);
        }
        if (ch == 'l' || ch == 'L')
        {
            ch = BasicScanner_MatchChar(scanner);
            if (ch == 'l' || ch == 'L')
            {
                ch = BasicScanner_MatchChar(scanner);
            }
            else if (ch == 'u' || ch == 'U')
            {
                ch = BasicScanner_MatchChar(scanner);
            }
            else
            {
                //error
            }
        }
        else if (ch == 'i' || ch == 'I')
        {
            ch = BasicScanner_MatchChar(scanner);
            if (ch == '3')
            {
                ch = BasicScanner_MatchChar(scanner);
                if (ch == '2')
                {
                    ch = BasicScanner_MatchChar(scanner);
                }
                else
                {
                    //error 
                }
            }
            else if (ch == '1')
            {
                ch = BasicScanner_MatchChar(scanner);
                if (ch == '6')
                {
                    ch = BasicScanner_MatchChar(scanner);
                }
                else
                {
                    //error 
                }
            }
            else if (ch == '6')
            {
                ch = BasicScanner_MatchChar(scanner);
                if (ch == '4')
                {
                    ch = BasicScanner_MatchChar(scanner);
                }
                else
                {
                    //error 
                }
            }
            else if (ch == '8')
            {
                ch = BasicScanner_MatchChar(scanner);
            }
        }

        return;
    }
    //numero
    if (ch >= '0' && ch <= '9')
    {
        scanner->currentItem.token = TK_DECIMAL_INTEGER;
        ch = BasicScanner_MatchChar(scanner);
        while ((ch >= '0' && ch <= '9'))
        {
            ch = BasicScanner_MatchChar(scanner);
        }

        //integer suffix
        if (ch == 'u' || ch == 'U')
        {
            ch = BasicScanner_MatchChar(scanner);
        }
        if (ch == 'l' || ch == 'L')
        {
            ch = BasicScanner_MatchChar(scanner);
            if (ch == 'l' || ch == 'L')
            {
                ch = BasicScanner_MatchChar(scanner);
            }
            else if (ch == 'u' || ch == 'U')
            {
                ch = BasicScanner_MatchChar(scanner);
            }
            else
            {
                //error
            }
        }
        else if (ch == 'i' || ch == 'I')
        {
            ch = BasicScanner_MatchChar(scanner);
            if (ch == '3')
            {
                ch = BasicScanner_MatchChar(scanner);
                if (ch == '2')
                {
                    ch = BasicScanner_MatchChar(scanner);
                }
                else
                {
                    //error 
                }
            }
            else if (ch == '1')
            {
                ch = BasicScanner_MatchChar(scanner);
                if (ch == '6')
                {
                    ch = BasicScanner_MatchChar(scanner);
                }
                else
                {
                    //error 
                }
            }
            else if (ch == '6')
            {
                ch = BasicScanner_MatchChar(scanner);
                if (ch == '4')
                {
                    ch = BasicScanner_MatchChar(scanner);
                }
                else
                {
                    //error 
                }
            }
            else if (ch == '8')
            {
                ch = BasicScanner_MatchChar(scanner);
            }
        }
        else
        {
            if (ch == '.')
            {
                ch = BasicScanner_MatchChar(scanner);
                scanner->currentItem.token = TK_FLOAT_NUMBER;
                while ((ch >= '0' && ch <= '9'))
                {
                    ch = BasicScanner_MatchChar(scanner);
                }
            }

            if (scanner->stream.Character == 'e' ||
                scanner->stream.Character == 'E')
            {
                ch = BasicScanner_MatchChar(scanner);
                if (ch == '-' ||
                    ch == '+')
                {
                    ch = BasicScanner_MatchChar(scanner);
                }
                while ((ch >= '0' && ch <= '9'))
                {
                    ch = BasicScanner_MatchChar(scanner);
                }
            }

            if (ch == 'L' || ch == 'l' || ch == 'F' || ch == 'f')
            {
                ch = BasicScanner_MatchChar(scanner);
            }
        }

        scanner->bLineStart = false;
        return;
    }
    //quebra de linha
    if (ch == '\n' || ch == L'\r')
    {
        scanner->currentItem.token = TK_BREAKLINE;
        if (ch == L'\r' && ch1 == L'\n')
        {
            //so coloca \n
            Stream_Match(&scanner->stream);
            ch = scanner->stream.Character;
            ch = BasicScanner_MatchChar(scanner);
        }
        else
        {
            ch = BasicScanner_MatchChar(scanner);
            LocalStrBuilder_Clear(&scanner->currentItem.lexeme);
            //normaliza para windows?
            LocalStrBuilder_Append(&scanner->currentItem.lexeme, "\r\n");
        }
        scanner->bLineStart = true;
        return;
    }
    if (ch == '\0')
    {
        if (scanner->bMacroExpanded)
        {
            scanner->currentItem.token = TK_MACRO_EOF;
        }
        else
        {
            scanner->currentItem.token = TK_FILE_EOF;
        }

        scanner->bLineStart = false;
        return;
    }
    if (ch == '\f')
    {
        scanner->currentItem.token = TK_SPACES;
        BasicScanner_MatchChar(scanner);
        return;
    }
    //espacos
    if (ch == ' ' || ch == '\t')
    {
        scanner->currentItem.token = TK_SPACES;
        ch = BasicScanner_MatchChar(scanner);
        while (ch == ' ' || ch == '\t')
        {
            ch = BasicScanner_MatchChar(scanner);
        }
        //continua com scanner->bLineStart
        return;
    }
    if (ch < 32)
    {
        scanner->currentItem.token = TK_SPACES;
    }
    //
    if (scanner->stream.Character == '#')
    {
        ch = BasicScanner_MatchChar(scanner);
        if (scanner->bLineStart)
        {
            scanner->currentItem.token = TK_PREPROCESSOR;
        }
        else
        {
            scanner->currentItem.token = TK_NUMBER_SIGN;
        }
        return;
    }



    //comentario de linha
    if (ch == '/')
    {
        if (ch1 == '/')
        {
            scanner->currentItem.token = TK_LINE_COMMENT;
            ch = BasicScanner_MatchChar(scanner);
            ch = BasicScanner_MatchChar(scanner);
            while (ch != '\r' &&
                   ch != '\n' &&
                   ch != '\0')
            {
                ch = BasicScanner_MatchChar(scanner);
            }
        }
        else if (ch1 == '*')
        {
            scanner->currentItem.token = TK_COMMENT;
            ch = BasicScanner_MatchChar(scanner);
            ch = BasicScanner_MatchChar(scanner);
            if (ch == '@')
            {
                scanner->currentItem.token = TK_OPEN_COMMENT;
                ch = BasicScanner_MatchChar(scanner);
            }
            else
            {
                for (;;)
                {
                    if (ch == '*')
                    {
                        ch = BasicScanner_MatchChar(scanner);
                        if (ch == '/')
                        {
                            ch = BasicScanner_MatchChar(scanner);
                            break;
                        }
                    }
                    else if (ch == L'\r')
                    {
                        //so coloca \n
                        Stream_Match(&scanner->stream);
                        ch = scanner->stream.Character;
                        if (ch == L'\n')
                        {
                            ch = BasicScanner_MatchChar(scanner);
                        }
                        else
                        {
                        }
                    }
                    else if (ch == L'\n')
                    {
                        ch = BasicScanner_MatchChar(scanner);
                    }
                    else
                    {
                        ch = BasicScanner_MatchChar(scanner);
                    }
                }
            }



            return;
        }
        else
        {
            scanner->currentItem.token = TK_SOLIDUS;
            ch = BasicScanner_MatchChar(scanner);
        }
        return;
    }
    //junta linha
    if (ch == L'\\' &&
        (ch1 == L'\n' || ch1 == L'\r'))
    {
        //1) Whenever backslash appears at the end of 
        //a line(immediately followed by the newline character), both 
        //backslash and newline are deleted,
        //combining two physical source lines into one logical 
        //source line.This is a single - pass operation; 
        //a line ending in two backslashes followed by an empty 
        //line does not combine three lines into one.
        //If a universal character name(\uXXX) is formed in this 
        //phase, the behavior is undefined.

        ch = BasicScanner_MatchChar(scanner);

        if (ch == L'\r')
        {
            ch = BasicScanner_MatchChar(scanner);

            if (ch == L'\n')
            {
                ch = BasicScanner_MatchChar(scanner);
            }
        }
        else if (ch == L'\n')
        {
            BasicScanner_MatchChar(scanner);
        }

        //homogeiniza \r\n para \n
        LocalStrBuilder_Set(&scanner->currentItem.lexeme, "\\\n");

        scanner->currentItem.token = TK_BACKSLASHBREAKLINE;
        scanner->bLineStart = false;
        //BasicScanner_Match(scanner);

        return;
    }
    if (ch == 2)  //peguei um
    {
        ch = BasicScanner_MatchChar(scanner);
        scanner->currentItem.token = TK_MACROPLACEHOLDER;
        scanner->bLineStart = false;
        return;
    }
    if (ch == '\\')
    {
        ch = BasicScanner_MatchChar(scanner);
        scanner->currentItem.token = REVERSE_SOLIDUS;
        return;
    }

    if (scanner->currentItem.token == TK_ERROR)
    {
        printf("invalid char, scanner");
    }
    //assert(false);
}


enum Tokens BasicScanner_Token(struct BasicScanner* scanner)
{
    return scanner->currentItem.token;
}

const char* BasicScanner_Lexeme(struct BasicScanner* scanner)
{
    return scanner->currentItem.lexeme.c_str;
}

bool BasicScanner_IsLexeme(struct BasicScanner* scanner, const char* psz)
{
    return strcmp(BasicScanner_Lexeme(scanner), psz) == 0;
}

wchar_t BasicScanner_MatchChar(struct BasicScanner* scanner)
{
    LocalStrBuilder_AppendChar(&scanner->currentItem.lexeme,
        (char)scanner->stream.Character);
    Stream_Match(&scanner->stream);
    return scanner->stream.Character;
}

//////////////////////////////////////////////



void BasicScannerStack_Init(struct BasicScannerStack* stack) /*@default*/
{
    stack->pTop = NULL;
}

void BasicScannerStack_Push(struct BasicScannerStack* stack, struct BasicScanner* pItem)
{
    if (stack->pTop == NULL)
    {
        stack->pTop = pItem;
    }
    else
    {
        pItem->pPrevious = stack->pTop;
        stack->pTop = pItem;
    }
}

struct BasicScanner* BasicScannerStack_PopGet(struct BasicScannerStack* stack)
{
    struct BasicScanner* pItem = NULL;
    if (stack->pTop != NULL)
    {
        pItem = stack->pTop;
        stack->pTop = pItem->pPrevious;
    }
    return pItem;
}

void BasicScannerStack_PopIfNotLast(struct BasicScannerStack* stack)
{
    //assert(*stack != NULL);
    if (stack->pTop->pPrevious != NULL)
    {
        BasicScanner_Delete(BasicScannerStack_PopGet(stack));
    }
}

void BasicScannerStack_Pop(struct BasicScannerStack* stack)
{
    BasicScanner_Delete(BasicScannerStack_PopGet(stack));
}

void BasicScannerStack_Destroy(struct BasicScannerStack* stack)
{
    struct BasicScanner* pItem = stack->pTop;
    while (pItem)
    {
        struct BasicScanner* p = pItem;
        pItem = pItem->pPrevious;
        BasicScanner_Delete(p);
    }
}
