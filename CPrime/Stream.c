
#include "Stream.h"
#include "StringEx.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "StrBuilder.h"

#include <ctype.h>
#include "Path.h"
#include "Mem.h"

Result LoadFile(const char* filename, const char** out, int* szOut)
{
  Result result = RESULT_FAIL;
  int lSize = 0;
  FILE*  fp = (FILE* ) fopen(filename, "rb");

  if (fp)
  {
    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);
    char*  buffer = (char*)Malloc(lSize + 1);

    if (buffer)
    {
      /*int fr =*/ fread(buffer, 1, lSize , fp);

      if (feof(fp))
      {
        //ok leu tudo
      }

      if (!ferror(fp))
      {
        //ok
        buffer[lSize] = '\0';
        *out = buffer;
        buffer = NULL;
        result = RESULT_OK;
        *szOut = lSize;
      }

      Free(buffer);
    }

    fclose(fp);
  }

  return result;
}


Result SStream_InitFile(SStream* pStream,
                        const char* fullPath)
{
  //assert(IsFullPath(fullPath));
  String_InitWith(&pStream->NameOrFullPath, fullPath);
  String_InitWith(&pStream->FullDir2, NULL);
  pStream->currentLine = 1;
  pStream->currentCol = 1;
  pStream->position = 0;
  Result result = LoadFile(fullPath, (const char**)&pStream->text,
                           &pStream->text_length);

  if (result == RESULT_OK)
  {
    //O objetivo aqui eh pegar o diretorio
    GetFullDir(fullPath, &pStream->FullDir2);

    if (pStream->text != NULL &&
        pStream->text[0] != '\0')
    {
      //unicode?
      pStream->currentChar = pStream->text[0];
    }

    else
    {
      pStream->currentChar = '\0';
    }
  }

  return result;
}

Result SStream_Init(SStream* pStream, const char* name, const char*  text)
{
  pStream->currentLine = 1;
  pStream->currentCol = 1;
  pStream->position = 0;
  String_InitWith(&pStream->text, text);
  String_InitWith(&pStream->NameOrFullPath, name);
  String_InitWith(&pStream->FullDir2, "");

  if (text != NULL)
  {
    pStream->text_length = strlen(text);
  }

  else
  {
    pStream->text_length = 0;
  }

  if (pStream->text != NULL &&
      pStream->text[0] != '\0')
  {
    //unicode?
    pStream->currentChar = pStream->text[0];
  }

  else
  {
    pStream->currentChar = '\0';
  }

  return RESULT_OK;
}


void SStream_Destroy(SStream* pStream) /*default*/
{
    String_Destroy(&pStream->NameOrFullPath);
    String_Destroy(&pStream->FullDir2);
    String_Destroy(&pStream->text);
}


wchar_t SStream_LookAhead(SStream* pStream)
{
  if (pStream->position + 1 >= pStream->text_length)
  {
    return '\0';
  }

  return pStream->text[pStream->position + 1];
}

bool SStream_MatchChar(SStream* pStream, wchar_t ch)
{
    bool b = pStream->currentChar == ch;
    SStream_Match(pStream);
    return b;
}

void SStream_Match(SStream* pStream)
{
  if (pStream->position >= pStream->text_length)
  {
    pStream->currentChar = L'\0';
    return;
  }

  pStream->currentCol++;
  pStream->position++;

  if (pStream->position == pStream->text_length)
  {
    pStream->currentChar = '\0';
  }

  else
  {
    pStream->currentChar = pStream->text[pStream->position];
  }

  if (pStream->currentChar == '\n')
  {
    pStream->currentLine++;
    pStream->currentCol = 0;
  }
}

