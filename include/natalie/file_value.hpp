#pragma once

#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "natalie/forward.hpp"
#include "natalie/integer_value.hpp"
#include "natalie/io_value.hpp"
#include "natalie/regexp_value.hpp"
#include "natalie/string_value.hpp"

namespace Natalie {

struct FileValue : IoValue {
    FileValue(Env *env)
        : IoValue { env, env->Object()->const_fetch("File")->as_class() } { }

    Value *initialize(Env *, Value *, Value *, Block *);

    static Value *open(Env *env, Value *filename, Value *flags_obj, Block *block) {
        Value *args[] = { filename, flags_obj };
        size_t argc = 1;
        if (flags_obj) argc++;
        return _new(env, env->Object()->const_fetch("File")->as_class(), argc, args, block);
    }

    static Value *expand_path(Env *env, Value *path, Value *root) {
        path->assert_type(env, Value::Type::String, "String");
        if (path->as_string()->length() > 0 && path->as_string()->c_str()[0] == '/') {
            return path;
        }
        StringValue *merged;
        if (root) {
            root = expand_path(env, root, nullptr);
            merged = StringValue::sprintf(env, "%S/%S", root, path);
        } else {
            char root[4096];
            if (getcwd(root, 4096)) {
                merged = StringValue::sprintf(env, "%s/%S", root, path);
            } else {
                env->raise("RuntimeError", "could not get current directory");
            }
        }
        // collapse ..
        RegexpValue dotdot { env, "[^/]*/\\.\\.(/|\\z)" };
        StringValue empty_string { env, "" };
        do {
            merged = merged->sub(env, &dotdot, &empty_string);
        } while (env->caller()->match());
        // remove trailing slash
        if (merged->length() > 1 && merged->c_str()[merged->length() - 1] == '/') {
            merged->truncate(merged->length() - 1);
        }
        return merged;
    }

    static Value *unlink(Env *env, Value *path) {
        int result = ::unlink(path->as_string()->c_str());
        if (result == 0) {
            return new IntegerValue { env, 1 };
        } else {
            Value *args[] = { new IntegerValue { env, errno } };
            auto exception = env->Object()->const_fetch("SystemCallError")->send(env, "exception", 1, args)->as_exception();
            env->raise_exception(exception);
        }
    }

    static void build_constants(Env *env, ClassValue *klass) {
        Value *Constants = new ModuleValue { env, "Constants" };
        klass->const_set(env, "Constants", Constants);
        klass->const_set(env, "APPEND", new IntegerValue { env, O_APPEND });
        Constants->const_set(env, "APPEND", new IntegerValue { env, O_APPEND });
        klass->const_set(env, "RDONLY", new IntegerValue { env, O_RDONLY });
        Constants->const_set(env, "RDONLY", new IntegerValue { env, O_RDONLY });
        klass->const_set(env, "WRONLY", new IntegerValue { env, O_WRONLY });
        Constants->const_set(env, "WRONLY", new IntegerValue { env, O_WRONLY });
        klass->const_set(env, "TRUNC", new IntegerValue { env, O_TRUNC });
        Constants->const_set(env, "TRUNC", new IntegerValue { env, O_TRUNC });
        klass->const_set(env, "CREAT", new IntegerValue { env, O_CREAT });
        Constants->const_set(env, "CREAT", new IntegerValue { env, O_CREAT });
        klass->const_set(env, "DSYNC", new IntegerValue { env, O_DSYNC });
        Constants->const_set(env, "DSYNC", new IntegerValue { env, O_DSYNC });
        klass->const_set(env, "EXCL", new IntegerValue { env, O_EXCL });
        Constants->const_set(env, "EXCL", new IntegerValue { env, O_EXCL });
        klass->const_set(env, "NOCTTY", new IntegerValue { env, O_NOCTTY });
        Constants->const_set(env, "NOCTTY", new IntegerValue { env, O_NOCTTY });
        klass->const_set(env, "NOFOLLOW", new IntegerValue { env, O_NOFOLLOW });
        Constants->const_set(env, "NOFOLLOW", new IntegerValue { env, O_NOFOLLOW });
        klass->const_set(env, "NONBLOCK", new IntegerValue { env, O_NONBLOCK });
        Constants->const_set(env, "NONBLOCK", new IntegerValue { env, O_NONBLOCK });
        klass->const_set(env, "RDWR", new IntegerValue { env, O_RDWR });
        Constants->const_set(env, "RDWR", new IntegerValue { env, O_RDWR });
        klass->const_set(env, "SYNC", new IntegerValue { env, O_SYNC });
        Constants->const_set(env, "SYNC", new IntegerValue { env, O_SYNC });
    }

    static bool exist(Env *env, Value *path) {
        struct stat sb;
        path->assert_type(env, Value::Type::String, "String");
        return stat(path->as_string()->c_str(), &sb) != -1;
    }

    const char *path() { return m_path; }
    void set_path(const char *path) { m_path = path; }

private:
    const char *m_path { nullptr };
};

}
