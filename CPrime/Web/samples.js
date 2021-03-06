var sample = {};

sample["Especial functions (auto tag)"] =
 `

typedef char * /*@auto*/ String;
struct X
{
    String Name;
    int i;
};

struct X * X_Create() /*@default*/;
void X_Init(struct X * p) /*@default*/;
void X_Destroy(struct X * p) /*@default*/;
void X_Delete(struct X * p) /*@default*/;

int main()
{
    struct X x = /*@default*/{0};
    return 1;
}
`;


sample["Especial functions C' (auto tag)"] =
    `

typedef char * auto String;
struct X
{
    String Name;
    int i;
};

struct X * X_Create() default;
void X_Init(struct X * p) default;
void X_Destroy(struct X * p) default;
void X_Delete(struct X * p) default;

int main()
{
    struct X x = {};
    return 1;
}
`;

sample["Especial functions with function tags"] =
    `

struct X
{
    char* auto Name;
    int i;
};

//tagged functions

struct X * makeX() : create;
void initX(struct X * p) : init;
void destroyX(struct X * p) : destroy;
void deleteX(struct X * p) : delete;

int main()
{
    struct X x = {};
    return 1;
}


//it is not necessary to tag again

struct X * makeX() default;

void initX(struct X * p) default;

void destroyX(struct X * p) default;

void deleteX(struct X * p) default;


`;

sample["Decopling (auto tag)"] =
`
// -- header file -- 

typedef char * /*@auto*/ String;

struct X
{
    String Name;
    int i;
};

struct Y
{
  int i;
  struct X x; //try comment this line
}

void Y_Init(struct Y * p);
void Y_Destroy(struct Y * p);

int main()
{
    return 1;
}

//-- implementation file --

void Y_Init(struct Y * p) /*@default*/
{
    p->i = 0;
    p->x.Name = 0;
    p->x.i = 0;
}

void Y_Destroy(struct Y * p) /*@default*/
{
    free((void*)p->x.Name);
}

`;
sample["StrArray (auto tag)"] =
`
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>

struct StrArray
{
    const char* /*@auto*/* /*@auto*/ /*@[Size]*/ data;
    int Size;
    int Capacity;
};

void StrArray_Destroy(struct StrArray* p) /*@default*/
{
    for (int i = 0; i < p->Size; i++)
    {
        free((void*)p->data[i]);
    }
    free((void*)p->data);
}

void StrArray_PushBack(struct StrArray* p, const char* s) /*@default*/
{
    if (p->Size + 1 > p->Capacity)
    {
        int n = p->Capacity * 2;
        if (n == 0)
        {
            n = 1;
        }
        const char** pnew = p->data;
        pnew = (const char**)realloc(pnew, n * sizeof(const char*));
        if (pnew)
        {
            p->data = pnew;
            p->Capacity = n;
        }
    }
    p->data[p->Size] = s;
    p->Size++;
}

int main()
{
    struct StrArray strings = { 0 };
    
    StrArray_PushBack(&strings, _strdup("a"));
    StrArray_Destroy(&strings);

}
`;

sample["StrArray C' with function tags"] =
`

//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>

struct StrArray
{
    const char* auto* auto [Size] data;
    int Size;
    int Capacity;
};

void Destroy(struct StrArray* p) : destroy;
void Insert(struct StrArray* p, const char* s) : push;


int main()
{
    struct StrArray strings = { 0 };
    
    Insert(&strings, _strdup("a"));
    Destroy(&strings);

}

void Insert(struct StrArray* p, const char* s) default;
void Destroy(struct StrArray* p) : destroy default;

`;

sample["Dynamic Array of int (auto tag)"] =
    `
struct Items
{
	int * /*@auto [Size]*/ pData;
	int Size;
	int Capacity;
};


void Items_PushBack(struct Items* pItems, int i) /*@default*/;
void Items_Destroy(struct Items* pItems) /*@default*/;


int main(int argc, char **argv)
{
	struct Items items = /*@default*/{0};

	Items_PushBack(&items, 1);
	Items_PushBack(&items, 2);
	Items_PushBack(&items, 3);

	for (int i = 0; i < items.Size; i++)
	{
		printf("%d\\n", items.pData[i]);
	}

	Items_Destroy(&items);
	return 0;
}

`;

sample["Dynamic Array of Item* (auto tag)"] =
 `
struct Item
{
	int i;
};


struct Item* Item_Create() /*@default*/;
void Item_Delete(struct Item* p) /*@default*/;

struct Items
{
	struct Item * /*@auto*/ * /*@auto [Size]*/ pData;
	int Size;
	int Capacity;
};


void Items_PushBack(struct Items* pItems, struct Item* pItem) /*@default*/;
void Items_Destroy(struct Items* pItems) /*@default*/;


int main(int argc, char **argv)
{
	struct Items items = /*@default*/{0};

	Items_PushBack(&items, Item_Create());
	Items_PushBack(&items, Item_Create());
	Items_PushBack(&items, Item_Create());

	for (int i = 0; i < items.Size; i++)
	{
		printf("%d\\n", items.pData[i]->i);
	}

	Items_Destroy(&items);
	return 0;
}

`;

sample["Linked list (auto tag)"] =
`
struct Item
{
    int i;
    struct Item* /*@auto*/ pNext;
};

void Item_Delete(struct Item* pItem) /*@default*/;

struct Items
{
    struct Item* /*@auto*/ pHead,* pTail;
};

void Items_Destroy(struct Items* pItems) /*@default*/;
void Items_PushBack(struct Items* pItems, struct Item* pItem) /*@default*/;

`;

sample["Initialization"] =
`
struct Point
{
  int x /*@= 1*/;
  int y /*@= 2*/;
};

struct Line
{
  struct Point start, end;
};

int main()
{
  //C' will keep these initializers updated 
  //try with one file option changing the point default
  struct Point pt = /*@default*/{/*.x=*/ 1, /*.y=*/ 2};
  struct Line ln = /*@default*/{{/*.x=*/ 1, /*.y=*/ 2}, {/*.x=*/ 1, /*.y=*/ 2}};
}
`;

sample["Initialization C'"] =
    `

struct Point
{
  int x = 1;
  int y = 2;
};

struct Line
{
  struct Point start, end;
};

int main()
{
  struct Point pt = {};
  struct Line ln = {};
}

`;

sample["Polimorphism (auto tag)"] =
`
struct Box
{
    int id /*@= 1*/;
};

struct Box* Box_Create() /*@default*/;
void Box_Delete(struct Box* pBox) /*@default*/;

void Box_Draw(struct Box* pBox)
{
    printf("Box");
}

struct Circle
{
    int id /*@= 2*/;
};
struct Circle* Circle_Create() /*@default*/;
void Circle_Delete(struct Circle* pCircle) /*@default*/;

void Circle_Draw(struct Circle* pCircle)
{
    printf("Circle");
}

//Shape is a pointer to Box or Circle
struct /*@<Box | Circle>*/ Shape
{
    int id;
};

//polimorphic objects are not using tag yet
//they require the same function name to work
void Shape_Delete(struct Shape* pShape) /*@default*/;

//except for delete I am not sure if tags will be used
//because they can create coupling.
void Shape_Draw(struct Shape* pShape) /*@default*/;

`;


sample["Polimorphism C' (auto tag)"] =
    `
struct Box
{
    int id = 1;
};

struct Box* Box_Create() default;
void Box_Delete(struct Box* pBox) default;

void Box_Draw(struct Box* pBox)
{
    printf("Box");
}

struct Circle
{
    int id = 2;
};
struct Circle* Circle_Create() default;
void Circle_Delete(struct Circle* pCircle) default;

void Circle_Draw(struct Circle* pCircle)
{
    printf("Circle");
}

//Shape is a pointer to Box or Circle
struct <Box | Circle> Shape
{
    int id;
};

void Shape_Delete(struct Shape* pShape) default;

void Shape_Draw(struct Shape* pShape) default;

`;


sample["Lambdas"] =
    `
void Run(void (*callback)(void*), void* data);

int main()
{  
  Run([](void* data){
  
    printf("first");
    Run([](void* data){
      printf("second");
    }, 0);     
  }, 0);
}
`;

