/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <fstream>

#include "etld/types.h"
#include "etld/parser.h"
#include "etld/matcher.h"

namespace Brave {
namespace eTLD {

Matcher::Matcher(std::ifstream &rule_file) {
  PublicSuffixParseResult rules = parse_rule_file(rule_file);
  ConsumeRules(rules);
}

Matcher::Matcher(const std::string &rule_text) {
  PublicSuffixParseResult rules = parse_rule_text(rule_text);
  ConsumeRules(rules);
}

Matcher::Matcher(const PublicSuffixParseResult &rules) {
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

  return BuildDomainInfo(PublicSuffixRule("*"), domain);
}

DomainInfo Matcher::BuildDomainInfo(const PublicSuffixRule &rule,
    const Domain &domain) const {
  return rule.Apply(domain);
}

void Matcher::ConsumeRules(const PublicSuffixParseResult &rules) {
  for (auto &elm : rules.Rules()) {
    if (elm.IsException()) {
      exception_rules_.AddRule(elm);
    } else {
      rules_.AddRule(elm);
    }
  }
}

}  // namespace eTLD
}  // namespace Brave
