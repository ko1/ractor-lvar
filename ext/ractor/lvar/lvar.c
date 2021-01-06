#include "ruby/ruby.h"
#include "ruby/ractor.h"

static VALUE rb_cRactorLVar;

// hidden C-API
void rb_ractor_local_storage_delkey(rb_ractor_local_key_t key);
VALUE rb_proc_isolate(VALUE proc);

struct ractor_lvar {
    rb_ractor_local_key_t key;
    VALUE default_proc;
};

static void
ractor_lvar_mark(void *ptr)
{
    struct ractor_lvar *lvar = (struct ractor_lvar *)ptr;
    rb_gc_mark(lvar->default_proc);
}

static void
ractor_lvar_free(void *ptr)
{
    struct ractor_lvar *lvar = (struct ractor_lvar *)ptr;
    rb_ractor_local_storage_delkey(lvar->key);
    ruby_xfree(lvar);
}

static size_t
ractor_lvar_memsize(const void *ptr)
{
    return sizeof(struct ractor_lvar);
}

static const rb_data_type_t ractor_lvar_data_type = {
    "ractor/lvar",
    {
        ractor_lvar_mark,
        ractor_lvar_free,
        ractor_lvar_memsize,
        NULL, // update
    },
    0, 0, RUBY_TYPED_FREE_IMMEDIATELY | RUBY_TYPED_WB_PROTECTED,
};

static VALUE
ractor_lvar_new(VALUE self)
{
    VALUE default_proc = rb_block_given_p() ? rb_block_proc() : Qnil;

    struct ractor_lvar *lvar;
    VALUE lv = TypedData_Make_Struct(rb_cRactorLVar, struct ractor_lvar, &ractor_lvar_data_type, lvar);
    lvar->key = rb_ractor_local_storage_value_newkey();
    lvar->default_proc = RTEST(default_proc) ? rb_proc_isolate(default_proc) : Qfalse;
    FL_SET_RAW(lv, RUBY_FL_SHAREABLE);
    return lv;
}

static struct ractor_lvar *
ractor_lvar_ptr(VALUE self)
{
    RUBY_ASSERT(rb_typeddata_is_kind_of(self, &ractor_lvar_data_type));
    return DATA_PTR(self);
}

static VALUE
ractor_lvar_get(VALUE self)
{
    rb_ractor_local_key_t key = ractor_lvar_ptr(self)->key;
    VALUE val;
    if (rb_ractor_local_storage_value(key, &val)) {
        return val;
    }
    else {
        VALUE default_proc = ractor_lvar_ptr(self)->default_proc;
        if (default_proc) {
            val = rb_proc_call_with_block(default_proc, 0, NULL, Qnil);
            rb_ractor_local_storage_value_set(key, val);
            return val;
        }
        else {
            return Qnil;
        }
    }
}

static VALUE
ractor_lvar_set(VALUE self, VALUE val)
{
    rb_ractor_local_key_t key = ractor_lvar_ptr(self)->key;
    rb_ractor_local_storage_value_set(key, val);
    return val;
}

void
Init_lvar(void)
{
    rb_ext_ractor_safe(true);
    rb_cRactorLVar = rb_define_class_under(rb_cRactor, "LVar", rb_cObject);
    rb_define_singleton_method(rb_cRactorLVar, "new", ractor_lvar_new, 0);
    rb_define_method(rb_cRactorLVar, "ractor_local_value", ractor_lvar_get, 0);
    rb_define_method(rb_cRactorLVar, "ractor_local_value=", ractor_lvar_set, 1);
}
