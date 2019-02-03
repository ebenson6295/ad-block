/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <fstream>

#include "./types.h"
#include "./parser.h"
#include "./matcher.h"

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
  PublicSuffixRuleSetMatchResult exception_rule_match = exception_rules_.Match(domain);
  if (exception_rule_match.found_match) {
    return BuildDomainInfo(exception_rule_match.rule, domain);
  }

  PublicSuffixRuleSetMatchResult rule_match = rules_.Match(domain);
  if (rule_match.found_match) {
    return BuildDomainInfo(rule_match.rule, domain);
  }

  return BuildDomainInfo(PublicSuffixRule("*"), domain);
}

DomainInfo Matcher::BuildDomainInfo(const PublicSuffixRule &rule, const Domain &domain) const {
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

}
}