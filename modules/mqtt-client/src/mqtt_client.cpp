#include "MQTTClient.hpp"

// ---

#ifdef BUILD_PD_OBJECT
extern "C" {
#include "m_pd.h"

using t_comp_outlet = t_outlet;

using t_comp_newmethod = t_newmethod;
using t_comp_method = t_method;

inline t_comp_outlet* comp_outlet_new(t_object* x, const char* s) { return (t_comp_outlet*)::outlet_new(x, gensym(s)); }
inline void comp_outlet_delete(t_comp_outlet* x) { ::outlet_free(x); }

inline void comp_class_add_method(t_class* c, t_comp_method m, const char* n) { class_addmethod(c, m, gensym(n), A_GIMME); }
inline t_class* comp_class_new(const char* name, t_comp_newmethod n, t_comp_method f, size_t s)
{
    return class_new(gensym(name), n, f, s, 0, A_GIMME, 0);
}

#define COMP_OBJECT_NEW pd_new
#define COMP_SYMBOL A_SYMBOL
#define COMP_W_SYMBOL w_symbol
#define COMP_LONG A_FLOAT
#define COMP_W_LONG w_float

#define COMP_T_ARGC int

#define outlet_int outlet_float
}
#endif

#ifdef BUILD_MAX_OBJECT
#include "max-compatibility.h"

extern "C" {

#include "ext.h"

using t_comp_outlet = outlet;

using t_comp_newmethod = method;
using t_comp_method = method;

inline t_comp_outlet* comp_outlet_new(void* x, const char* s) { return (t_comp_outlet*)::outlet_new(x, s); }
inline void comp_outlet_delete(void* x) { ::outlet_delete(x); }

inline void comp_class_add_method(t_class* c, t_comp_method m, const char* n) { class_addmethod(c, m, n, A_GIMME, 0); }
inline t_class* comp_class_new(const char* name, t_comp_newmethod n, t_comp_method f, size_t s)
{
    return class_new(name, n, f, s, 0L, A_GIMME, 0);
};

#define COMP_OBJECT_NEW object_alloc
#define COMP_SYMBOL A_SYM
#define COMP_W_SYMBOL w_sym
#define COMP_LONG A_LONG
#define COMP_W_LONG w_long

#define COMP_T_ARGC long
};

#endif

// -----

t_class* mqtt_client_class = nullptr;

struct t_mqtt_client {
    t_object obj;

    std::unique_ptr<MQTTClient> client; ///< must be on heap

    t_comp_outlet* out1 = nullptr;
    t_comp_outlet* out2 = nullptr;

    std::unordered_map<std::string, std::string> publishedValues;
};

// ---

static void mqtt_client_connect(t_mqtt_client* x, t_symbol* s, COMP_T_ARGC argc, t_atom* argv)
{
    if (argc < 2) {
        error("bad arguments (count: %i)", argc);
        return;
    }

    if (argv[0].a_type != COMP_SYMBOL) {
        error("bad argument (1) type");
        return;
    }
    if (argv[1].a_type != COMP_SYMBOL) {
        error("bad argument (2) type");
        return;
    }

    std::string userName;
    std::string password;

    if (argc >= 4)
    {
        if (argv[2].a_type == COMP_SYMBOL) {
            userName = argv[2].a_w.COMP_W_SYMBOL->s_name;
        }
        if (argv[3].a_type == COMP_SYMBOL) {
            password = argv[3].a_w.COMP_W_SYMBOL->s_name;
        }
    }

    // ---
    post("MQTT Client: connecting to ... %s", argv[0].a_w.COMP_W_SYMBOL->s_name);

    if (!x->client->connect(std::string(argv[0].a_w.COMP_W_SYMBOL->s_name), 1883, std::string(argv[1].a_w.COMP_W_SYMBOL->s_name), userName, password)) {

        t_atom a;
        a.a_type = COMP_LONG;
        a.a_w.COMP_W_LONG = 0; ////gensym(value.c_str());
        outlet_list(x->out2, gensym("connected"), 1, &a);

        error("MQTT Client: %s", x->client->getLastError().c_str());
        return;
    }
}

static void mqtt_client_subscribe(t_mqtt_client* x, t_symbol* s, COMP_T_ARGC argc, t_atom* argv)
{
    if (argc < 1) {
        error("bad arguments (count: %i)", argc);
        return;
    }

    if (argv[0].a_type != COMP_SYMBOL) {
        error("bad argument (0) type");
        return;
    }

    // ---
    std::string key = std::string(argv[0].a_w.COMP_W_SYMBOL->s_name);
    auto subscribeResult = x->client->subscribe(std::string(argv[0].a_w.COMP_W_SYMBOL->s_name), [=](const std::string& value) {
        bool isNumber = true;

        float floatValue = 0;
        try{
        floatValue = std::stof(value);
        }catch(std::exception&){
            isNumber = false;
        }
        long intValue = 0;
        try{
        intValue= std::stol(value);
        }catch(std::exception&){
            isNumber = false;
        }

        // type check
        bool isInteger = (floatValue - int(floatValue)) == 0;

        if (isNumber) {
            if (isInteger) {
                t_atom a;
                a.a_type = COMP_LONG;
                a.a_w.COMP_W_LONG = intValue;
                outlet_anything(x->out1, gensym(key.c_str()), 1, &a);
            } else {
                t_atom a;
                a.a_type = A_FLOAT;
                a.a_w.w_float = floatValue;
                outlet_anything(x->out1, gensym(key.c_str()), 1, &a);
            }
        } else {
            t_atom a[2];
            a[0].a_type = COMP_SYMBOL;
            a[0].a_w.COMP_W_SYMBOL = gensym(key.c_str());
            a[1].a_type = COMP_SYMBOL;
            a[1].a_w.COMP_W_SYMBOL = gensym(value.c_str());
            outlet_anything(x->out1, gensym(key.c_str()), 2, &a[0]);
        }
    });

    if (subscribeResult)
        post("MQTT Client: subscribed to '%s'", argv[0].a_w.COMP_W_SYMBOL->s_name);
    else
        error("MQTT Client: %s", x->client->getLastError().c_str());

    t_atom a;
    a.a_type = COMP_LONG;
    a.a_w.COMP_W_LONG = subscribeResult ? 1 : 0;
    outlet_anything(x->out2, gensym("subscribe"), 1, &a);
}

static void mqtt_client_unsubscribe(t_mqtt_client* x, t_symbol* s, COMP_T_ARGC argc, t_atom* argv)
{
    if (argc < 1) {
        error("bad arguments (count: %i)", argc);
        return;
    }

    if (argv[0].a_type != COMP_SYMBOL) {
        error("bad argument (0) type");
        return;
    }

    // ---
    post("MQTT Client: unsubscribed from '%s'", argv[0].a_w.COMP_W_SYMBOL->s_name);

    x->client->unsubscribe(argv[0].a_w.COMP_W_SYMBOL->s_name);
}

static void mqtt_client_publish(t_mqtt_client* x, t_symbol* s, COMP_T_ARGC argc, t_atom* argv)
{
    if (argc < 2) {
        error("bad arguments (count: %i)", argc);
        return;
    }

    if (argv[0].a_type != COMP_SYMBOL) {
        error("bad argument (0) type");
        return;
    }

    if ((argv[1].a_type != COMP_SYMBOL)
        && (argv[1].a_type != COMP_LONG)
        && (argv[1].a_type != A_FLOAT)) {
        error("bad argument (1) type");
        return;
    }

    // ---
    auto k = std::string(argv[0].a_w.COMP_W_SYMBOL->s_name);
    auto v = (argv[1].a_type == COMP_SYMBOL) ? std::string(argv[1].a_w.COMP_W_SYMBOL->s_name)
        : (argv[1].a_type == A_FLOAT)
        ? std::to_string((argv[1].a_w.w_float))
                                                   : std::to_string(int(argv[1].a_w.COMP_W_LONG));
    x->publishedValues[k] = v;

    auto publishResult = x->client->publish(k, x->publishedValues[k]);
    if (!publishResult) {
        error("MQTT Client: %s", x->client->getLastError().c_str());
    }

    t_atom a;
    a.a_type = COMP_LONG;
    a.a_w.COMP_W_LONG = publishResult ? 1 : 0; ////gensym(value.c_str());
    outlet_anything(x->out2, gensym("publish_result"), 1, &a);
}

// ---

static void* mqtt_client_new(t_symbol* s, COMP_T_ARGC argc, t_atom* argv)
{
    t_mqtt_client* x = (t_mqtt_client*)COMP_OBJECT_NEW(mqtt_client_class);

    x->out1 = comp_outlet_new((t_object*)x, "list");
    x->out2 = comp_outlet_new((t_object*)x, "list");

    // Max outlet order fix
    #ifdef BUILD_MAX_OBJECT
    x->out1 = (t_comp_outlet*)outlet_nth((t_object*)x, 0);
    x->out2 = (t_comp_outlet*)outlet_nth((t_object*)x, 1);
    #endif

    x->client.reset(new MQTTClient());

    x->client->setOnConnected([=](const std::string& value) {
        t_atom a;
        a.a_type = COMP_LONG;
        a.a_w.COMP_W_LONG = 1;
        outlet_anything(x->out2, gensym("connected"), 1, &a);
    });

    x->client->setOnConnectionLost([=](const std::string& value) {
        outlet_anything(x->out2, gensym("connection_lost"), 0, 0);
    });

    return x;
}

static void mqtt_client_free(t_mqtt_client* x)
{
    comp_outlet_delete(x->out1);
    comp_outlet_delete(x->out2);
}

extern "C" {

void mqtt_client_setup(void)
{

    mqtt_client_class = comp_class_new("mqtt_client", (t_comp_newmethod)mqtt_client_new, (t_comp_method)mqtt_client_free, sizeof(t_mqtt_client));

    comp_class_add_method(mqtt_client_class, (t_comp_method)&mqtt_client_connect, "connect");
    comp_class_add_method(mqtt_client_class, (t_comp_method)&mqtt_client_subscribe, "subscribe");
    comp_class_add_method(mqtt_client_class, (t_comp_method)&mqtt_client_unsubscribe, "unsubscribe");
    comp_class_add_method(mqtt_client_class, (t_comp_method)&mqtt_client_publish, "publish");

#ifdef BUILD_MAX_OBJECT
    class_register(CLASS_BOX, mqtt_client_class);
#endif
}

void ext_main(void* r)
{
    mqtt_client_setup();
}
}
