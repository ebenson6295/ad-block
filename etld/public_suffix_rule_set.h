/* Copyright (c) 2018 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_ETLD_PUBLIC_SUFFIX_RULE_SET_H_
#define BRAVE_ETLD_PUBLIC_SUFFIX_RULE_SET_H_

#include <map>
#include <string>
#include <vector>
#include "./domain.h"
#include "./public_suffix_rule.h"

namespace Brave {
namespace eTLD {

struct PublicSuffixRuleSetMatchResult {
  bool found_match;
  PublicSuffixRule rule;
};

class PublicSuffixRuleSet {
  public:
    PublicSuffixRuleSet() {};
    void AddRule(const PublicSuffixRule &rule);
    PublicSuffixRuleSetMatchResult Match(const Domain &domain) const;

  private:
    std::vector<PublicSuffixRule> rules_;
};

}
}

#endif