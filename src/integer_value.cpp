#include "natalie.hpp"

#include <math.h>
#include <string>

namespace Natalie {

Value *IntegerValue::to_s(Env *env) {
    auto str = std::to_string(m_integer);
    return new StringValue { env, str };
}

Value *IntegerValue::to_i() {
    return this;
}

Value *IntegerValue::add(Env *env, Value *arg) {
    if (arg->is_float()) {
        double result = to_nat_int_t() + arg->as_float()->to_double();
        return new FloatValue { env, result };
    }
    arg->assert_type(env, Value::Type::Integer, "Integer");
    nat_int_t result = to_nat_int_t() + arg->as_integer()->to_nat_int_t();
    return new IntegerValue { env, result };
}

Value *IntegerValue::sub(Env *env, Value *arg) {
    if (arg->is_float()) {
        double result = to_nat_int_t() - arg->as_float()->to_double();
        return new FloatValue { env, result };
    }
    arg->assert_type(env, Value::Type::Integer, "Integer");
    nat_int_t result = to_nat_int_t() - arg->as_integer()->to_nat_int_t();
    return new IntegerValue { env, result };
}

Value *IntegerValue::mul(Env *env, Value *arg) {
    if (arg->is_float()) {
        double result = to_nat_int_t() * arg->as_float()->to_double();
        return new FloatValue { env, result };
    }
    arg->assert_type(env, Value::Type::Integer, "Integer");
    nat_int_t result = to_nat_int_t() * arg->as_integer()->to_nat_int_t();
    return new IntegerValue { env, result };
}

Value *IntegerValue::div(Env *env, Value *arg) {
    if (arg->is_integer()) {
        nat_int_t dividend = to_nat_int_t();
        nat_int_t divisor = arg->as_integer()->to_nat_int_t();
        if (divisor == 0) {
            env->raise("ZeroDivisionError", "divided by 0");
        }
        nat_int_t result = dividend / divisor;
        return new IntegerValue { env, result };

    } else if (arg->respond_to(env, "coerce")) {
        Value *args[] = { this };
        Value *coerced = arg->send(env, "coerce", 1, args, nullptr);
        Value *dividend = (*coerced->as_array())[0];
        Value *divisor = (*coerced->as_array())[1];
        return dividend->send(env, "/", 1, &divisor, nullptr);

    } else {
        arg->assert_type(env, Value::Type::Integer, "Integer");
        abort();
    }
}

Value *IntegerValue::mod(Env *env, Value *arg) {
    arg->assert_type(env, Value::Type::Integer, "Integer");
    nat_int_t result = to_nat_int_t() % arg->as_integer()->to_nat_int_t();
    return new IntegerValue { env, result };
}

Value *IntegerValue::pow(Env *env, Value *arg) {
    arg->assert_type(env, Value::Type::Integer, "Integer");
    nat_int_t result = ::pow(to_nat_int_t(), arg->as_integer()->to_nat_int_t());
    return new IntegerValue { env, result };
}

Value *IntegerValue::cmp(Env *env, Value *arg) {
    if (arg->type() != Value::Type::Integer) return env->nil_obj();
    nat_int_t i1 = to_nat_int_t();
    nat_int_t i2 = arg->as_integer()->to_nat_int_t();
    if (i1 < i2) {
        return new IntegerValue { env, -1 };
    } else if (i1 == i2) {
        return new IntegerValue { env, 0 };
    } else {
        return new IntegerValue { env, 1 };
    }
}

bool IntegerValue::eq(Env *env, Value *other) {
    if (other->is_integer()) {
        return to_nat_int_t() == other->as_integer()->to_nat_int_t();
    } else if (other->is_float()) {
        return to_nat_int_t() == other->as_float()->to_double();
    }
    return false;
}

bool IntegerValue::lt(Env *env, Value *other) {
    if (other->is_integer()) {
        return to_nat_int_t() < other->as_integer()->to_nat_int_t();
    } else if (other->is_float()) {
        return to_nat_int_t() < other->as_float()->to_double();
    }
    env->raise("ArgumentError", "comparison of Integer with %s failed", other->inspect_str(env));
}

bool IntegerValue::lte(Env *env, Value *other) {
    if (other->is_integer()) {
        return to_nat_int_t() <= other->as_integer()->to_nat_int_t();
    } else if (other->is_float()) {
        return to_nat_int_t() <= other->as_float()->to_double();
    }
    env->raise("ArgumentError", "comparison of Integer with %s failed", other->inspect_str(env));
}

bool IntegerValue::gt(Env *env, Value *other) {
    if (other->is_integer()) {
        return to_nat_int_t() > other->as_integer()->to_nat_int_t();
    } else if (other->is_float()) {
        return to_nat_int_t() > other->as_float()->to_double();
    }
    env->raise("ArgumentError", "comparison of Integer with %s failed", other->inspect_str(env));
}

bool IntegerValue::gte(Env *env, Value *other) {
    if (other->is_integer()) {
        return to_nat_int_t() >= other->as_integer()->to_nat_int_t();
    } else if (other->is_float()) {
        return to_nat_int_t() >= other->as_float()->to_double();
    }
    env->raise("ArgumentError", "comparison of Integer with %s failed", other->inspect_str(env));
}

Value *IntegerValue::eqeqeq(Env *env, Value *arg) {
    if (arg->type() == Value::Type::Integer && to_nat_int_t() == arg->as_integer()->to_nat_int_t()) {
        return env->true_obj();
    } else {
        return env->false_obj();
    }
}

Value *IntegerValue::times(Env *env, Block *block) {
    nat_int_t val = to_nat_int_t();
    assert(val >= 0);
    env->assert_block_given(block); // TODO: return Enumerator when no block given
    Value *num;
    for (long long i = 0; i < val; i++) {
        num = new IntegerValue { env, i };
        NAT_RUN_BLOCK_AND_POSSIBLY_BREAK(env, block, 1, &num, nullptr);
    }
    return this;
}

Value *IntegerValue::bitwise_and(Env *env, Value *arg) {
    arg->assert_type(env, Value::Type::Integer, "Integer");
    return new IntegerValue { env, to_nat_int_t() & arg->as_integer()->to_nat_int_t() };
}

Value *IntegerValue::bitwise_or(Env *env, Value *arg) {
    arg->assert_type(env, Value::Type::Integer, "Integer");
    return new IntegerValue { env, to_nat_int_t() | arg->as_integer()->to_nat_int_t() };
}

Value *IntegerValue::succ(Env *env) {
    return new IntegerValue { env, to_nat_int_t() + 1 };
}

Value *IntegerValue::coerce(Env *env, Value *arg) {
    ArrayValue *ary = new ArrayValue { env };
    switch (arg->type()) {
    case Value::Type::Float:
        ary->push(arg);
        ary->push(new FloatValue { env, to_nat_int_t() });
        break;
    case Value::Type::Integer:
        ary->push(arg);
        ary->push(this);
        break;
    case Value::Type::String:
        printf("TODO\n");
        abort();
        break;
    default:
        env->raise("ArgumentError", "invalid value for Float(): %S", arg->inspect_str(env));
    }
    return ary;
}

bool IntegerValue::eql(Env *env, Value *other) {
    return other->is_integer() && other->as_integer()->to_nat_int_t() == to_nat_int_t();
}

Value *IntegerValue::abs(Env *env) {
    auto number = to_nat_int_t();
    if (number < 0) {
        return new IntegerValue { env, -1 * number };
    } else {
        return this;
    }
}

Value *IntegerValue::chr(Env *env) {
    char c = static_cast<char>(to_nat_int_t());
    char str[] = " ";
    str[0] = c;
    return new StringValue { env, str };
}

}
