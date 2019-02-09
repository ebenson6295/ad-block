/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_MATCHER_H_
#define ETLD_MATCHER_H_

#include <string>
#include <fstream>
#include <vector>
#include "etld/domain.h"
#include "etld/types.h"
#include "etld/public_suffix_rule.h"
#include "etld/public_suffix_rule_set.h"

namespace brave_etld {

class Matcher {
 public:
  explicit Matcher(const std::vector<PublicSuffixRule> &rules);
  explicit Matcher(
    const std::vector<PublicSuffixRuleSerialized> &serialized_rules);

  DomainInfo Match(const Domain &domain) const;

 private:
  DomainInfo BuildDomainInfo(const PublicSuffixRule &rule,
    const Domain &domain) const;
  void ConsumeRules(const std::vector<PublicSuffixRule> &rules);
  PublicSuffixRuleSet exception_rules_;
  PublicSuffixRuleSet rules_;
};

}  // namespace brave_etld

#endif  // ETLD_MATCHER_H_
