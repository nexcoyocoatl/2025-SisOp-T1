#include <stdio.h>
#include <stdlib.h>

#define INT_DYNARRAY_INIT_SIZE 4

#define INT_DYNARRAY_INIT(da) int_dynarray da;\
                          init(&da)

typedef struct
{
    int *m_int_items;
    size_t m_size;
    size_t m_capacity;
} s_int_dynarraylist;

typedef struct s_int_dynarray int_dynarray;
struct s_int_dynarray
{
    s_int_dynarraylist int_dynarray_list;
    size_t (*size) (int_dynarray *);
    void (*resize) (int_dynarray *, size_t);
    void (*add) (int_dynarray *, int);
    void (*set) (int_dynarray *, size_t, int);
    int (*get) (int_dynarray *, size_t);
    void (*del) (int_dynarray *, size_t);
    void (*free) (int_dynarray *);
};

size_t dynarray_size(int_dynarray *da)
{
    if (da)
    {
        return da->int_dynarray_list.m_size;
    }

    return 0;
}

void dynarray_resize(int_dynarray *da, size_t capacity)
{
    if (da)
    {
        int *items = realloc(da->int_dynarray_list.m_int_items, sizeof(int *) * capacity);
        if (items)
        {
            da->int_dynarray_list.m_int_items = items;
            da->int_dynarray_list.m_capacity = capacity;
        }
    }
}

void dynarray_add(int_dynarray *da, int item)
{
    if (da)
    {
        if (da->int_dynarray_list.m_capacity == da->int_dynarray_list.m_size)
        {
            dynarray_resize( da, da->int_dynarray_list.m_capacity <<= 1 );
        }

        da->int_dynarray_list.m_int_items[da->int_dynarray_list.m_size++] = item;
    }
}

void dynarray_set(int_dynarray *da, size_t index, int item)
{
    if (da)
    {
        if ( (index >= 0) && (index < da->int_dynarray_list.m_size) )
        {
            da->int_dynarray_list.m_int_items[index] = item;
        }
    }
}

int dynarray_get(int_dynarray *da, size_t index)
{
    int data = 0;

    if (da)
    {
        if ( (index>=0) && (index < da->int_dynarray_list.m_size) )
        {
            data = da->int_dynarray_list.m_int_items[index];
        }
    }

    return data;
}

void dynarray_del(int_dynarray *da, size_t index)
{
    size_t i = 0;
    if(da)
    {
        if ( (index < 0) || (index >= da->int_dynarray_list.m_size) )
        {
            return;
        }

        da->int_dynarray_list.m_int_items[index] = 0;

        for (i = index; (i < da->int_dynarray_list.m_size - 1); i++)
        {
            da->int_dynarray_list.m_int_items[i] = da->int_dynarray_list.m_int_items[i+1];
        }
        da->int_dynarray_list.m_int_items[da->int_dynarray_list.m_size] = 0;

        da->int_dynarray_list.m_size--;

        if ( (da->int_dynarray_list.m_size > 0) && ((da->int_dynarray_list.m_size) == ((da->int_dynarray_list.m_capacity) >>= 2 )) )
        {
            dynarray_resize(da, da->int_dynarray_list.m_capacity >> 1);
        }
    }
}

void dynarray_free(int_dynarray *da)
{
    if (da)
    {
        free(da->int_dynarray_list.m_int_items);
        da->int_dynarray_list.m_int_items = NULL;
    }
}

void init(int_dynarray *da)
{
    da->size = dynarray_size;
    da->resize = dynarray_resize;
    da->add = dynarray_add;
    da->set = dynarray_set;
    da->get = dynarray_get;
    da->del = dynarray_del;
    da->free = dynarray_free;

    da->int_dynarray_list.m_capacity = INT_DYNARRAY_INIT_SIZE;
    da->int_dynarray_list.m_size = 0;
    da->int_dynarray_list.m_int_items = malloc( sizeof(int) * da->int_dynarray_list.m_capacity );

    if (!da->int_dynarray_list.m_int_items)
    {
        exit(EXIT_FAILURE);
    }
}

void test()
{
    INT_DYNARRAY_INIT(da);

    da.add(&da, 1);
    da.add(&da, 2);
    da.add(&da, 3);

    // Print elements through casting
    for (size_t i = 0; i < da.size(&da); i++)
    {
        printf("%d\n", da.get(&da, i));
    }
}

int main(int argc, char *argv[])
{
    test();

    return 0;
}