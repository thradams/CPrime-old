# C'

![robot](/robots.jpg)
(Picture courtesy of my niece Bethina)

## Description

Have you ever imagined in do pair programming with an robot? Now you can.

C' (pronounced c prime) is C language transpiler that read comments in your code and can be responsable to implement and maintain some parts of your code.

* It can generate constructors and destructors 
* it can generate the operator new and delete (similarly of C++).
* It can generate polimorphic functions.
* It can generate containers like vector, list
* It can help with lamddas and initialization

The best introduction is to try the samples online.
http://www.thradams.com/web/cprime.html

CPrime is written using itself. Some files are 60% generated, see Ast.h and Ast.c.

## Why?

Using this tool you achieve **decoupling** because you don´t have to implement the mechanical relationship between objects.

This mechanical relatioship also makes possible a **new kind of polimorphism where the coupling is much smaller** than the traditional interface (virtual) methodoly.

You can compare this code generation with C++ explicity template instantiation.
But C' instanciates just one function and always explicity.


## Features 

### Especial functions
The compiler can generate something similar of C++ constructor,destructor, operator new and operator delete.

To generate these functions use the comment /*default*/ at the end of function declaration. 

The name of the functions and signature are used to undestand what you want to generate. 

Use XXX_Create for "operator new", XXX_Init for constructor, XXX_Destroy for destructor and XXX_Delete for operator delete. 


Sample input with comments:

```c

typedef char * /*auto*/ String;

struct X
{
    String Name;
    int i;
};

struct X * X_Create() /*default*/
{
    struct X *p = (struct X *) malloc(sizeof * p);
    if (p)
    {
        X_Init(p);
    }
    return p;
}

void X_Init(struct X * p) /*default*/
{
    p->Name = 0;
    p->i = 0;
}

void X_Destroy(struct X * p) /*default*/
{
    free((void*)p->Name);
}

void X_Delete(struct X * p) /*default*/
{
    if (p)
    {
        X_Destroy(p);
        free((void*)p);
    }
}

int main()
{
    struct X x = /*default*/{0};
    return 1;
}


```
The **/*auto*/** type qualifier is a qualifier that can be applied to pointers. When a pointer has auto it means that the pointer
is the **owner of the pointed object**. This information is used to generate destructors.
See the declaration of String and the generated destructor X_Destroy.


### Dynamic Arrays (like std::vector)
Instead of using templates, we define a concrete data structure.
Two functions can be created 'PushBack' and 'Reserve' and the normal 'Destroy'.

When the algorithm 'PushBack' is instaciated it will check the type. If the type looks like an vector (pointer, size and capacity) then the function will be instaciated having that in mind. The same function, let´s say PushBack can be instanciated for list.

Differently from C++, one instanciation will not instantiate another one.
But one instantion can use another one if present. Comment 'Reserve' and compare.
```c

struct Items
{
	int * /*auto [Size]*/ pData;
	int Size;
	int Capacity;
};


void Items_Reserve(Items* pItems, int n) /*default*/;
void Items_PushBack(Items* pItems, int i) /*default*/;

void Items_Destroy(Items* pItems) /*default*/;


int main(int argc, char **argv)
{
	Items items = /*default*/{0};

	Items_PushBack(&items, 1);
	Items_PushBack(&items, 2);
	Items_PushBack(&items, 3);

	for (int i = 0; i < items.Size; i++)
	{
		printf("%d\n", items.pData[i]);
	}

	Items_Destroy(&items);
	return 0;
}

```

### Initialization
Struct data members can have initializers. This initializers are used to generate special functions and for the default initialization.

```c

struct Point
{
  int x /*= 1*/;
  int y /*= 2*/;
};

struct Line
{
  struct Point start, end;
};

int main()
{
  struct Point pt = /*default*/{/*.x=*/ 1, /*.y=*/ 2};
  struct Line ln = /*default*/{{/*.x=*/ 1, /*.y=*/ 2}, {/*.x=*/ 1, /*.y=*/ 2}};
}


```
### Polimorphism

```c


struct Box
{
    int id /*= 1*/;
};

struct Box* Box_Create() /*default*/
{
    struct Box* p = (struct Box*) malloc(sizeof * p);
    if (p)
    {
        p->id =  1;
    }
    return p;
}
void Box_Delete(struct Box* pBox) /*default*/
{
    if (pBox)
    {
        free((void*)pBox);
    }
}

void Box_Draw(struct Box* pBox)
{
    printf("Box");
}

struct Circle
{
    int id /*= 2*/;
};
struct Circle* Circle_Create() /*default*/
{
    struct Circle* p = (struct Circle*) malloc(sizeof * p);
    if (p)
    {
        p->id =  2;
    }
    return p;
}
void Circle_Delete(struct Circle* pCircle) /*default*/
{
    if (pCircle)
    {
        free((void*)pCircle);
    }
}

void Circle_Draw(struct Circle* pCircle)
{
    printf("Circle");
}

//Shape is a pointer to Box or Circle
struct /*Box | Circle*/ Shape
{
    int id;
};

void Shape_Delete(struct Shape* pShape) /*default*/
{
    if (pShape)
    {
            switch (pShape->id)
            {
                case  2:
                    Circle_Delete((struct Circle*)pShape);
                break;
                case  1:
                    Box_Delete((struct Box*)pShape);
                break;
                default:
                break;
            }
    }
}

void Shape_Draw(struct Shape* pShape) /*default*/
{
    switch (pShape->id)
    {
        case  2:
            Circle_Draw((struct Circle*)pShape);
        break;
        case  1:
            Box_Draw((struct Box*)pShape);
        break;
        default:
        break;
    }
}
```


### Lambdas 
Lambdas without capture are implemented using C++ syntax.
This is one way operation.

Input

```c

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

```

Output
```c

void Run(void (*callback)(void*), void* data);

static void _lambda_1(void* data){
      printf("second");
    }

static void _lambda_0(void* data){
  
    printf("first");
    Run(_lambda_1, 0);     
  }

int main()
{  
  Run(_lambda_0, 0);
}

```

## Next steps

[Using](usingcp.md) the compiler.


## Background & Motives

I work professionally with C++ since 2000 keeping my knowledge about C++ updated. After 2014 I noticed that my interest for the C language was gradually increasing. 

In 2012, I opened a topic in com.lang.C++.moderated about the possibility of to have “smart pointers” inside the C++ type system and later in 2015 I watched the Herb Sutter presentation cppcon “Writing Good C++14 By Default" where he talks about memory ownership checked statically.  After this point I became very motivated to implement the checked ownership analysis in the C language as part of the type system and I started to write my C parser in C. 

During the development of this parser and static analysis, I had some difficulties with the C language that have already begin addressed by C++, like to generate special member functions. I was doing constructors and destructor by hand and wondering how they could be designed and used in C. 

At some point I decided that I should address the problem to work better with C before to do the static analysis and then I renamed the project to C’ cprime.  Because I want a tool to be useful now (not something experimental) I spent some time to solve the problem of generating C code from C code including the preprocessed parts. This allowed me to use existing C compilers and IDEs.
C’ can generate destructors for structs and can have owner pointers in the type system. The motivation for static analysis still there and it is also related with code generation. More motivations like containers and polymorphism are included in the C’ as well.

## Current status

I still having some problem to parse (see issues)  and I am fixing these problems parsing existing code like oppenssl, sqlite, duktape, opcua ansi stack, tiny c threads. (Basicaly libs I use)

I also need to make the code compatible with VC++ to parse some code, for instance, integer suffixes i64 etc.

## Roadmap

At some point I want to include static analysis again and check the onwership and null pointers as part of the type system. 
 
See:
https://github.com/isocpp/CppCoreGuidelines/blob/master/docs/Lifetime.pdf
