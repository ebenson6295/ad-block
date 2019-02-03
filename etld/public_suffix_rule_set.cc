/* Copyright (c) 2018 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include <string>
#include <map>
#include "./domain.h"
#include "./public_suffix_rule.h"
#include "./public_suffix_rule_set.h"
#include "./types.h"

using std::cout;
using std::endl;

namespace Brave {
namespace eTLD {

PublicSuffixRuleSetMatchResult PublicSuffixRuleSet::Match(const Domain &domain) const {
  int longest_length = -1;
  int found_length;
  PublicSuffixRule longest_rule;
  for (auto &elm : rules_) {
    if (elm.Matches(domain)) {
      found_length = elm.Labels().size();
      if (found_length > longest_length) {
        longest_rule = elm;
        longest_length = found_length;
      }
    }
  }

  PublicSuffixRuleSetMatchResult result;

  if (longest_length == -1) {
    result.found_match = false;
    return result;
  }

  result.found_match = true;
  result.rule = longest_rule;
  return result;
}


void PublicSuffixRuleSet::AddRule(const PublicSuffixRule &rule) {
  rules_.push_back(rule);
}

}
}