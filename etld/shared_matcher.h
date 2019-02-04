/* Copyright (c) 2018 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_SHARED_MATCHER_H_
#define ETLD_SHARED_MATCHER_H_

#include "./matcher.h"
#include "./types.h"

namespace Brave {
namespace eTLD {

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
  Brave::eTLD::Matcher matcher_ = Matcher::FromFilePath(
    "./etld/data/public_suffix_list.dat");
};

}  // namespace eTLD
}  // namespace Brave

#endif  // ETLD_SHARED_MATCHER_H_
