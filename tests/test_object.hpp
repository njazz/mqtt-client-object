// mock for pd/max headers for testing

// 1. macros

#define COMP_OBJECT_NEW pd_new

#define COMP_SYMBOL 1
#define COMP_LONG 2
#define A_FLOAT 3

#define COMP_W_SYMBOL w_symbol
#define COMP_W_LONG w_float

#define COMP_T_ARGC int

//#define

// 2. types

using t_class = void;

// 2a structs
struct t_object {
};
struct t_symbol {
    const char* s_name = nullptr;
};

struct t_atom {
    size_t a_type;
    union a_w {
        t_symbol* w_symbol = nullptr;
        float w_float;
    } a_w;
};

using t_comp_outlet = void;
using t_comp_newmethod = void*;
using t_comp_method = void*;

// 3. variadics - stub
#define post printf
#define error printf

// 4. fn

t_symbol* gensym(const char* c)
{
    auto ret = new t_symbol;
    ret->s_name = c;
    return ret;
}

inline void* outlet_anything(void* o, const t_symbol * s, short ac, const t_atom* av) { }

inline t_comp_outlet* comp_outlet_new(void* x, const char* s) { return nullptr; }
inline void comp_outlet_delete(void* x) { }

inline void comp_class_add_method(t_class* c, t_comp_method m, const char* n) { }
inline t_class* comp_class_new(const char* name, t_comp_newmethod n, t_comp_method f, size_t s)
{
    return nullptr;
};

void* pd_new(t_class* c) { return nullptr; }
