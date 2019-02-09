/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_MATCHER_H_
#define ETLD_MATCHER_H_

#include <string>
#include <fstream>
#include "etld/domain.h"
#include "etld/types.h"
#include "etld/parser.h"
#include "etld/public_suffix_rule.h"
#include "etld/public_suffix_rule_set.h"

namespace Brave {
namespace eTLD {

class Matcher {
 public:
  static Matcher FromFilePath(const std::string &file_path) {
    std::ifstream rule_file;
    rule_file.open(file_path, std::ifstream::in);
    return Matcher(rule_file);
  }
  explicit Matcher(std::ifstream &rule_file);
  explicit Matcher(const std::string &rule_text);
  explicit Matcher(const PublicSuffixParseResult &rules);

  DomainInfo Match(const Domain &domain) const;

 private:
  DomainInfo BuildDomainInfo(const PublicSuffixRule &rule,
    const Domain &domain) const;
  void ConsumeRules(const PublicSuffixParseResult &rules);
  PublicSuffixRuleSet exception_rules_;
  PublicSuffixRuleSet rules_;
};

}  // namespace eTLD
}  // namespace Brave

#endif  // ETLD_MATCHER_H_
