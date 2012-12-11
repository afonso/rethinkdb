#include <rdb_protocol/ql2.hpp>

namespace ql {

static datum_t add(const datum_t &lhs, const datum_t &rhs) {
    runtime_check(lhs.get_type() == rhs.get_type(),
                  strprintf("Cannot add %s to %s (types differ).",
                            lhs.print().c_str(), rhs.print().c_str()));
    if (lhs.get_type() == datum_t::R_NUM) {
        return datum_t(lhs.as_num() + rhs.as_num());
    } else if (lhs.get_type() == datum_t::R_STR) {
        return datum_t(lhs.as_str() + rhs.as_str());
    }

    runtime_fail("Cannot add objects of type %s (e.g. %s).",
                 lhs.get_type_name(), lhs.print().c_str());
    unreachable();
}

static datum_t sub(const datum_t &lhs, const datum_t &rhs) {
    lhs.check_type(datum_t::R_NUM);
    rhs.check_type(datum_t::R_NUM);
    return datum_t(lhs.as_num() - rhs.as_num());
}
static datum_t mul(const datum_t &lhs, const datum_t &rhs) {
    lhs.check_type(datum_t::R_NUM);
    rhs.check_type(datum_t::R_NUM);
    return datum_t(lhs.as_num() * rhs.as_num());
}
static datum_t div(const datum_t &lhs, const datum_t &rhs) {
    lhs.check_type(datum_t::R_NUM);
    rhs.check_type(datum_t::R_NUM);
    return datum_t(lhs.as_num() / rhs.as_num());
}

arith_term_t::arith_term_t(env_t *env, const Term2 *term)
    : simple_op_term_t(env, term), namestr(0),  op(0) {
    int arithtype = term->type();
    switch(arithtype) {
    case Term2_TermType_ADD: namestr = "ADD"; op = &add; break;
    case Term2_TermType_SUB: namestr = "SUB"; op = &sub; break;
    case Term2_TermType_MUL: namestr = "MUL"; op = &mul; break;
    case Term2_TermType_DIV: namestr = "DIV"; op = &div; break;
    default:unreachable();
    }
    guarantee(namestr && op);
}

arith_term_t::~arith_term_t() { }

val_t *arith_term_t::simple_call_impl(env_t *env, std::vector<val_t *> *args) {
    runtime_check(args->size() >= 1,
                  strprintf("%s requires at least one argument.", name()));
    scoped_ptr_t<datum_t> acc(new datum_t());
    *acc.get() = *(*args)[0]->as_datum();
    for (size_t i = 1; i < args->size(); ++i) {
        *acc.get() = (*op)(*acc.get(), *(*args)[i]->as_datum());
    }
    return env->new_val(acc.release());
}

const char *arith_term_t::name() const { return namestr; }


} //namespace ql
