// Copyright 2010-2015 RethinkDB, all rights reserved.
#ifndef RDB_PROTOCOL_QUERY_HPP_
#define RDB_PROTOCOL_QUERY_HPP_

#include "rapidjson/rapidjson.h"

#include "concurrency/new_semaphore.hpp"
#include "containers/intrusive_list.hpp"
#include "rdb_protocol/error.hpp"
#include "rdb_protocol/ql2.pb.h"
#include "rdb_protocol/term_storage.hpp"

namespace ql {

class query_cache_t;

class query_params_t {
public:
    query_params_t(int64_t _token,
                   ql::query_cache_t *_query_cache,
                   scoped_array_t<char> &&_original_data,
                   rapidjson::Document &&_query_json);

    // A query id is allocated when each query is received from the client
    // in order, so order can be checked for in queries that require it
    class query_id_t : public intrusive_list_node_t<query_id_t> {
    public:
        explicit query_id_t(query_cache_t *_parent);
        query_id_t(query_id_t &&other);
        ~query_id_t();

        uint64_t value() const;

    private:
        query_cache_t *parent;
        uint64_t value_;
        DISABLE_COPYING(query_id_t);
    };

    void maybe_release_query_id();

    query_cache_t *query_cache;
    term_storage_t term_storage;
    query_id_t id;

    int64_t token;
    Query::QueryType type;
    bool noreply;
    bool profile;

    new_semaphore_acq_t throttler;

private:
    DISABLE_COPYING(query_params_t);
};

} // namespace ql

#endif // RDB_PROTOCOL_QUERY_HPP_
