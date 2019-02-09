/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_PUBLIC_SUFFIX_RULE_H_
#define ETLD_PUBLIC_SUFFIX_RULE_H_

#include <string>
#include <vector>
#include <exception>
#include "etld/domain.h"
#include "etld/types.h"

namespace brave_etld {

struct PublicSuffixRuleSerialized {
  explicit PublicSuffixRuleSerialized(const std::vector<Label> &labels) :
      is_wildcard(false),
      is_exception(false),
      labels(labels) {}

  PublicSuffixRuleSerialized(
    bool is_wildcard,
    bool is_exception,
    const std::vector<Label> &labels) :
      is_wildcard(is_wildcard),
      is_exception(is_exception),
      labels(labels) {}

  bool is_wildcard;
  bool is_exception;
  const std::vector<Label> &labels;
};

class PublicSuffixRule {
 public:
  PublicSuffixRule() {}
  explicit PublicSuffixRule(const PublicSuffixRuleSerialized &serialized_rule);
  explicit PublicSuffixRule(
      std::vector<Label> labels,
      bool is_exception = false,
      bool is_wildcard = false) :
    labels_(labels),
    is_exception_(is_exception),
    is_wildcard_(is_wildcard) {}

  bool Equals(const PublicSuffixRule &rule) const;
  bool Matches(const Domain &domain) const;
  DomainInfo Apply(const Domain &domain) const;
  std::string ToString() const;
  std::string DomainString() const;

  std::vector<Label> Labels() const {
    return labels_;
  }
  std::vector<Label>::size_type Length() const {
    return labels_.size();
  }
  bool IsException() const {
    return is_exception_;
  }
  bool IsWildcard() const {
    return is_wildcard_;
  }

 protected:
  std::vector<Label> labels_;
  bool is_exception_ = false;
  bool is_wildcard_ = false;
};

}  // namespace brave_etld

#endif  // ETLD_PUBLIC_SUFFIX_RULE_H_
