/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <fstream>

#include "etld/types.h"
#include "etld/public_suffix_rule.h"
#include "etld/matcher.h"

namespace brave_etld {

Matcher::Matcher(const std::vector<PublicSuffixRule> &rules) {
  ConsumeRules(rules);
}

Matcher::Matcher(const std::vector<PublicSuffixRuleSerialized> &serialized_rules) {
  std::vector<PublicSuffixRule> rules;
  for (auto &elm : serialized_rules) {
    rules.push_back(PublicSuffixRule(elm));
  }
  ConsumeRules(rules);
}

// Attempts to implement the algoritms described here:
//   https://publicsuffix.org/list/
DomainInfo Matcher::Match(const Domain &domain) const {
  PublicSuffixRuleSetMatchResult except_match = exception_rules_.Match(domain);
  if (except_match.found_match) {
    return BuildDomainInfo(except_match.rule, domain);
  }

  PublicSuffixRuleSetMatchResult rule_match = rules_.Match(domain);
  if (rule_match.found_match) {
    return BuildDomainInfo(rule_match.rule, domain);
  }

  return BuildDomainInfo(PublicSuffixRule({"*"}), domain);
}

DomainInfo Matcher::BuildDomainInfo(const PublicSuffixRule &rule,
    const Domain &domain) const {
  return rule.Apply(domain);
}

void Matcher::ConsumeRules(const std::vector<PublicSuffixRule> &rules) {
  for (auto &elm : rules) {
    if (elm.IsException()) {
      exception_rules_.AddRule(elm);
    } else {
      rules_.AddRule(elm);
    }
  }
}

}  // namespace brave_etld
