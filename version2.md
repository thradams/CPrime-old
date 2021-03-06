
## Description - Version 2 (not implemented yet)

C' is a extended C compiler that generates readably C code.

See it online:
http://www.thradams.com/web/cprime.html

## C language extensions tour

### Struct members with initializers

```c
struct Point
{
  int x = 1;
  int y = 2;
};
```

### Default initializer
```c
int main()
{
  struct Point pt = {}; //same as {.x = 1, .y = 2}
}
```

### Lambda Expression

This is similar of C++. The diference is that we don't capture and the result of the lambdas expression is always a pointer to function.

```c
void Run(void (*callback)(void*), void* data);
int main()
{  
  Run([](void* data) {
    printf("first");
    Run([](void* data){
      printf("second");
    }, 0);     
  }, 0);
}
```

### Operator new

```c
int main()
{
  struct X* pX = new (struct X) {};
}
```
Allocates the memory using malloc and initializes the object.
Returns null on error.

### Operator delete

```c
int main()
{
  struct X* pX = new (struct X) {};
  delete pX;
}
```

### Operator destroy

Operator destroy is called at the end of scope, unless you modifiy the type unsing 'view'.

```c
int main()
{
  struct X * pX = new (struct X){};
  if (pX)
  {
    destroy *pX;
    free(pX);
  }
}

int main()
{
  struct X x1 = {};
  view struct X x = x1;    
}//only destroy if x1 is called


int main()
{
  view struct X x1 = {};  
  destroy x1; //explict  
}//destroy is not called here

```
See auto for the behaviour of default destroy.

### Function/operator overload

Obs: Functions marked as 'overload' have their names mangled.

```c
struct X
{
   char * text;
};

void new(struct X* init) overload {}

void delete(struct X* init) overload {}

void destroy(struct X* p) overload {  free(p->text); }

void print(struct X* p) overload { }

```


### Auto pointer qualifier

Pointers can be qualified with the 'auto' keyword.

This tells the compiler that the pointer is the owner of the pointed object.

In pratice this is used for destroy and delete instantiation:

For instance:

```c
struct X {
    char * auto Text;
};

int main()
{  
  struct X x = {};
  x.Text = strdup("test"); 
} //destroy is called and free(x.Text) is called
```

```
### Especial comments
CPrime has especial comments that are ignored.

```c
/*@   and   */
```
These two sequences will be interpreted as spaces.

These special comments where created to allow C language extensions be created using normal C syntax.

For instance, this C code parsed by C compiler will see comments

```c
//file X.h
struct X {
    int i /*@= 3*/;
};
```
but cprime compiler will see:

```c
//file X.h
struct X {
    int i = 3;
};

```
>
> CPrime is compiled using itself. I use VC++ compiler and IDE with this annotation method.
> Without this method I would need a IDE plugin, and this is something I dont have at this time.
>
### Resizeble Arrays

```c


void F(int ar[auto])
{
}

int main()
{
  int ar[auto];

  reserve(ar, 2);

  push(ar, 1);
  push(ar, 2);

 for (int i = 0 ; i < ar.size; i++)
 {
   printf("%d\n", ar[i]);
 }

 printf("size = %d, capacity=%d\n", (int) ar.size, (int) ar.capacity);

 F(ar); //always passed as "reference"

}
```

### If with initializer (same of C++)

```c
   
    if (struct X* auto pX = new (struct X){}, pX)
    {
    }

```
### Runtime polymorphism

In C we have void * that means a pointer to any object.
In C' we can espcify a list of possible pointed objects.

Sample:

```c
struct Box {
    int id = 1;
};

void draw(struct Box* pBox) overload
{
}

struct Circle {
    int id = 2;
};

void draw(struct Circle* pCircle) overload
{
}


struct <Box | Circle> Shape;

struct Shape * p ...;

draw(p); //calls the correct overloaded function

```
## Template functions

A function with some type auto is automatially template and automatically overload.

Arguments are always passed as "reference" (There is no C++ references & here)
```c
void swap(auto a, auto b)
{
  decltype(a) temp = a;
  a = b;
  b = a;
}
```

## Parametrized types

```c
template<class T>
struct X
{
  T a;
}
```
