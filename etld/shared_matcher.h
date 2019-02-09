/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_SHARED_MATCHER_H_
#define ETLD_SHARED_MATCHER_H_

#include "etld/matcher.h"
#include "etld/types.h"
#include "etld/data/public_suffix_list.h"

namespace brave_etld {

class SharedETLDMatcher {
 public:
  static SharedETLDMatcher& GetInstance() {
    static SharedETLDMatcher instance;
    return instance;
  }
  SharedETLDMatcher(SharedETLDMatcher const&)= delete;
  void operator=(SharedETLDMatcher const&)= delete;

  Matcher GetMatcher() const {
    return matcher_;
  }

 private:
  SharedETLDMatcher() {}
  Matcher matcher_ = Matcher(SHARED_RULES);
};

}  // namespace brave_etld

#endif  // ETLD_SHARED_MATCHER_H_
