/*
 * Copyright 2020 Redpanda Data, Inc.
 *
 * Use of this software is governed by the Business Source License
 * included in the file licenses/BSL.md
 *
 * As of the Change Date specified in that file, in accordance with
 * the Business Source License, use of this software will be governed
 * by the Apache License, Version 2.0
 */

#pragma once

#include "config/property.h"
#include "raft/types.h"
#include "random/simple_time_jitter.h"

namespace raft {
class timeout_jitter {
public:
    explicit timeout_jitter(config::binding<std::chrono::milliseconds> timeout)
      : _base_timeout(timeout)
      , _time_jitter(_base_timeout()) {
        timeout.watch([this] { update_base_timeout(); });
    }
    raft::clock_type::time_point operator()() { return _time_jitter(); }

    raft::clock_type::duration base_duration() const {
        return _time_jitter.base_duration();
    }

    raft::clock_type::duration next_duration() {
        return _time_jitter.next_duration();
    }

    raft::clock_type::duration next_jitter_duration() {
        return _time_jitter.next_jitter_duration();
    }

private:
    void update_base_timeout() {
        _time_jitter
          = simple_time_jitter<raft::clock_type, raft::duration_type>(
            _base_timeout());
    }

    config::binding<std::chrono::milliseconds> _base_timeout;
    simple_time_jitter<raft::clock_type, raft::duration_type> _time_jitter;
};

} // namespace raft
